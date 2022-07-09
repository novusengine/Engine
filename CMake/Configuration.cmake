include_guard(GLOBAL)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if (NOT MSVC)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    endif()
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_compile_definitions(NC_$<CONFIG> NOMINMAX WIN32_LEAN_AND_MEAN)

if (MSVC)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang") #MSVC
        add_compile_options("/WX")
    else() #GCC/Clang on Windows
        add_compile_options("-Werror")
    endif()
endif()