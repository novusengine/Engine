include_guard(GLOBAL)

if (MSVC)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang") #MSVC
        #add_compile_options("/WX")
    else() #GCC/Clang on Windows
        add_compile_options("-Werror")
    endif()
endif()