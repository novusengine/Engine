project(engine-dependencies VERSION 1.0.0)

## Json
add_library(json INTERFACE)
add_library(json::json ALIAS json)

target_include_directories(json INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/json"
)

## Robin hood hashing
add_library(robin-hood-hashing INTERFACE)
add_library(robin-hood-hashing::robin-hood-hashing ALIAS robin-hood-hashing)

target_include_directories(robin-hood-hashing INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/robinhood"
)

## GLM math library
add_library(glm INTERFACE)
add_library(glm::glm ALIAS glm)

target_include_directories(glm INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/glm"
)

add_subdirectory(glfw)
add_subdirectory(gli)
add_subdirectory(imgui)
add_subdirectory(typesafe)

if(NOT TARGET tracy)
    add_subdirectory(TracyProfiler)
endif()

if(NOT TARGET shadercooker)
    add_subdirectory(dxcompiler)
endif()

## engine-dependencies

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_library(engine::dependencies ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE
    json::json
    robin-hood-hashing::robin-hood-hashing
    glm::glm

    type_safe
)
