project(renderer VERSION 1.0.0 DESCRIPTION "Renderer is a static library for Engine")

find_package(Vulkan REQUIRED)
file(GLOB_RECURSE RENDERER_FILES "*.cpp" "*.h")

add_library(${PROJECT_NAME} ${RENDERER_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set(SHADER_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/../Data/shaders/ CACHE PATH "Output Directory for shaders")
target_compile_definitions(${PROJECT_NAME} PUBLIC SHADER_SOURCE_DIR="${shaders_SOURCE_DIR}" NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS _CRT_SECURE_NO_WARNINGS VK_USE_PLATFORM_WIN32_KHR)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC
    base::base
    shadercooker::shadercooker
    tracy::tracy

    Vulkan::Vulkan
    gli::gli
    imgui::imgui
    
    glfw ${GLFW_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${ROOT_FOLDER} LINKER_LANGUAGE CXX)
create_vs_filters(${RENDERER_FILES})