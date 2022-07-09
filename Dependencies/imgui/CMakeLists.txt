project(imgui VERSION 1.78)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

find_package(Vulkan REQUIRED) 

set(imgui_folder "${CMAKE_CURRENT_LIST_DIR}/imgui")

set(imgui_sources
	"${imgui_folder}/imgui.cpp"
	"${imgui_folder}/imgui_demo.cpp"
	"${imgui_folder}/imgui_draw.cpp"   
	"${imgui_folder}/imgui_widgets.cpp"
	"${imgui_folder}/imgui_tables.cpp"
	"${imgui_folder}/backends/imgui_impl_vulkan.cpp"
	"${imgui_folder}/backends/imgui_impl_glfw.cpp"
	"${imgui_folder}/misc/cpp/imgui_stdlib.cpp"
	
	"${imgui_folder}/implot.cpp"
	"${imgui_folder}/implot_demo.cpp"
	"${imgui_folder}/implot_items.cpp"

	"${imgui_folder}/imguizmo/ImGuizmo.cpp"
	"${imgui_folder}/imguizmo/GraphEditor.cpp"
	"${imgui_folder}/imguizmo/ImCurveEdit.cpp"
	"${imgui_folder}/imguizmo/ImGradient.cpp"
	"${imgui_folder}/imguizmo/ImSequencer.cpp"
	)

add_library(${PROJECT_NAME} STATIC ${imgui_sources})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${ROOT_FOLDER}/dep)
target_link_libraries(${PROJECT_NAME} 
	glfw ${GLFW_LIBRARIES}
	Vulkan::Vulkan
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "${CMAKE_CURRENT_LIST_DIR}/imgui")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_CURRENT_LIST_DIR}")