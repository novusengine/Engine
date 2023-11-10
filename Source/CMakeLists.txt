if(NOT TARGET base)
    add_subdirectory(Base)
endif()

if(NOT TARGET fileformat)
    add_subdirectory(FileFormat)
endif()

if(NOT TARGET input)
    add_subdirectory(Input)
endif()

if(NOT TARGET network)
    add_subdirectory(Network)
endif()

if(NOT TARGET renderer)
    add_subdirectory(Renderer)
endif()

if(NOT TARGET scripting)
    # Set Output Directories
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/Luau/Libs)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/Luau)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/Luau)

    add_subdirectory(Scripting)

    # Restore Output Directories
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/Libs)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
endif()

if(NOT TARGET shadercooker)
    add_subdirectory(ShaderCooker)
endif()
