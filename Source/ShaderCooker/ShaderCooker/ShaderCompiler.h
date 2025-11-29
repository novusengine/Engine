#pragma once
#include "Permutation.h"

#include <Base/Types.h>
#include <Base/Container/ConcurrentQueue.h>

#include <robinhood/robinhood.h>

#include <thread>
#include <shared_mutex>
#include <filesystem>

namespace ShaderCooker
{
    class ShaderCache;

    class ShaderCompiler
    {
    public:
        enum class Stage
        {
            STOPPED,
            IDLE,
            LEXER_SYNTAX_IMPORT,
            INCLUDE_RESOLVE,
            COMPILATION
        };

        bool Start();
        bool Stop();
        bool Process();

        bool AddPath(std::filesystem::path paths);
        bool AddPaths(std::vector<std::filesystem::path> paths);
        bool AddInclude(std::filesystem::path shaderPath, std::filesystem::path includePath);

        void SetShaderCache(ShaderCooker::ShaderCache* shaderCache) { _shaderCache = shaderCache; }

        std::filesystem::path GetSourceDirPath() { return _sourceDirPath; }
        void SetSourceDirPath(std::filesystem::path sourceDirPath) { _sourceDirPath = sourceDirPath; }

        void SetBinDirPath(std::filesystem::path binDirPath) { _binDirPath = binDirPath; }

        Stage GetStage() { return _stageInfo.stage; }
        void SetStage(Stage stage) { _stageInfo.stage = stage; }

        u32 GetNumCompiledShaders() { return _numCompiledShaders; }
        u32 GetNumFailedShaders() { return _numFailedShaders; }

        void SetShouldForceCompile(bool shouldForceCompile) { _shouldForceCompile = shouldForceCompile; }
        void SetDebugOutputSPV(bool debugOutputSPV) { _debugOutputSPV = debugOutputSPV; }

    private:
        void Run();
        bool Compile(std::string& path);

        bool HasShader(u32 hash);
        bool HasShader(u32 hash, u32& index);
        bool GetShaderByIndex(u32 index, Shader& shader);

        void GetPermutationFilename(const Shader& shader, u32 permutationID, const std::string& inputFileName, std::string& filename);

    private:
        ShaderCooker::ShaderCache* _shaderCache;
        std::filesystem::path _sourceDirPath;
        std::filesystem::path _binDirPath;

        std::vector<Shader> _shaders;
        robin_hood::unordered_map<u32, std::vector<std::filesystem::path>> _shaderHashToPaths;
        robin_hood::unordered_map<u32, u32> _shaderHashToIndex;

        u32 _numCompiledShaders = 0;
        u32 _numFailedShaders = 0;

        struct StageState
        {
            Stage stage = Stage::STOPPED;

            std::vector<std::filesystem::path> paths;
            std::shared_mutex mutex;
        };

        StageState _stageInfo;

        struct Message
        {
            enum class Type
            {
                NONE,
                STOP,
                PROCESS
            };

            Type type;
        };
        moodycamel::ConcurrentQueue<Message> _messages;

        bool _shouldForceCompile = false;
        bool _debugOutputSPV = false;
    };
}