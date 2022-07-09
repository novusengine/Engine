project(dxcompiler VERSION 3.2.2)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include/")
target_link_libraries(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/lib/windows/dxcompiler.lib")

set(DXCOMPILER_DYNAMIC_LIB_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/windows/dxcompiler.dll" CACHE STRING "")