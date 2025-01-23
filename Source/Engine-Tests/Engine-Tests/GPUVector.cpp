#include <catch2/catch2.hpp>

#include <Renderer/Renderer.h>
#include <Renderer/Renderers/Vulkan/RendererVK.h>
#include <Renderer/GPUVector.h>
#include <Renderer/Window.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <filesystem>
namespace fs = std::filesystem;

class QuillReporter : public Catch::EventListenerBase
{
public:
    using Catch::EventListenerBase::EventListenerBase;

    virtual void testCaseStarting(Catch::TestCaseInfo const& testInfo) override 
    {
        NC_LOG_INFO("Starting test: {}", testInfo.name);
    }

    virtual void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override 
    {
        if (testCaseStats.totals.testCases.failed > 0) 
        {
            NC_LOG_ERROR("Test failed: {}", testCaseStats.testInfo->name);
        }
        else 
        {
            NC_LOG_INFO("Test passed: {}", testCaseStats.testInfo->name);
        }
    }

    virtual void sectionStarting(Catch::SectionInfo const& sectionInfo) override 
    {
        NC_LOG_INFO("Entering section: {}", sectionInfo.name);
    }

    virtual void sectionEnded(Catch::SectionStats const& sectionStats) override 
    {
        NC_LOG_INFO("Finished section: {}, Duration: {}ns",
            sectionStats.sectionInfo.name,
            sectionStats.durationInSeconds * 1'000'000'000);
    }

    virtual void assertionEnded(Catch::AssertionStats const& assertionStats) override 
    {
        const auto& result = assertionStats.assertionResult;
        if (!result.isOk()) 
        {
            NC_LOG_ERROR("Assertion failed in {}:{}: {}",
                result.getSourceInfo().file,
                result.getSourceInfo().line,
                result.getExpression());
        }
        else 
        {
            NC_LOG_INFO("Assertion passed: {}", result.getExpression());
        }
    }
};

//CATCH_REGISTER_LISTENER(QuillReporter) // TODO: Enable this when we have a custom main function where we can start quill backend

void VerifyU32Vector(Renderer::GPUVector<u32>& vector, u32 expectedCount, u32 expectedCapacity)
{
    bool expectedIsEmpty = expectedCount == 0;

    REQUIRE(vector.IsEmpty() == expectedIsEmpty);
    REQUIRE(vector.Count() == expectedCount);
    REQUIRE(vector.Capacity() == expectedCapacity);
    REQUIRE(vector.UsedBytes() == expectedCount * vector.ELEMENT_SIZE);
    REQUIRE(vector.TotalBytes() == expectedCapacity * vector.ELEMENT_SIZE);
    REQUIRE(vector.UsedBytes() <= vector.TotalBytes());
}

void WaitForUpload(Renderer::Renderer* renderer, Novus::Window* window, Renderer::ImageID rt)
{
    // Flip the frame and present to execute the upload
    u32 frameIndex = 0;
    f32 timeWaited = renderer->FlipFrame(frameIndex);

    Renderer::SemaphoreID uploadFinishedSemaphore = renderer->GetUploadFinishedSemaphore();
    renderer->Present(window, rt, uploadFinishedSemaphore);
}

TEST_CASE("GPU Vector", "[Renderer]")
{
    // Set up logger
    quill::Backend::start();

    auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console_sink_1");
    quill::Logger* logger = quill::Frontend::create_or_get_logger("root", std::move(console_sink), "%(time:<16) LOG_%(log_level:<11) %(message)", "%H:%M:%S.%Qms", quill::Timezone::LocalTime, quill::ClockSourceType::System);

    // Create window
    Novus::Window* window = new Novus::Window();
    window->Init(800, 600);

    // Create and init renderer
    Renderer::Renderer* renderer = new Renderer::RendererVK(window);

    fs::path absoluteShaderSourcePath = fs::absolute("Data/ShaderSrc/");
    renderer->SetShaderSourceDirectory(absoluteShaderSourcePath.string());

    ImGui::CreateContext(); // TODO: Make it optional to have ImGui enabled
    ImGui_ImplGlfw_InitForVulkan(window->GetWindow(), true);
    renderer->InitImgui();

    renderer->InitDebug();
    renderer->InitWindow(window);

    // Create rendertarget
    Renderer::ImageDesc finalColorDesc;
    finalColorDesc.debugName = "FinalColor";
    finalColorDesc.dimensions = vec2(1.0f, 1.0f);
    finalColorDesc.dimensionType = Renderer::ImageDimensionType::DIMENSION_SCALE_WINDOW;
    finalColorDesc.format = Renderer::ImageFormat::R16G16B16A16_FLOAT;
    finalColorDesc.sampleCount = Renderer::SampleCount::SAMPLE_COUNT_1;
    finalColorDesc.clearColor = Color(0.43f, 0.50f, 0.56f, 1.0f); // Slate gray

    Renderer::ImageID finalColor = renderer->CreateImage(finalColorDesc);

    // Create a GPU Vector
    Renderer::GPUVector<u32> u32Vector;
    u32Vector.SetDebugName("u32Vector");

    // Add elements to the vector and set them to 0
    for (i32 i = 0; i < 10; i++)
    {
        u32Vector.Add();
        u32Vector[i] = 0;
    }
    VerifyU32Vector(u32Vector, 10, 16);

    // Verify contents of the vector
    for (i32 i = 0; i < 10; i++)
    {
        REQUIRE(u32Vector[i] == 0);
    }

    // Clear the vector
    u32Vector.Clear();

    // Verify that the vector is empty but has capacity
    VerifyU32Vector(u32Vector, 0, 16);

    // Re-add elements to the vector, this time using values
    for (i32 i = 0; i < 10; i++)
    {
        u32Vector.Add(i);
    }
    VerifyU32Vector(u32Vector, 10, 16);

    // Grow the vector again
    for (i32 i = 0; i < 10; i++)
    {
        u32Vector.Add(10+i);
    }
    VerifyU32Vector(u32Vector, 20, 24);

    // Verify contents of the vector
    for (i32 i = 0; i < 20; i++)
    {
        REQUIRE(u32Vector[i] == i);
    }

    // Create a couple of gaps in the vector by removing some elements
    for (i32 i = 5; i < 8; i++)
    {
        u32Vector.Remove(i);
    }
    u32Vector.Remove(15, 3);
    VerifyU32Vector(u32Vector, 20, 24); // Removing does not change size or capacity until .Compress()

    // Compress the vector
    u32Vector.Compress();

    // Verify the vector is now smaller
    VerifyU32Vector(u32Vector, 14, 24);

    // Verify the contents of the vector
    {
        i32 index = 0;
        for (i32 i = 0; i < 20; i++)
        {
            if (i >= 5 && i < 8)
            {
                continue; // Skip the first gap
            }

            if (i >= 15 && i < 18)
            {
                continue; // Skip the second gap
            }

            REQUIRE(u32Vector[index++] == i);
        }
    }

    // Correct the elements in the vector
    for (i32 i = 0; i < 14; i++)
    {
        u32Vector[i] = i;
    }

    // Create a new gap in the vector
    u32Vector.Remove(5, 3);

    // Verify the vector is the same size
    VerifyU32Vector(u32Vector, 14, 24);

    // Add 3 elements which will fill gap
    for (i32 i = 0; i < 3; i++)
    {
        u32Vector.Add(i);
    }

    // Verify the vector is the same size
    VerifyU32Vector(u32Vector, 14, 24);

    // Verify the contents of the vector
    {
        for (i32 i = 0; i < 14; i++)
        {
            if (i >= 5 && i < 8)
            {
                REQUIRE(u32Vector[i] == i - 5);
            }
            else
            {
                REQUIRE(u32Vector[i] == i);
            }
        }
    }

    // Test GPU upload
    {
        REQUIRE(u32Vector.SyncToGPU(renderer)); // Require ensures that it grew
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());
    }

    // Test GPU upload with dirty element
    {
        u32Vector[5] = 100;
        u32Vector.SetDirtyElement(5);

        REQUIRE(!u32Vector.SyncToGPU(renderer)); // Require ensures that it DIDN'T grow
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());
    }
    
    // Test GPU upload with dirty elements
    {
        u32Vector[8] = 100;
        u32Vector[9] = 101;
        u32Vector[10] = 102;
        u32Vector.SetDirtyElements(8, 3);

        REQUIRE(!u32Vector.SyncToGPU(renderer)); // Require ensures that it DIDN'T grow
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());
    }

    // Test GPU upload with whole buffer dirty
    {
        for (i32 i = 0; i < 14; i++)
        {
            u32Vector[i] = 100 + i;
        }
        u32Vector.SetDirty();

        REQUIRE(!u32Vector.SyncToGPU(renderer)); // Require ensures that it DIDN'T grow
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());
    }

    // Test GPU upload with hole
    {
        u32Vector.Remove(5, 3);

        REQUIRE(!u32Vector.SyncToGPU(renderer)); // Require ensures that it DIDN'T grow
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());

        // Now fill the hole
        u32Vector.Add(200);
        u32Vector.Add(201);
        u32Vector.Add(202);

        REQUIRE(!u32Vector.SyncToGPU(renderer)); // Require ensures that it DIDN'T grow
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());
    }

    // Test GPU upload with compressed hole
    {
        u32Vector.Remove(5, 3);
        u32Vector.Compress();

        REQUIRE(!u32Vector.SyncToGPU(renderer)); // Require ensures that it DIDN'T grow
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());

        // Now add to the end
        u32Vector.Add(300);
        REQUIRE(!u32Vector.SyncToGPU(renderer)); // Require ensures that it DIDN'T grow
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());
    }

    // Grow two frames in a row
    {
        for (u32 i = 0; i < 13; i++)
        {
            u32Vector.Add(400 + i);
        }

        REQUIRE(u32Vector.SyncToGPU(renderer)); // Require ensures that it grew
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());

        for (u32 i = 0; i < 8; i++)
        {
            u32Vector.Add(500 + i);
        }

        REQUIRE(u32Vector.SyncToGPU(renderer)); // Require ensures that it grew
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());
    }

    // Test GPU upload with several dirty frames
    {
        for (i32 i = 0; i < 3; i++)
        {
            u32Vector[i] = 600 + i;
        }
        u32Vector.SetDirtyElements(0, 3);

        for (i32 i = 7; i < 10; i++)
        {
            u32Vector[i] = 607 + i;
        }
        u32Vector.SetDirtyElements(7, 3);

        for (i32 i = 30; i < 33; i++)
        {
            u32Vector[i] = 630 + i;
        }
        u32Vector.SetDirtyElements(30, 3);

        REQUIRE(!u32Vector.SyncToGPU(renderer)); // Require ensures that it DIDN'T grow
        WaitForUpload(renderer, window, finalColor);
        REQUIRE(u32Vector.Validate());

        // Wait one last frame
        WaitForUpload(renderer, window, finalColor);
    }
}