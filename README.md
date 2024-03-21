## What is Novus
Novus is an MMO Engine.

Currently Novus is developing a game/server solution where our main focus is to be feature complete with the most popular MMOs out there.
Long term, we hope to support many more features.

## The Novus Promise
The project was made with the promise that we would always focus on reliability, redundancy, and performance over convenience. We achieve this through making use of experience, but also applying modern techniques and design patterns.

The end-goal is to provide a game/server setup, capable of tackling all of the limitations set by the current standard. Solving those issues are complicated, but we start by applying a proper foundation for our architecture to allow for better flow of information(data) and performance.

## Novus Discord
The project has an official [Discord](https://discord.gg/gz6FMZa).
You will find the developers to be active on the discord and always up for answering any questions you might have regarding the project. Despite Novus not currently being ready for production level use, we are always welcoming any users that want to try using it.

## Dependencies
* [OpenSSL 1.1.0](https://www.openssl.org/source/)
* [Premake5](https://premake.github.io/)
* [Vulkan 1.1 (or higher)](https://vulkan.lunarg.com/)

## Libraries
Here we include a honorable mention to all the libraries included directly into the source of Novus. You do not need to download these on your own, but we felt it was important to address these as without them, Novus would be a lot more time consuming to develop.
* [Moodycamel ConcurrentQueue](https://github.com/cameron314/concurrentqueue)
* [Nlohmann Json](https://github.com/nlohmann/json)
* [Robin Hood Hashing](https://github.com/martinus/robin-hood-hashing)
* [Tracy Profiler](https://github.com/wolfpld/tracy)
* [Entt ECS](https://github.com/skypjack/entt/)
* [EnkiTS](https://github.com/dougbinks/enkiTS)
* [ImGui](https://github.com/ocornut/imgui)
* [Libpqxx](https://github.com/jtv/libpqxx)
* [catch2](https://github.com/catchorg/Catch2)
* [glfw](https://www.glfw.org/)
* [glm](https://github.com/g-truc/glm)
* [gli](https://github.com/g-truc/gli)
* [luau](https://github.com/luau-lang/luau)
* [spdlog](https://github.com/gabime/spdlog)
* [typesafe](https://github.com/foonathan/type_safe)
* [refl-cpp](https://github.com/veselink1/refl-cpp)
* [dxcompiler](https://github.com/microsoft/DirectXShaderCompiler)
* [Base64](https://github.com/tobiaslocker/base64)

## How to build (Microsoft Visual 2022)
>[!TIP]
> If you plan to use [symlink](https://schinagl.priv.at/nt/hardlinkshellext/linkshellextension.html) to link engine to the [AssetConverter](https://github.com/novusengine/AssetConverter) and [Game](https://github.com/novusengine/Game) projects, you do not need to run premake or build the solution from this folder.
1. Download the dependencies.
2. Fork and clone the repositry
3. Open the project folder and open a terminal within the project's directory.
4. Use the command `premake5 vs2022`
> [!NOTE]
> You can change the files generated by using a different [action](https://premake.github.io/docs/using-premake) than `vs2022`.   
5. Open the new `Build` folder and double click the `Engine.sln` file.
6. Use `ctrl`+`shift`+`b` to build the solution or right click the solution and select `Build Solution`
7. The solution builds to `../Build/Bin/Engine/[BuildConfig]`

## How to use
This project is a dependency of the [AssetConverter](https://github.com/novusengine/AssetConverter) and [Game](https://github.com/novusengine/Game) projects. Both projects already include engine, but it's suggested to clone this project anyway and symlink the engine into the `Submodule` folder of the projects.
