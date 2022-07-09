project(Engine)

# CMake >= 3.12
cmake_minimum_required(VERSION 3.12)

# add these options before declaring the project
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo CACHE STRING "" FORCE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules")
set(CMAKE_CXX_STANDARD 17)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(ROOT_FOLDER ${PROJECT_NAME})
set(ROOT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include(CMake/Configuration.cmake)
include(CMake/VSFilterUtils.cmake)

add_subdirectory(Dependencies)
add_subdirectory(Source)