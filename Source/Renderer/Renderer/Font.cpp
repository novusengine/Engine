#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "Font.h"
#include "Renderer.h"
#include "Ubuntu-Regular.h"

#include <Base/Memory/FileReader.h>
#include <Base/Util/XXHash64.h>
#include <Base/Util/DebugHandler.h>

#include <filesystem>

namespace Renderer
{
    robin_hood::unordered_map<u64, Font*> Font::_fonts;

    FontChar& Font::GetChar(u32 character)
    {
        auto it = _chars.find(character);
        if (it == _chars.end())
        {
            FontChar fontChar;
            if (!InitChar(character, fontChar))
            {
                DebugHandler::PrintFatal("The font does not support this character");
            }

            _chars[character] = fontChar;
        }

        return _chars[character];
    }

    Font* Font::GetDefaultFont(Renderer* renderer, f32 fontSize)
    {
        // Hash and add together the fontPath and fontSize
        u64 hash = XXHash64::hash("DEFAULT", 8, 42) + XXHash64::hash(&fontSize, sizeof(f32), 42);

        auto it = _fonts.find(hash);
        if (it == _fonts.end())
        {
            Font* font = CreateFont(renderer, UBUNTU_REGULAR.data(), fontSize);
            _fonts[hash] = font;
        }

        return _fonts[hash];
    }

    Font* Font::GetFont(Renderer* renderer, const std::string& fontPath, f32 fontSize)
    {
        // Hash and add together the fontPath and fontSize
        u64 hash = XXHash64::hash(fontPath.data(), fontPath.size(), 42) + XXHash64::hash(&fontSize, sizeof(f32), 42);

        auto it = _fonts.find(hash);
        if (it == _fonts.end())
        {
            std::filesystem::path path = std::filesystem::absolute(fontPath);
            std::string fontPathStr = path.string();

            FileReader file(fontPathStr);
            if (!file.Open())
            {
                DebugHandler::PrintWarning("Could not open Font file %s. Using fallback font", fontPath.c_str());

                path = std::filesystem::absolute(FALLBACK_FONT_PATH);
                file.SetPath(path.string());
                if (!file.Open())
                {
                    DebugHandler::PrintFatal("Could not open Fallback Font file %s.", FALLBACK_FONT_PATH);
                }
            }

            std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<209715200>();
            file.Read(buffer.get(), file.Length());

            Font* font = CreateFont(renderer, buffer->GetDataPointer(), fontSize);
            _fonts[hash] = font;
        }

        return _fonts[hash];
    }

    bool Font::InitChar(u32 character, FontChar& fontChar)
    {
        fontChar.data = stbtt_GetCodepointSDF(fontInfo, scale, character, desc.padding, 128, 64.0f, &fontChar.width, &fontChar.height, &fontChar.xOffset, &fontChar.yOffset);

        if (fontChar.width == 0 && fontChar.height == 0)
            return false;

        int advance;
        stbtt_GetCodepointHMetrics(fontInfo, character, &advance, NULL);
        fontChar.advance = advance * scale;

        DataTextureDesc textureDesc;
        textureDesc.width = fontChar.width;
        textureDesc.height = fontChar.height;
        textureDesc.format = ImageFormat::R8_UNORM;
        textureDesc.data = fontChar.data;
        textureDesc.debugName = desc.path + " " + std::to_string(character);

        _renderer->CreateDataTextureIntoArray(textureDesc, _textureArray, fontChar.textureIndex);

        delete[] textureDesc.data;

        return true;
    }

    Font* Font::CreateFont(Renderer* renderer, const u8* data, f32 fontSize)
    {
        Font* font = new Font();
        font->_renderer = renderer;

        font->fontInfo = new stbtt_fontinfo();
        stbtt_InitFont(font->fontInfo, data, 0);

        font->scale = stbtt_ScaleForPixelHeight(font->fontInfo, fontSize);

        // Create texture array
        TextureArrayDesc desc;
        desc.size = 4096;

        font->_textureArray = renderer->CreateTextureArray(desc);

        // Preload char 32 to 127 (commonly used ASCII characters)
        for (u32 i = 32; i < 127; i++)
        {
            FontChar fontChar;
            if (font->InitChar(i, fontChar))
            {
                font->_chars[i] = fontChar;
            }
        }

        return font;
    }

    TextureArrayID Font::GetTextureArray()
    {
        return _textureArray;
    }
}