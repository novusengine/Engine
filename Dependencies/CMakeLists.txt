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

## Refl-cpp
add_library(refl-cpp INTERFACE)
add_library(refl-cpp::refl-cpp ALIAS refl-cpp)

target_include_directories(refl-cpp INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/refl-cpp"
)

## entt
add_library(entt INTERFACE)
add_library(entt::entt ALIAS entt)

target_include_directories(entt INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/entt"
)

## GLM math library
add_library(glm INTERFACE)
add_library(glm::glm ALIAS glm)

target_include_directories(glm INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/glm"
)
target_compile_definitions(glm INTERFACE GLM_FORCE_LEFT_HANDED GLM_FORCE_DEPTH_ZERO_TO_ONE)

## UTFCPP
add_library(utfcpp INTERFACE)
add_library(utfcpp::utfcpp ALIAS utfcpp)

target_include_directories(utfcpp INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/utfcpp"
)

add_subdirectory(glfw)
add_subdirectory(gli)
add_subdirectory(imgui)
add_subdirectory(typesafe)
add_subdirectory(spdlog)

if(NOT TARGET tracy)
    add_subdirectory(TracyProfiler)
endif()

if(NOT TARGET shadercooker)
    add_subdirectory(dxcompiler)
endif()

## enkiTS
if(NOT TARGET enkiTS)
    set(enkiTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/enkiTS)
    set(enkiTS_INCLUDE_DEST ${CMAKE_CURRENT_BINARY_DIR}/include/enkiTS)

    set(ENKITS_BUILD_EXAMPLES OFF CACHE BOOL "Disable enkiTS examples" FORCE)
    set(ENKITS_BUILD_C_INTERFACE OFF CACHE BOOL "Disable enkiTS C Interface" FORCE)
    add_subdirectory(${enkiTS_ROOT})

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${enkiTS_INCLUDE_DEST}
        COMMAND ${CMAKE_COMMAND} -E copy ${enkiTS_ROOT}/src/TaskScheduler.h ${enkiTS_INCLUDE_DEST}/TaskScheduler.h
    )

    add_library(enkiTS_proxy INTERFACE)
    target_link_libraries(enkiTS_proxy INTERFACE enkiTS)
    target_include_directories(enkiTS_proxy INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)

    add_library(enkiTS::enkiTS ALIAS enkiTS_proxy)
endif()

## engine-dependencies

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_library(engine::dependencies ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE
    json::json
    robin-hood-hashing::robin-hood-hashing
    glm::glm
    entt::entt
    type_safe
    spdlog
    refl-cpp::refl-cpp
)
