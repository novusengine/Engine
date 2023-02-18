project(tracy VERSION 1.0.0)

set(ENABLE_TRACY_PROFILER false CACHE BOOL "Enables Tracy profiling")

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set(TRACY_SOURCE
    "${CMAKE_CURRENT_LIST_DIR}/tracy/TracyClient.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}"
)

source_group(TracyProfiler ${TRACY_SOURCE})

target_sources(${PROJECT_NAME} PRIVATE
    ${TRACY_SOURCE}
)

if (ENABLE_TRACY_PROFILER)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        TRACY_ENABLE
    )
endif()