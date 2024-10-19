#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "Font.h"
#include "Renderer.h"
#include "Ubuntu-Regular.h"

#include <Base/Memory/FileReader.h>
#include <Base/Util/XXHash64.h>
#include <Base/Util/DebugHandler.h>

#include <filesystem>
#include <utfcpp/utf8.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#undef CreateFont

namespace Renderer
{
    msdfgen::FreetypeHandle* Font::_ftHandle = nullptr;
    robin_hood::unordered_map<u64, Font*> Font::_fonts;

    const Glyph& Font::GetGlyph(u32 codepoint)
    {
        auto it = _codepointToGlyphIndex.find(codepoint);
        if (it == _codepointToGlyphIndex.end())
        {
            NC_LOG_CRITICAL("The font does not support this codepoint");
        }

        u32 glyphIndex = it->second;
        return _glyphs[glyphIndex];
    }

    Font* Font::GetDefaultFont(Renderer* renderer)
    {
        // Hash the fontPath
        u64 hash = XXHash64::hash("DEFAULT", 8, 42);

        auto it = _fonts.find(hash);
        if (it == _fonts.end())
        {
            Font* font = InitFont(renderer, "DEFAULT_FONT", UBUNTU_REGULAR.data(), UBUNTU_REGULAR.size());
            font->desc.path = "DEFAULT_FONT";
            _fonts[hash] = font;
        }

        return _fonts[hash];
    }

    Font* Font::GetFont(Renderer* renderer, const std::string& fontPath)
    {
        // Hash the fontPath
        u64 hash = XXHash64::hash(fontPath.data(), fontPath.size(), 42);

        auto it = _fonts.find(hash);
        if (it == _fonts.end())
        {
            std::filesystem::path path = std::filesystem::absolute(fontPath);
            std::string fontPathStr = path.string();

            FileReader file(fontPathStr);
            if (!file.Open())
            {
                NC_LOG_WARNING("Could not open Font file {0}. Using fallback font", fontPath.c_str());

                path = std::filesystem::absolute(FALLBACK_FONT_PATH);
                file.SetPath(path.string());
                if (!file.Open())
                {
                    NC_LOG_CRITICAL("Could not open Fallback Font file {0}.", FALLBACK_FONT_PATH);
                }
            }

            std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<209715200>();
            file.Read(buffer.get(), file.Length());

            Font* font = InitFont(renderer, fontPath, buffer->GetDataPointer(), file.Length());
            font->desc.path = fontPath;
            _fonts[hash] = font;
        }

        return _fonts[hash];
    }

    Font* Font::InitFont(Renderer* renderer, std::string_view debugName, const u8* data, size_t dataSize)
    {
        Font* font = new Font();
        font->_renderer = renderer;

        // Initialize freetype if it isn't already
        if (_ftHandle == nullptr)
        {
            _ftHandle = msdfgen::initializeFreetype();
        }

        // Generate atlas
        bool success = true;

        // Load font file
        msdfgen::FontHandle* fontHandle = msdfgen::loadFontData(_ftHandle, data, static_cast<i32>(dataSize));

        if (fontHandle == nullptr)
        {
            NC_LOG_ERROR("Failed to load font");
            return nullptr;
        }

        // FontGeometry is a helper class that loads a set of glyphs from a single font.
        // It can also be used to get additional font metrics, kerning information, etc.
        msdf_atlas::FontGeometry fontGeometry(&font->_glyphs);
        fontGeometry.loadCharset(fontHandle, 1.0f, msdf_atlas::Charset::ASCII); // TODO: Loop over and load all characters

        // Apply MSDF edge coloring. See edge-coloring.h for other coloring strategies.
        const f64 maxCornerAngle = 3.0;
        for (msdf_atlas::GlyphGeometry& glyph : font->_glyphs)
        {
            glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
        }

        // TightAtlasPacker class computes the layout of the atlas.
        msdf_atlas::TightAtlasPacker packer;
        // Set atlas parameters:
        // setDimensions or setDimensionsConstraint to find the best value
        //packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
        packer.setDimensions(512, 512);

        // setScale for a fixed size or setMinimumScale to use the largest that fits
        packer.setMinimumScale(1.0);
        // setPixelRange or setUnitRange
        //packer.setPixelRange(2.0);
        packer.setUnitRange(0.5);
        packer.setMiterLimit(1.0);

        // Compute atlas layout - pack glyphs
        packer.pack(font->_glyphs.data(), static_cast<i32>(font->_glyphs.size()));

        // Get final atlas dimensions
        int width = 0, height = 0;
        packer.getDimensions(width, height);

        // The ImmediateAtlasGenerator class facilitates the generation of the atlas bitmap.
        msdf_atlas::ImmediateAtlasGenerator<
            float, // pixel type of buffer for individual glyphs depends on generator function
            4, // number of atlas color channels
            msdf_atlas::mtsdfGenerator, // function to generate bitmaps for individual glyphs
            msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 4> // class that stores the atlas bitmap
            // For example, a custom atlas storage class that stores it in VRAM can be used.
        > generator(width, height);

        // GeneratorAttributes can be modified to change the generator's default settings.
        msdf_atlas::GeneratorAttributes attributes;
        generator.setAttributes(attributes);
        generator.setThreadCount(4);

        // Generate atlas bitmap
        generator.generate(font->_glyphs.data(), static_cast<i32>(font->_glyphs.size()));

        // The atlas bitmap can now be retrieved via atlasStorage as a BitmapConstRef.
        DataTextureDesc fontTextureDesc;
        fontTextureDesc.debugName = debugName;
        fontTextureDesc.format = ImageFormat::R8G8B8A8_UNORM;
        fontTextureDesc.width = width;
        fontTextureDesc.height = height;

        msdfgen::BitmapConstRef<msdf_atlas::byte, 4> textureData = generator.atlasStorage();
        fontTextureDesc.data = const_cast<msdf_atlas::byte*>(textureData.pixels);

        font->_texture = renderer->CreateDataTexture(fontTextureDesc);

       
        for(u32 i = 0; i < font->_glyphs.size(); i++)
        {
            u32 codepoint = font->_glyphs[i].getCodepoint();
            font->_codepointToGlyphIndex[codepoint] = i;
        }

        font->metrics = fontGeometry.getMetrics();
        font->width = static_cast<f32>(width);
        font->height = static_cast<f32>(height);
        
        auto range = packer.getPixelRange();
        font->lowerPixelRange = static_cast<f32>(range.lower);
        font->upperPixelRange = static_cast<f32>(range.upper);

        return font;
    }

    TextureID Font::GetTextureID()
    {
        return _texture;
    }

    vec2 Font::CalculateTextSize(const std::string& text, f32 fontSize, f32 borderSize)
    {
        utf8::iterator it(text.begin(), text.begin(), text.end());
        utf8::iterator endIt(text.end(), text.begin(), text.end());

        vec2 size = vec2(borderSize * 2.0f, 0);
        for (; it != endIt; it++)
        {
            u32 c = *it;

            // Skip carriage return characters
            if (c == '\r')
            {
                continue;
            }

            // Handle newline characters
            if (c == '\n')
            {
                size.y += fontSize * static_cast<f32>(metrics.lineHeight);
                continue;
            }

            const Glyph& glyph = GetGlyph(c);
            if (!glyph.isWhitespace())
            {
                f64 planeLeft, planeBottom, planeRight, planeTop;

                // Get the glyph's quad bounds in plane space (vertex positions)
                glyph.getQuadPlaneBounds(planeLeft, planeBottom, planeRight, planeTop);

                // Scale the plane coordinates by the font scale
                planeBottom *= fontSize;
                planeTop *= fontSize;

                f32 height = static_cast<f32>(planeTop - planeBottom);
                height += metrics.descenderY * fontSize + borderSize;
                size.y = height;
            }

            f32 advance = static_cast<f32>(glyph.getAdvance());
            //advance = font->GetFontAdvance(advance, c, *(it+1)); // TODO: Kerning

            size.x += (fontSize * advance) + borderSize;
        }

        return size;
    }
}