if(NOT TARGET base)
    add_subdirectory(Base)
endif()

if(NOT TARGET fileformat)
    add_subdirectory(FileFormat)
endif()

if(NOT TARGET network)
    add_subdirectory(Network)
endif()

if(NOT TARGET renderer)
    add_subdirectory(Renderer)
endif()

if(NOT TARGET scripting)
    add_subdirectory(Scripting)
endif()

if(NOT TARGET shadercooker)
    add_subdirectory(Shadercooker)
endif()