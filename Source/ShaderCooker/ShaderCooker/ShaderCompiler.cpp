#include "ShaderCompiler.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "SlangBridge.h"
#include "ShaderCache.h"

#include <Base/Memory/FileReader.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

#include <FileFormat/Novus/ShaderPack/ShaderPack.h>

namespace fs = std::filesystem;

namespace ShaderCooker
{
    bool ShaderCompiler::Start()
    {
        if (_stageInfo.stage != Stage::STOPPED)
            return false;

        _stageInfo.stage = Stage::IDLE;
        _stageInfo.paths.reserve(32);

        //std::thread runThread = std::thread(&Compiler::Run, this);
        //runThread.detach();

        _shaders.clear();
        _shaderHashToPaths.clear();
        _shaderHashToIndex.clear();
        _numCompiledShaders = 0;
        _numFailedShaders = 0;

        return true;
    }

    bool ShaderCompiler::Stop()
    {
        Message stopMessage;
        stopMessage.type = Message::Type::STOP;

        _messages.enqueue(stopMessage);
        return true;
    }

    bool ShaderCompiler::Process()
    {
        if (_stageInfo.stage != Stage::IDLE)
            return false;

        if (_stageInfo.paths.size() == 0)
            return false;

        Message processMessage;
        processMessage.type = Message::Type::PROCESS;

        _messages.enqueue(processMessage);

        Run();

        return true;
    }

    bool ShaderCompiler::AddPath(std::filesystem::path path)
    {
        if (_stageInfo.stage != Stage::IDLE)
            return false;

        path = fs::absolute(path);
        _stageInfo.paths.push_back(path);

        return true;
    }

    bool ShaderCompiler::AddPaths(std::vector<fs::path> paths)
    {
        if (_stageInfo.stage != Stage::IDLE)
            return false;

        size_t numPaths = _stageInfo.paths.size();
        size_t numNewPaths = paths.size();

        _stageInfo.paths.reserve(numPaths + numNewPaths);

        for (fs::path& path : paths)
        {
            path = fs::absolute(path);
            _stageInfo.paths.push_back(path);
        }
        return true;
    }

    bool ShaderCompiler::AddInclude(fs::path shaderPath, fs::path includePath)
    {
        if (_stageInfo.stage != Stage::LEXER_SYNTAX_IMPORT)
            return false;

        std::string includePathString = includePath.make_preferred().string();

        u32 strHash = StringUtils::fnv1a_32(includePathString.c_str(), includePathString.length());
        _shaderHashToPaths[strHash].push_back(shaderPath);

        return true;
    }

    bool ShaderCompiler::HasShader(u32 hash)
    {
        return _shaderHashToIndex.find(hash) != _shaderHashToIndex.end();
    }

    bool ShaderCompiler::HasShader(u32 hash, u32& index)
    {
        auto moduleItr = _shaderHashToIndex.find(hash);
        if (moduleItr == _shaderHashToIndex.end())
            return false;

        index = moduleItr->second;
        return true;
    }

    bool ShaderCompiler::GetShaderByIndex(u32 index, Shader& shader)
    {
        if (index >= _shaders.size())
            return false;

        shader = _shaders[index];
        return true;
    }

    void ShaderCompiler::Run()
    {
        bool isRunning = true;

        while (isRunning)
        {
            Message message;
            while (_messages.try_dequeue(message))
            {
                if (message.type == Message::Type::STOP)
                {
                    isRunning = false;
                    break;
                }
                else if (message.type == Message::Type::PROCESS)
                {
                    _stageInfo.stage = Stage::LEXER_SYNTAX_IMPORT;
                }
            }

            if (isRunning == false)
                break;

            Stage currentStage = _stageInfo.stage;
            if (currentStage == Stage::IDLE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            else if (currentStage == Stage::LEXER_SYNTAX_IMPORT)
            {
                for (auto itr = _stageInfo.paths.begin(); itr != _stageInfo.paths.end();)
                {
                    std::string fileName = itr->filename().string();
                    std::string path = itr->make_preferred().string();
                    if (fs::exists(*itr))
                    {
                        if (fs::is_regular_file(*itr) && itr->extension() == ".hlsl")
                        {
                            //std::transform(path.begin(), path.end(), path.begin(), ::tolower);
                            u32 strHash = StringUtils::fnv1a_32(path.c_str(), path.length());

                            if (HasShader(strHash) == false)
                            {
                                Shader& shader = _shaders.emplace_back();

                                std::ifstream file(itr->string());
                                shader.source = std::string((std::istreambuf_iterator<char>(file)),
                                    (std::istreambuf_iterator<char>()));
                                file.close();

                                if (shader.source.length() == 0)
                                {
                                    _shaders.pop_back();
                                    NC_LOG_ERROR("Compiler failed to read script ({0})", itr->string().c_str());
                                    itr = _stageInfo.paths.erase(itr);
                                    _numFailedShaders++;
                                    continue;
                                }

                                shader.name = itr->filename().string();
                                shader.path = path;
                                shader.pathHash = strHash;
                                shader.parentPath = itr->parent_path();
                                shader.fileBuffer = &shader.source[0];
                                shader.fileBufferSize = shader.source.length();

                                if (!Lexer::Process(shader) || !Parser::CheckSyntax(shader) || !Parser::ResolveIncludes(this, shader))
                                    _shaders.pop_back();
                                else
                                    _shaderHashToIndex[strHash] = static_cast<u32>(_shaders.size() - 1);
                            }
                        }

                        itr = _stageInfo.paths.erase(itr);
                        continue;
                    }

                    NC_LOG_ERROR("Compiler failed to find path ({0})", path.c_str());
                    _numFailedShaders++;
                }

                _stageInfo.stage = Stage::INCLUDE_RESOLVE;
            }
            else if (_stageInfo.stage == Stage::INCLUDE_RESOLVE)
            {
                while (true)
                {
                    u32 counter = 0;

                    for (auto& shader : _shaders)
                    {
                        // If we have resolved this one, continue
                        if (shader.hasResolvedIncludes)
                            continue;

                        // If timestamp hasn't changed, continue
                        if (!_shouldForceCompile && (!_shaderCache->HasChanged(shader.path) && !shader.markedForChange))
                            continue;

                        auto iterator = _shaderHashToPaths.find(shader.pathHash);

                        if (iterator != _shaderHashToPaths.end())
                        {
                            for (auto& path : iterator->second)
                            {
                                u32 strHash = StringUtils::fnv1a_32(path.string().c_str(), path.string().length());
                                u32 index = _shaderHashToIndex[strHash];
                                Shader& shaderThatIncludesThisOne = _shaders[index];
                                shaderThatIncludesThisOne.markedForChange = true;
                            }
                        }

                        counter++;
                        shader.hasResolvedIncludes = true;
                    }

                    if (counter == 0)
                    {
                        break;
                    }
                    counter = 0;
                }

                _stageInfo.stage = Stage::COMPILATION;
            }
            else if (_stageInfo.stage == Stage::COMPILATION)
            {
                for (Shader& shader : _shaders)
                {
                    if (!shader.hasResolvedIncludes)
                        continue;

                    if (StringUtils::EndsWith(shader.name, ".inc.hlsl"))
                    {
                        _shaderCache->Touch(shader.path);
                        continue;
                    }

                    ShaderCooker::SlangBridge slangBridge(_sourceDirPath);

                    // Figure out the actual permutations
                    u32 numPermutationGroups = static_cast<u32>(shader.permutationGroups.size());

                    std::vector<u32> indices;
                    indices.resize(numPermutationGroups);

                    while (1)
                    {
                        Permutation& permutation = shader.permutations.emplace_back();
                        for (u32 j = 0; j < numPermutationGroups; j++)
                        {
                            u32 index = indices[j];

                            std::string value = { shader.permutationGroups[j].types[index].nameHash.name, shader.permutationGroups[j].types[index].nameHash.length };
                            permutation.defines.push_back({ .name = shader.permutationGroups[j].name, .value = value });
                        }

                        // Find the rightmost permutationGroup that has more elements left after the current element in that group
                        i32 next = numPermutationGroups - 1;
                        while (next >= 0 && indices[next] + 1 >= shader.permutationGroups[next].types.size())
                        {
                            next--;
                        }

                        // No such group was found so no more permutations left
                        if (next < 0)
                        {
                            break;
                        }

                        // If we found it, move to the next type of that permutationGroup
                        indices[next]++;

                        // For all groups to the right of this group, set the index back to the first element
                        for (u32 j = next + 1; j < numPermutationGroups; j++)
                        {
                            indices[j] = 0;
                        }
                    }

                    // If we didn't parse any permutations we need to make a dummy permutation for the compilation step, this does not require defines
                    if (shader.permutations.size() == 0)
                    {
                        shader.permutations.emplace_back();
                    }

                    u32 numPermutations = static_cast<u32>(shader.permutations.size());
                    std::vector<FileFormat::ShaderInMemory> shaderOutputs(numPermutations);

                    // If we found a permutation block at the top of the shader we need to strip it
                    if (numPermutationGroups > 0)
                    {
                        size_t endOfPermutationBlockOffset = shader.permutationGroups[numPermutationGroups - 1].endOffset + 1;
                        shader.source = shader.source.substr(endOfPermutationBlockOffset);
                    }

                    bool didFail = false;
                    // Compile permutations
                    for (u32 i = 0; i < numPermutations; i++)
                    {
                        Permutation& permutation = shader.permutations[i];

                        slangBridge.ClearDefines();
                        for (Define& define : permutation.defines)
                        {
                            slangBridge.AddDefine(define.name, define.value);
                        }

                        std::string name = fs::relative(shader.path, _sourceDirPath).string();
                        if (i == 0)
                        {
                            if (numPermutations > 1)
                            {
                                NC_LOG_INFO("Compiling: {0} ({1} permutations)", name.c_str(), numPermutations);
                            }
                            else
                            {
                                NC_LOG_INFO("Compiling: {0}", name.c_str());
                            }
                        }

                        std::string permutationName;
                        GetPermutationFilename(shader, i, name, permutationName);
                        u32 permutationNameHash = StringUtils::fnv1a_32(permutationName.c_str(), permutationName.length());
                        shaderOutputs[i].permutationNameHash = permutationNameHash;

                        if (!slangBridge.Compile(shader.path, shader.source, shaderOutputs[i]))
                        {
                            NC_LOG_ERROR("{0} Failed to compile with Slang", permutationName.c_str());
                            didFail = true;
                            break;
                        }

                        if (_debugOutputSPV)
                        {
                            std::filesystem::path outputPath = _binDirPath / (permutationName + ".spv");
                            outputPath = std::filesystem::absolute(outputPath.make_preferred());

                            // Create output directories
                            std::filesystem::create_directories(outputPath.parent_path());

                            // Output file
                            std::ofstream ofstream(outputPath, std::ios::trunc | std::ofstream::binary);
                            ofstream.write(reinterpret_cast<const char*>(shaderOutputs[i].data), shaderOutputs[i].size);
                            ofstream.close();
                        }
                    }
                    _numFailedShaders += static_cast<u32>(didFail);
                    _shaderCache->Touch(shader.path);

                    if (!didFail)
                    {
                        std::filesystem::path outputPath = _binDirPath / fs::relative(shader.path, _sourceDirPath);
                        outputPath.replace_extension(FileFormat::SHADER_PACK_EXTENSION);

                        std::filesystem::create_directories(outputPath.parent_path());

                        FileFormat::ShaderPack shaderPack;
                        shaderPack.Save(outputPath.string(), shaderOutputs);

                        _numCompiledShaders++;
                    }
                }
                break;
            }
            else
            {
                // Unsupported Stage
                break;
            }

            std::this_thread::yield();
        }

        _stageInfo.stage = Stage::STOPPED;
    }

    bool ShaderCompiler::Compile(std::string& path)
    {
        fs::path filePath = path;

        return true;
    }

    void ShaderCompiler::GetPermutationFilename(const Shader& shader, u32 permutationID, const std::string& inputFileName, std::string& filename)
    {
        filename = inputFileName;
        size_t offset = filename.find_first_of('.');

        std::string extension = filename.substr(offset);
        filename = filename.substr(0, offset);

        const Permutation& permutation = shader.permutations[permutationID];

        for (u32 i = 0; i < permutation.defines.size(); i++)
        {
            const Define& define = permutation.defines[i];
            filename += "-" + define.name + define.value;
        }

        filename += extension;
        std::replace(filename.begin(), filename.end(), '\\', '/');
    }

}
