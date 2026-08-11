#include "RenderGraph.h"
#include "Renderer.h"

#include <Base/Memory/Allocator.h>
#include <Base/Container/DynamicArray.h>
#include <Base/CVarSystem/CVarSystem.h>

#include <tracy/Tracy.hpp>
#include <chrono>

namespace Renderer
{
    namespace
    {
        class RenderGraphResourceSet
        {
        public:
            RenderGraphResourceSet(Memory::Allocator* allocator, u32 numBits)
                : _numSets(std::max(1u, (numBits + 63u) / 64u))
                , _sets(_numSets <= INLINE_SET_COUNT ? _inlineSets : Memory::Allocator::NewArray<u64>(allocator, _numSets))
            {
                memset(_sets, 0, sizeof(u64) * _numSets);
            }

            void Set(u32 index) { _sets[index / 64u] |= 1ull << (index % 64u); }
            void Unset(u32 index) { _sets[index / 64u] &= ~(1ull << (index % 64u)); }
            bool Has(u32 index) const { return (_sets[index / 64u] & (1ull << (index % 64u))) != 0; }

        private:
            static constexpr u32 INLINE_SET_COUNT = 8;
            u32 _numSets;
            u64* _sets;
            u64 _inlineSets[INLINE_SET_COUNT];
        };
    }

    AutoCVar_Int CVAR_RenderGraphPrintNumBarriers(CVarCategory::Client | CVarCategory::Rendering, "renderGraphPrintNumBarriers", "Print number of barriers automatically placed by the Rendergraph", 0, CVarFlags::EditCheckbox);
    AutoCVar_Int CVAR_RenderGraphCullPasses(CVarCategory::Client | CVarCategory::Rendering, "renderGraphCullPasses", "Cull render passes without a live output consumer", 1, CVarFlags::EditCheckbox);
    AutoCVar_Int CVAR_RenderGraphPrintCullStats(CVarCategory::Client | CVarCategory::Rendering, "renderGraphPrintCullStats", "Print render graph pass-culling statistics", 0, CVarFlags::EditCheckbox);

    struct RenderGraphData : IRenderGraphData
    {
        RenderGraphData(Memory::Allocator* allocator)
            : passes(allocator, 32)
            , exportedImages(allocator, 4)
            , exportedDepthImages(allocator, 4)
            , exportedBuffers(allocator, 4)
            , signalSemaphores(allocator, 4)
            , waitSemaphores(allocator, 4)
        {

        }

        DynamicArray<IRenderPass*> passes;
        DynamicArray<ImageID> exportedImages;
        DynamicArray<DepthImageID> exportedDepthImages;
        DynamicArray<BufferID> exportedBuffers;
        BitSet* enabledPasses = nullptr;
        BitSet* livePasses = nullptr;
        u32 numEnabledPasses = 0;
        u32 numLivePasses = 0;
        f32 setupTimeMicroseconds = 0.0f;
        f32 cullTimeMicroseconds = 0.0f;

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
            NC_LOG_CRITICAL("You need to set an allocator!");
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

    void RenderGraph::Export(ImageID imageID)
    {
        static_cast<RenderGraphData*>(_data)->exportedImages.Insert(imageID);
    }

    void RenderGraph::Export(DepthImageID imageID)
    {
        static_cast<RenderGraphData*>(_data)->exportedDepthImages.Insert(imageID);
    }

    void RenderGraph::Export(BufferID bufferID)
    {
        static_cast<RenderGraphData*>(_data)->exportedBuffers.Insert(bufferID);
    }

    void RenderGraph::Setup()
    {
        ZoneScopedNC("RenderGraph::Setup", tracy::Color::Red2);
        const auto setupStart = std::chrono::steady_clock::now();

        RenderGraphData* data = static_cast<RenderGraphData*>(_data);

        size_t numPasses = data->passes.Count();
        u32 numTotalBuffers = _renderer->GetNumBuffers();
        _renderGraphBuilder = Memory::Allocator::New<RenderGraphBuilder>(_desc.allocator, _desc.allocator, _renderer, numPasses, numTotalBuffers);

        const u32 numPassSets = std::max(1u, static_cast<u32>((numPasses + 63u) / 64u));
        data->enabledPasses = Memory::Allocator::New<BitSet>(_desc.allocator, _desc.allocator, numPassSets);
        data->livePasses = Memory::Allocator::New<BitSet>(_desc.allocator, _desc.allocator, numPassSets);
        data->enabledPasses->Reset();
        data->livePasses->Reset();
        data->numEnabledPasses = 0;
        data->numLivePasses = 0;

        for (u32 i = 0; i < data->passes.Count(); i++)
        {
            IRenderPass* pass = data->passes[i];

            ZoneScopedC(tracy::Color::Red2);
            ZoneName(pass->_name, pass->_nameLength);

            _renderGraphBuilder->SetCurrentPassIndex(i);
            if (pass->Setup(_renderGraphBuilder))
            {
                data->enabledPasses->Set(i);
                ++data->numEnabledPasses;
            }
        }

        const bool cullingEnabled = CVAR_RenderGraphCullPasses.Get() != 0;
        const auto cullStart = std::chrono::steady_clock::now();
        if (!cullingEnabled)
        {
            data->livePasses->SetEquals(*data->enabledPasses);
        }
        else
        {
            ZoneScopedN("RenderGraph::ResolveLiveness");
            RenderGraphResources& resources = _renderGraphBuilder->GetResources();
            for (ImageID imageID : data->exportedImages)
                resources.GetMutableResource(imageID);
            for (DepthImageID imageID : data->exportedDepthImages)
                resources.GetMutableResource(imageID);
            for (BufferID bufferID : data->exportedBuffers)
                resources.GetMutableResource(bufferID);
            const u32 numImages = static_cast<u32>(resources.GetTrackedImages().Count());
            const u32 numDepthImages = static_cast<u32>(resources.GetTrackedDepthImages().Count());
            const u32 numBuffers = static_cast<u32>(resources.GetTrackedBuffers().Count());
            RenderGraphResourceSet neededImages(_desc.allocator, numImages);
            RenderGraphResourceSet neededDepthImages(_desc.allocator, numDepthImages);
            RenderGraphResourceSet neededBuffers(_desc.allocator, numBuffers);

            for (ImageID imageID : data->exportedImages)
                neededImages.Set(static_cast<ImageMutableResource::type>(resources.GetMutableResource(imageID)));
            for (DepthImageID imageID : data->exportedDepthImages)
                neededDepthImages.Set(static_cast<DepthImageMutableResource::type>(resources.GetMutableResource(imageID)));
            for (BufferID bufferID : data->exportedBuffers)
                neededBuffers.Set(static_cast<BufferMutableResource::type>(resources.GetMutableResource(bufferID)));

            for (i32 passIndex = static_cast<i32>(numPasses) - 1; passIndex >= 0; --passIndex)
            {
                if (!data->enabledPasses->Has(passIndex))
                    continue;

                bool live = HasRenderPassFlag(data->passes[passIndex]->_flags, RenderPassFlags::SideEffect);
                for (const TrackedImageAccess& access : resources.GetImageAccesses(passIndex))
                {
                    if (access.accessType == AccessType::WRITE && neededImages.Has(access.resourceIndex))
                        live = true;
                }
                for (const TrackedDepthImageAccess& access : resources.GetDepthImageAccesses(passIndex))
                {
                    if (access.accessType == AccessType::WRITE && neededDepthImages.Has(access.resourceIndex))
                        live = true;
                }
                for (const TrackedBufferAccess& access : resources.GetBufferAccesses(passIndex))
                {
                    if (access.accessType == AccessType::WRITE && neededBuffers.Has(access.resourceIndex))
                        live = true;
                }

                if (!live)
                    continue;

                data->livePasses->Set(passIndex);
                ++data->numLivePasses;
                for (const TrackedImageAccess& access : resources.GetImageAccesses(passIndex))
                    if (access.accessType == AccessType::WRITE) neededImages.Unset(access.resourceIndex);
                for (const TrackedDepthImageAccess& access : resources.GetDepthImageAccesses(passIndex))
                    if (access.accessType == AccessType::WRITE) neededDepthImages.Unset(access.resourceIndex);
                for (const TrackedBufferAccess& access : resources.GetBufferAccesses(passIndex))
                    if (access.accessType == AccessType::WRITE) neededBuffers.Unset(access.resourceIndex);

                for (const TrackedImageAccess& access : resources.GetImageAccesses(passIndex))
                    if (access.accessType == AccessType::READ) neededImages.Set(access.resourceIndex);
                for (const TrackedDepthImageAccess& access : resources.GetDepthImageAccesses(passIndex))
                    if (access.accessType == AccessType::READ) neededDepthImages.Set(access.resourceIndex);
                for (const TrackedBufferAccess& access : resources.GetBufferAccesses(passIndex))
                    neededBuffers.Set(access.resourceIndex);
            }
        }
        if (!cullingEnabled)
            data->numLivePasses = data->numEnabledPasses;
        data->cullTimeMicroseconds = std::chrono::duration<f32, std::micro>(std::chrono::steady_clock::now() - cullStart).count();
        _renderGraphBuilder->GetResources().SetLivePasses(data->livePasses);
        data->setupTimeMicroseconds = std::chrono::duration<f32, std::micro>(std::chrono::steady_clock::now() - setupStart).count();

        if (CVAR_RenderGraphPrintCullStats.Get() != 0)
            NC_LOG_INFO("RenderGraph passes declared={} enabled={} live={} culled={} setup_us={:.2f} cull_us={:.2f}", numPasses, data->numEnabledPasses, data->numLivePasses, data->numEnabledPasses - data->numLivePasses, data->setupTimeMicroseconds, data->cullTimeMicroseconds);
    }

    void RenderGraph::Execute()
    {
        ZoneScopedNC("RenderGraph::Execute", tracy::Color::Red2);

        RenderGraphData* data = static_cast<RenderGraphData*>(_data);
        RenderGraphResources& resources = _renderGraphBuilder->GetResources();
        
        CommandList commandList(_renderer, _desc.allocator, &resources);

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

        TimeQueryDesc totalTimeQueryDesc;
        totalTimeQueryDesc.name = "-- Total --";

        TimeQueryID totalTimeQueryID = _renderer->CreateTimeQuery(totalTimeQueryDesc);
        commandList.BeginTimeQuery(totalTimeQueryID);

        commandList.PushMarker("RenderGraph", Color::PastelBlue);
        commandList.UploadBufferBarrier();

        for (u32 i = 0; i < data->passes.Count(); i++)
        {
            if (!data->livePasses->Has(i))
                continue;
            IRenderPass* pass = data->passes[i];

            // Performance timing
            ZoneScopedC(tracy::Color::Red2);
            ZoneName(pass->_name, pass->_nameLength);

            TimeQueryDesc passTimeQueryDesc;
            passTimeQueryDesc.name = pass->_name;

            TimeQueryID passTimeQueryID = _renderer->CreateTimeQuery(passTimeQueryDesc);
            commandList.BeginTimeQuery(passTimeQueryID);

            commandList.PushMarker(pass->_name, Color::PastelGreen);
            resources.SetPassName(pass->_name);

            commandList.SetCurrentPassIndex(i);
            _renderGraphBuilder->PrePass(commandList, i, pass->_name);
            pass->Execute(resources, commandList);
            _renderGraphBuilder->PostPass(commandList, i, pass->_name);

            commandList.PopMarker();

            commandList.EndTimeQuery(passTimeQueryID);
        }
        commandList.PopMarker();
        commandList.EndTimeQuery(totalTimeQueryID);

        if (CVAR_RenderGraphPrintNumBarriers.Get())
        {
            u32 numPlacedImageBarriers = _renderGraphBuilder->GetNumPlacedImageBarriers();
            u32 numPlacedBufferBarriers = _renderGraphBuilder->GetNumPlacedBufferBarriers();
            NC_LOG_INFO("Image Barriers: {}, Buffer Barriers: {}", numPlacedImageBarriers, numPlacedBufferBarriers);
        }
        
        {
            ZoneScopedNC("CommandList::Execute", tracy::Color::Red2);
            commandList.Execute();
        }

        _renderer->EndExecutingCommandlist();
    }
}
