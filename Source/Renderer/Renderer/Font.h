#pragma once
#include "Descriptors/FontDesc.h"
#include "Descriptors/TextureDesc.h"

#include <Base/Types.h>

#include <robinhood/robinhood.h>
#include <msdf-atlas-gen/GlyphGeometry.h>
#include <msdf-atlas-gen/FontGeometry.h>

struct stbtt_fontinfo;

namespace msdfgen
{
    class FreetypeHandle;
}

namespace Renderer
{
    using Glyph = msdf_atlas::GlyphGeometry;
    using FontMetrics = msdfgen::FontMetrics;

    class Renderer;

    struct Font
    {
        static constexpr char* FALLBACK_FONT_PATH = (char*)"Data/Fonts/Ubuntu-Regular.ttf";

        FontDesc desc;
        FontMetrics metrics;
        
        f32 width;
        f32 height;
        f32 lowerPixelRange;
        f32 upperPixelRange;

        bool IsValidGlyph(u32 codepoint);
        const Glyph& GetGlyph(u32 codepoint);
        TextureID GetTextureID();

        f32 CalculateCharWidth(const char c, f32 fontSize, f32 borderSize);
        f32 CalculateCharHeight(const char c, f32 fontSize, f32 borderSize);
        vec2 CalculateCharSize(const char c, f32 fontSize, f32 borderSize);
        vec2 CalculateTextSize(const std::string& text, f32 fontSize, f32 borderSize);

        static Font* GetDefaultFont(Renderer* renderer);
        static Font* GetFont(Renderer* renderer, const std::string& fontPath);
        
    private:
        Font() = default;

        static Font* InitFont(Renderer* renderer, std::string_view debugName, const u8* data, size_t dataSize);

    private:
        static msdfgen::FreetypeHandle* _ftHandle;
        static robin_hood::unordered_map<u64, Font*> _fonts;

        std::vector<Glyph> _glyphs;
        robin_hood::unordered_map<u32, u32> _codepointToGlyphIndex;

        TextureID _texture = TextureID::Invalid();

        Renderer* _renderer;
        

        friend class Renderer;
    };
}