#pragma once
#include "CommandList.h"

#include <Base/Types.h>

#include <functional>

namespace Renderer
{
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
    };

    template <typename PassData>
    class RenderPass : public IRenderPass
    {
    public:
        typedef std::function<bool(PassData&, RenderGraphBuilder&)> SetupFunction;
        typedef std::function<void(PassData&, RenderGraphResources&, CommandList&)> ExecuteFunction;
    
        RenderPass(std::string& name, SetupFunction& onSetup, ExecuteFunction& onExecute)
            : _onSetup(std::move(onSetup))
            , _onExecute(std::move(onExecute))
        {
            if (name.length() >= 32)
            {
                DebugHandler::PrintFatal("We encountered a render pass name (%s) that is longer than 31 characters, we have this limit because we store the string internally and not on the heap.", name.c_str());
            }

            strcpy_s(_name, name.c_str());
            _nameLength = static_cast<u8>(name.length());
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
