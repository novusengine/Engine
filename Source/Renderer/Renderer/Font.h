#pragma once
#include "Descriptors/FontDesc.h"
#include "Descriptors/TextureArrayDesc.h"

#include <Base/Types.h>

#include <robinhood/robinhood.h>

struct stbtt_fontinfo;

namespace Renderer
{
    class Renderer;

    struct FontChar
    {
        f32 advance;
        i32 xOffset;
        i32 yOffset;
        i32 width = 0;
        i32 height = 0;
        u8* data;

        u32 textureIndex;
    };

    struct Font
    {
        static constexpr char* FALLBACK_FONT_PATH = (char*)"Data/fonts/Ubuntu/Ubuntu-Regular.ttf";

        FontDesc desc;

        stbtt_fontinfo* fontInfo;
        float scale;

        FontChar& GetChar(u32 character);
        TextureArrayID GetTextureArray();

        static Font* GetDefaultFont(Renderer* renderer, f32 fontSize);
        static Font* GetFont(Renderer* renderer, const std::string& fontPath, f32 fontSize);
        
    private:
        Font() = default;

        bool InitChar(u32 character, FontChar& fontChar);

        static Font* CreateFont(Renderer* renderer, const u8* data, f32 fontSize);

    private:
        static robin_hood::unordered_map<u64, Font*> _fonts;
        robin_hood::unordered_map<u32, FontChar> _chars;

        TextureArrayID _textureArray = TextureArrayID::Invalid();

        Renderer* _renderer;

        friend class Renderer;
    };
}