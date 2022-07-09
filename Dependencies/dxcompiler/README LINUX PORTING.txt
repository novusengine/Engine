Hi, if you are reading this you are probably thinking about porting NovusCore to Linux!

The NovusCore Shader Compiler is a commandline tool which reads shaders, compiles them and additionally outputs a file used by the engine to bind parameters to the shader.
While NovusCore uses Vulkan as the rendering backend, we're not very happy with the state of GLSL compared to HLSL. 
This together with the team being more familiar with HLSL led us to take the decision to use HLSL which compiles into SPIR-V for Vulkan.

The Shader Compiler uses the DirectX Shader Compiler (https://github.com/microsoft/DirectXShaderCompiler) as a library to compile HLSL into SPIR-V.
Because DXSC uses LLVM as a backend it's pretty complicated and time demanding to build it from sources, so we chose the library route.

The library binaries used are taken from DXSCs Appveyor, which can be found here. They also provide Linux versions: https://ci.appveyor.com/project/antiagainst/directxshadercompiler/

On Windows we use these files:
dxcompiler.lib - The compiler dll link lib
dxcompiler.dll - The compilers frontend
dxcapi.h - The header with all the required interfaces

To port this to Linux you'll need to download the linux version from the appveyor and make sure the right files are being linked against in cmake.

You'll also need to replace the way we load the DLL in ShaderCooker.cpp, I recommend taking a look at these github issues to see if they have a clear example (they don't at the time of writing):
https://github.com/microsoft/DirectXShaderCompiler/issues/1342
https://github.com/microsoft/DirectXShaderCompiler/issues/2680

Good luck, feel free to ask Pursche in the Discord if you have any questions but keep in mind that my Linux experience is extremely limited.