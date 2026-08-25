#pragma once
#include "CommandList.h"

#include <Base/Types.h>

#include <functional>

namespace Renderer
{
    enum class RenderPassFlags : u8
    {
        None = 0,
        SideEffect = 1u << 0u
    };

    inline bool HasRenderPassFlag(RenderPassFlags flags, RenderPassFlags flag)
    {
        return (static_cast<u8>(flags) & static_cast<u8>(flag)) != 0;
    }

    class Renderer;
    class RenderLayer;
    class RenderGraph;
    class RenderGraphBuilder;
    class RenderGraphResources;

    class IRenderPass
    {
    public:
        virtual bool Setup(RenderGraphBuilder* renderGraphBuilder) = 0;
        virtual void Execute(RenderGraphResources& resources, CommandList& commandList) = 0;
        virtual void DeInit() = 0;

        char _name[32];
        u8 _nameLength = 0;
        RenderPassFlags _flags = RenderPassFlags::None;
    };

    template <typename PassData>
    class RenderPass : public IRenderPass
    {
    public:
        typedef std::function<bool(PassData&, RenderGraphBuilder&)> SetupFunction;
        typedef std::function<void(PassData&, RenderGraphResources&, CommandList&)> ExecuteFunction;
    
        RenderPass(std::string& name, SetupFunction& onSetup, ExecuteFunction& onExecute, RenderPassFlags flags)
            : _onSetup(std::move(onSetup))
            , _onExecute(std::move(onExecute))
        {
            _flags = flags;
            if (name.length() < sizeof(_name))
            {
                strcpy_s(_name, name.c_str());
            }
            else
            {
                constexpr size_t PREFIX_LENGTH = 22;
                u32 hash = 2166136261u;
                for (const char character : name)
                {
                    hash ^= static_cast<u8>(character);
                    hash *= 16777619u;
                }
                sprintf_s(_name, "%.*s~%08X", static_cast<i32>(PREFIX_LENGTH), name.c_str(), hash);
            }
            _nameLength = static_cast<u8>(strlen(_name));
        }

    private:
        bool Setup(RenderGraphBuilder* renderGraphBuilder) override
        {
            return _onSetup(_data, *renderGraphBuilder);
        }

        void Execute(RenderGraphResources& resources, CommandList& commandList) override
        {
            _onExecute(_data, resources, commandList);
        }

        void DeInit() override
        {
            _onSetup = nullptr;
            _onExecute = nullptr;
        }
    private:

    private:
        SetupFunction _onSetup;
        ExecuteFunction _onExecute;

        PassData _data;
    };
}
