/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

#include <Axis/RendererPch.hpp>

#include <Axis/Buffer.hpp>
#include <Axis/Config.hpp>
#include <Axis/DeviceContext.hpp>
#include <Axis/Exception.hpp>
#include <Axis/GraphicsCommon.hpp>
#include <Axis/GraphicsDevice.hpp>
#include <Axis/Math.hpp>
#include <Axis/Memory.hpp>
#include <Axis/SmartPointer.hpp>
#include <Axis/SpriteFont.hpp>
#include <Axis/Texture.hpp>
#include <Axis/Utility.hpp>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_GLYPH_H

namespace Axis
{

/// Freetype library global configuration
Bool       s_FreetypeInitialized = false;
std::mutex s_FreetypeMutex       = {};
FT_Library s_FreetypeLibrary     = {};

SpriteFont::SpriteFont(FileStream&                   fileStream,
                       Uint32                        fontSize,
                       const FontAtlasConfiguration& atlasConfiguration) :
    _fontSize(fontSize),
    _atlasConfig(atlasConfiguration)
{
    // Checks the arguments
    {
        if (!fileStream.IsOpen())
            throw IOException("FileStream was not open!");

        if (!(Bool)(fileStream.GetFileModes() & FileMode::Read))
            throw InvalidArgumentException("FileStream was not opened for writing!");

        if (!(Bool)(fileStream.GetFileModes() & FileMode::Binary))
            throw InvalidArgumentException("FileStream was not opened in binary mode!");
    }

    // Copies file data
    {
        // Copies all the data from the file stream to the buffer
        _fontByte = UniquePointer<Byte[]>(Axis::NewArray<Byte>(fileStream.GetLength()));

        // Stores the font byte size
        _fontByteSize = fileStream.GetLength();

        // Reads the file stream into the buffer
        fileStream.Read(_fontByte.GetPointer(), 0, fileStream.GetLength());
    }

    Initialize();
}

SpriteFont::SpriteFont(CPVoid                        fontData,
                       Size                          fontDataSize,
                       Uint32                        fontSize,
                       const FontAtlasConfiguration& atlasConfiguration) :
    _atlasConfig(atlasConfiguration),
    _fontSize(fontSize)
{
    if (!fontData)
        throw InvalidArgumentException("fontData was nullptr!");

    if (!fontSize)
        throw InvalidArgumentException("fontDataSize was zero!");

    // Copies file data
    {
        // Creates a buffer to store the font data
        _fontByte = UniquePointer<Byte[]>(Axis::NewArray<Byte>(fontDataSize));

        // Copies the font data
        std::memcpy(_fontByte.GetPointer(), fontData, fontDataSize);
    }
}

SpriteFont::~SpriteFont() noexcept
{
    if (_fontFace)
        FT_Done_Face((FT_Face)_fontFace);
}

Vector2F SpriteFont::MeasureString(const StringView<WChar>& string) const noexcept
{
    if (string.GetLength() == 0)
        return {0.0f, 0.0f};

    Float32  width            = 0.0f;
    Float32  finalLineHeight  = (Float32)GetLineHeight();
    Vector2F offset           = {0.0f, 0.0f};
    Bool     firstGlyphOfLine = true;

    auto glyphEnd = _charGlyphs.end();
    auto rectEnd  = _charRects.end();

    for (const auto& character : string)
    {
        if (character == L'\r')
            continue;

        if (character == L'\n')
        {
            finalLineHeight = (Float32)GetLineHeight();

            offset.X = 0;
            offset.Y += (Float32)GetLineHeight();
            firstGlyphOfLine = true;
            continue;
        }

        auto glyph     = _charGlyphs.Find(character);
        auto rectangle = _charRects.Find(character);

        if (glyph == glyphEnd || rectangle == rectEnd)
            continue;

        if (firstGlyphOfLine)
        {
            offset.X         = (Float32)Math::Max(glyph->Second.Bearing.X, 0.0f);
            firstGlyphOfLine = false;
        }
        else
        {
            offset.X += glyph->Second.Bearing.X;
        }

        offset.X += glyph->Second.Advance.X;

        auto calculatedWidth = offset.X + rectangle->Second.Width;
        width                = Math::Max(width, calculatedWidth);
    }

    return {width, offset.Y + finalLineHeight};
}

void SpriteFont::Initialize()
{

    // Initializes feetype library for the first time
    {
        std::lock_guard<std::mutex> lock(s_FreetypeMutex);

        if (!s_FreetypeInitialized)
        {
            if (FT_Init_FreeType(&s_FreetypeLibrary))
                throw ExternalException("Freetype library initialization failed");

            s_FreetypeInitialized = true;
        }
    }

    // Checks the argument
    {
        if (_fontSize == 0)
            throw InvalidArgumentException("Size was zero!");

        if (!_atlasConfig.GraphicsDevice)
            throw InvalidArgumentException("GraphicsDevice was null!");

        if (!_atlasConfig.ImmediateDeviceContext)
            throw InvalidArgumentException("ImmediateDeviceContext was null!");

        if (!Math::ReadBitPosition(_atlasConfig.DeviceQueueFamilyMask, _atlasConfig.ImmediateDeviceContext->DeviceQueueFamilyIndex))
            throw InvalidArgumentException("DeviceQueueFamilyMask did not support the ImmediateDeviceContext's DeviceQueueFamilyIndex!");

        if (_atlasConfig.UseCharacterRange && _atlasConfig.StartCharacterRange > _atlasConfig.EndCharacterRange)
            throw InvalidArgumentException("StartCharacterRange was greater than EndCharacterRange!");
    }

    // Sets font size
    {
        // Creates the font face
        if (FT_New_Memory_Face(s_FreetypeLibrary, _fontByte.GetPointer(), _fontByteSize, 0, (FT_Face*)&_fontFace))
            throw ExternalException("Freetype library failed to create a new font face");

        // Sets the font size
        if (FT_Set_Pixel_Sizes((FT_Face)_fontFace, 0, _fontSize))
            throw ExternalException("Freetype library failed to set the font size");

        // Sets the font's default line spacing
        _lineHeight = ((FT_Face)_fontFace)->size->metrics.height / 64.f;
    }

    Size bufferSize = {};

    /// Calculates the glyph information of each character
    ///
    /// Iterates through each character in the font's character range
    /// and calculates the glyph information of each character.
    ///
    /// accumulates the size of the font's texture atlas
    {
        auto AppendCharacter = [&](WChar character) {
            // Gets the glyph
            if (FT_Load_Char((FT_Face)_fontFace, character, FT_LOAD_DEFAULT))
                return;

            auto& bbox = ((FT_Face)_fontFace)->bbox;

            auto width  = bbox.xMax - bbox.xMin;
            auto height = bbox.yMax - bbox.yMin;
            auto xOff   = (((FT_Face)_fontFace)->glyph->advance.x - width) / 2;
            auto yOff   = bbox.yMax - ((FT_Face)_fontFace)->glyph->metrics.horiBearingY / 64.f;

            // Gets the glyph's metrics
            Glyph glyph = {
                .Bearing         = {xOff, yOff},
                .Advance         = {((FT_Face)_fontFace)->glyph->advance.x / 64.f, ((FT_Face)_fontFace)->glyph->advance.y / 64.f},
                .SourceRectangle = {0.0f, 0.0f, width, height}};
        };

        // Checks if the user wants to use all of available glyphs or not
        if (_atlasConfig.UseCharacterRange)
        {
            // Gets the first and last character of the range
            const Uint32 firstCharacter = _atlasConfig.StartCharacterRange;
            const Uint32 lastCharacter  = _atlasConfig.EndCharacterRange;

            // Calculates the number of characters in the range
            const Uint32 numberOfCharacters = lastCharacter - firstCharacter + 1;

            // Allocates the character information
            _charGlyphs.Reserve(numberOfCharacters);

            // Iterates through the characters
            for (Uint32 character = firstCharacter; character <= lastCharacter; ++character)
            {
                // Appends the character
                AppendCharacter(character);
            }
        }
        else
        {
            // Gets the number of characters in the font
            const Uint32 numberOfCharacters = ((FT_Face)_fontFace)->num_glyphs;

            // Allocates the character information
            _charGlyphs.Reserve(numberOfCharacters);

            // Gets the first glyph index
        }
    }
}

} // namespace Axis