#include "RenderGraph.h"
#include "Renderer.h"

#include <Base/Memory/Allocator.h>
#include <Base/Container/DynamicArray.h>

#include <tracy/Tracy.hpp>

namespace Renderer
{
    struct RenderGraphData : IRenderGraphData
    {
        RenderGraphData(Memory::Allocator* allocator)
            : passes(allocator, 32)
            , executingPasses(allocator, 32)
            , signalSemaphores(allocator, 4)
            , waitSemaphores(allocator, 4)
        {

        }

        DynamicArray<IRenderPass*> passes;
        DynamicArray<IRenderPass*> executingPasses;

        DynamicArray<SemaphoreID> signalSemaphores;
        DynamicArray<SemaphoreID> waitSemaphores;
    };

    RenderGraph::RenderGraph(Memory::Allocator* allocator, Renderer* renderer)
        : _data(Memory::Allocator::New<RenderGraphData>(allocator, allocator))
        , _renderer(renderer)
        , _renderGraphBuilder(nullptr)
    {

    } // This gets friend-created by Renderer

    bool RenderGraph::Init(RenderGraphDesc& desc)
    {
        _desc = desc;

        if (desc.allocator == nullptr)
        {
            DebugHandler::PrintFatal("You need to set an allocator!");
        }

        return true;
    }

    void RenderGraph::AddPass(IRenderPass* pass)
    {
        RenderGraphData* data = static_cast<RenderGraphData*>(_data);
        data->passes.Insert(pass);
    }

    RenderGraph::~RenderGraph()
    {
        RenderGraphData* data = static_cast<RenderGraphData*>(_data);
        for (IRenderPass* pass : data->passes)
        {
            pass->DeInit();
        }
    }

    void RenderGraph::AddSignalSemaphore(SemaphoreID semaphoreID)
    {
        RenderGraphData* data = static_cast<RenderGraphData*>(_data);
        data->signalSemaphores.Insert(semaphoreID);
    }

    void RenderGraph::AddWaitSemaphore(SemaphoreID semaphoreID)
    {
        RenderGraphData* data = static_cast<RenderGraphData*>(_data);
        data->waitSemaphores.Insert(semaphoreID);
    }

    void RenderGraph::Setup()
    {
        ZoneScopedNC("RenderGraph::Setup", tracy::Color::Red2);

        RenderGraphData* data = static_cast<RenderGraphData*>(_data);

        size_t numPasses = data->passes.Count();
        _renderGraphBuilder = Memory::Allocator::New<RenderGraphBuilder>(_desc.allocator, _desc.allocator, _renderer, numPasses);

        for (u32 i = 0; i < data->passes.Count(); i++)
        {
            IRenderPass* pass = data->passes[i];

            ZoneScopedC(tracy::Color::Red2);
            ZoneName(pass->_name, pass->_nameLength);

            _renderGraphBuilder->SetCurrentPassIndex(i);
            if (pass->Setup(_renderGraphBuilder))
            {
                data->executingPasses.Insert(pass);
            }
        }
    }

    void RenderGraph::Execute()
    {
        ZoneScopedNC("RenderGraph::Execute", tracy::Color::Red2);

        RenderGraphData* data = static_cast<RenderGraphData*>(_data);
        RenderGraphResources& resources = _renderGraphBuilder->GetResources();
        
        CommandList commandList(_renderer, _desc.allocator);

        // Add semaphores
        for (SemaphoreID signalSemaphore : data->signalSemaphores)
        {
            commandList.AddSignalSemaphore(signalSemaphore);
        }

        for (SemaphoreID waitSemaphore : data->waitSemaphores)
        {
            commandList.AddWaitSemaphore(waitSemaphore);
        }

        _renderer->BeginExecutingCommandlist();

        commandList.PushMarker("RenderGraph", Color::PastelBlue);
        for (u32 i = 0; i < data->passes.Count(); i++)
        {
            IRenderPass* pass = data->passes[i];

            ZoneScopedC(tracy::Color::Red2);
            ZoneName(pass->_name, pass->_nameLength);

            commandList.PushMarker(pass->_name, Color::PastelGreen);

            _renderGraphBuilder->PreExecute(commandList, i);
            pass->Execute(resources, commandList);
            _renderGraphBuilder->PostExecute(commandList, i);

            commandList.PopMarker();
        }
        commandList.PopMarker();

        {
            ZoneScopedNC("CommandList::Execute", tracy::Color::Red2);
            commandList.Execute();
        }

        _renderer->EndExecutingCommandlist();
    }
}