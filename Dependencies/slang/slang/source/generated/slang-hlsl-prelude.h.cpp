// generated code; do not edit
#include "../source/core/slang-basic.h"
Slang::String get_slang_hlsl_prelude()
{
Slang::StringBuilder sb;
sb << 
"#ifdef SLANG_HLSL_ENABLE_NVAPI\n"
"#include \"nvHLSLExtns.h\"\n"
"#endif\n"
"\n"
"#ifndef __DXC_VERSION_MAJOR\n"
"// warning X3557: loop doesn't seem to do anything, forcing loop to unroll\n"
"#pragma warning(disable : 3557)\n"
"#endif\n"
"\n"
;
return sb.produceString();
}
