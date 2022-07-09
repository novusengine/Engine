#pragma once
#include <Base/Types.h>

#include <robinhood/robinhood.h>

#include <filesystem>

namespace ShaderCooker
{
    class ShaderCache
    {
    public:
        bool Load(std::filesystem::path path);
        void Save(std::filesystem::path path);

        void Touch(std::filesystem::path shaderPath);
        bool HasChanged(std::filesystem::path shaderPath);

    private:
        robin_hood::unordered_map<u32, std::time_t> _filesLastTouchTime;
    };
}