/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/RendererPch.hpp>

#include <Axis/Buffer.hpp>
#include <Axis/Config.hpp>
#include <Axis/DeviceContext.hpp>
#include <Axis/Exception.hpp>
#include <Axis/GraphicsCommon.hpp>
#include <Axis/GraphicsDevice.hpp>
#include <Axis/Math.hpp>
#include <Axis/Memory.hpp>
#include <Axis/PackSprite.hpp>
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

namespace Renderer
{

/// Freetype library global configuration
static Bool       s_FreetypeInitialized = false;
static std::mutex s_FreetypeMutex       = {};
static FT_Library s_FreetypeLibrary     = {};

SpriteFont::SpriteFont(System::FileStream&           fileStream,
                       Uint32                        fontSize,
                       const FontAtlasConfiguration& atlasConfiguration) :
    _fontSize(fontSize),
    _atlasConfig(atlasConfiguration)
{
    // Checks the arguments
    {
        if (!fileStream.IsOpen())
            throw System::IOException("FileStream was not open!");

        if (!(Bool)(fileStream.GetFileModes() & System::FileMode::Read))
            throw System::InvalidArgumentException("FileStream was not opened for writing!");

        if (!(Bool)(fileStream.GetFileModes() & System::FileMode::Binary))
            throw System::InvalidArgumentException("FileStream was not opened in binary mode!");
    }

    // Copies file data
    {
        // Copies all the data from the file stream to the buffer
        _fontByte = System::UniquePointer<Byte[]>(Axis::System::NewArray<Byte>(fileStream.GetLength()));

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
    _fontSize(fontSize),
    _atlasConfig(atlasConfiguration)
{
    if (!fontData)
        throw System::InvalidArgumentException("fontData was nullptr!");

    if (!fontSize)
        throw System::InvalidArgumentException("fontDataSize was zero!");

    // Copies file data
    {
        // Creates a buffer to store the font data
        _fontByte = System::UniquePointer<Byte[]>(Axis::System::NewArray<Byte>(fontDataSize));

        // Copies the font data
        std::memcpy(_fontByte.GetPointer(), fontData, fontDataSize);
    }

    Initialize();
}

SpriteFont::FontFaceRAII::~FontFaceRAII() noexcept
{
    if (_fontFace)
        FT_Done_Face(_fontFace);
}


SpriteFont::FontFaceRAII::FontFaceRAII(FontFaceRAII&& other) noexcept :
    _fontFace(other._fontFace)
{
    other._fontFace = nullptr;
}

SpriteFont::FontFaceRAII& SpriteFont::FontFaceRAII::operator=(FontFaceRAII&& other) noexcept
{
    if (this != &other)
    {
        if (_fontFace)
            FT_Done_Face(_fontFace);

        _fontFace       = other._fontFace;
        other._fontFace = nullptr;
    }

    return *this;
}

System::Vector2UI SpriteFont::MeasureString(const System::StringView<WChar>& string) const noexcept
{
    if (string.GetLength() == 0)
        return {0.0f, 0.0f};

    Bool firstGlyphOfLine = true;

    Uint32 furthestY = 0;
    Uint32 furthestX = 0;

    System::Vector2UI penPosition = {0, 0};

    auto glyphEnd = _charGlyphs.end();
    auto rectEnd  = _charRects.end();

    for (const auto& character : string)
    {
        if (character == L'\r')
            continue;

        if (character == L'\n')
        {
            penPosition.X = 0;
            penPosition.Y += (Uint32)GetLineHeight();

            firstGlyphOfLine = true;

            continue;
        }

        auto glyphIt = _charGlyphs.Find(character);
        auto rectIt  = _charRects.Find(character);

        if (glyphIt == glyphEnd)
            continue;

        auto drawingPos = penPosition;

        if (!firstGlyphOfLine && glyphIt->Second.Bearing.X > 0)
            drawingPos.X += glyphIt->Second.Bearing.X;

        drawingPos.Y += (Uint32)GetLineHeight() - glyphIt->Second.Bearing.Y;

        penPosition.X += glyphIt->Second.Advance.X;

        furthestX = System::Math::Max(drawingPos.X + glyphIt->Second.Advance.X, furthestX);
        furthestY = System::Math::Max(drawingPos.Y + (rectIt == rectEnd ? 0 : rectIt->Second.Height), furthestY);

        firstGlyphOfLine = true;
    }

    return {furthestX, furthestY};
}

void SpriteFont::Initialize()
{

    // Initializes feetype library for the first time
    {
        std::lock_guard<std::mutex> lock(s_FreetypeMutex);

        if (!s_FreetypeInitialized)
        {
            if (FT_Init_FreeType(&s_FreetypeLibrary))
                throw System::ExternalException("Freetype library initialization failed");

            s_FreetypeInitialized = true;
        }
    }

    // Checks the argument
    {
        if (_fontSize == 0)
            throw System::InvalidArgumentException("Size was zero!");

        if (!_atlasConfig.GraphicsDevice)
            throw System::InvalidArgumentException("GraphicsDevice was null!");

        if (!_atlasConfig.ImmediateDeviceContext)
            throw System::InvalidArgumentException("ImmediateDeviceContext was null!");

        if (!System::Math::ReadBitPosition(_atlasConfig.DeviceQueueFamilyMask, _atlasConfig.ImmediateDeviceContext->DeviceQueueFamilyIndex))
            throw System::InvalidArgumentException("DeviceQueueFamilyMask did not support the ImmediateDeviceContext's DeviceQueueFamilyIndex!");

        if (_atlasConfig.UseCharacterRange && _atlasConfig.StartCharacterRange > _atlasConfig.EndCharacterRange)
            throw System::InvalidArgumentException("StartCharacterRange was greater than EndCharacterRange!");
    }

    // Sets font size
    {
        FT_Face ftFace = nullptr;

        // Creates the font face
        if (FT_New_Memory_Face(s_FreetypeLibrary, _fontByte.GetPointer(), FT_Long(_fontByteSize), FT_Long{0}, &ftFace))
            throw System::ExternalException("Freetype library failed to create a new font face");

        _fontFace = FontFaceRAII(ftFace);

        // Sets the font size
        if (FT_Set_Pixel_Sizes(_fontFace, 0, _fontSize)) throw System::ExternalException("Freetype library failed to set the font size");

        // Sets the font's default line spacing
        _lineHeight = (Size)(_fontFace->size->metrics.height / 64);
    }

    constexpr Size OffsetChar = 1;

    Size                                                 bufferSize   = {};
    System::List<System::Pair<WChar, System::Vector2UI>> charSizeList = {};

    /// Calculates the glyph information of each character
    ///
    /// Iterates through each character in the font's character range
    /// and calculates the glyph information of each character.
    ///
    /// accumulates the size of the font's texture atlas
    {

        auto AppendCharacter = [&](WChar character) {
            // Gets the glyph
            if (FT_Load_Char(_fontFace, character, FT_LOAD_RENDER))
                return;

            auto width  = _fontFace->glyph->bitmap.width;
            auto height = _fontFace->glyph->bitmap.rows;
            auto xOff   = _fontFace->glyph->bitmap_left;
            auto yOff   = _fontFace->glyph->bitmap_top;

            // Gets the glyph's metrics
            Glyph glyph = {
                .Bearing         = {(Int32)xOff, (Int32)yOff},
                .Advance         = {(Int32)(_fontFace->glyph->advance.x / 64.f), 0},
                .SourceRectangle = {0, 0, width, height}};

            _charGlyphs.Insert({character, glyph});

            if (width > 0 && height > 0)
                charSizeList.Append({character, {width + (OffsetChar * 2), height + (OffsetChar * 2)}});

            bufferSize += (Size)(width) * (Size)(height)*4;
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
            const Uint32 numberOfCharacters = (_fontFace)->num_glyphs;

            // Allocates the character information
            _charGlyphs.Reserve(numberOfCharacters);

            FT_UInt glyphIndex = {};

            // Gets the first glyph index
            FT_ULong character = FT_Get_First_Char(_fontFace, &glyphIndex);

            while (glyphIndex)
            {
                // Appends the character
                AppendCharacter((WChar)character);

                character = FT_Get_Next_Char(_fontFace, character, &glyphIndex);
            }
        }
    }

    Graphics::BufferDescription stagingBufferDescription = {
        .BufferSize            = bufferSize,
        .BufferBinding         = Graphics::BufferBinding::TransferSource,
        .Usage                 = Graphics::ResourceUsage::StagingSource,
        .DeviceQueueFamilyMask = Axis::System::Math::AssignBitToPosition(Size{0}, _atlasConfig.ImmediateDeviceContext->DeviceQueueFamilyIndex, true)};

    // Creates the staging buffer
    auto stagingBuffer = _atlasConfig.GraphicsDevice->CreateBuffer(stagingBufferDescription, nullptr);

    auto result = Axis::Renderer::PackSprite(charSizeList);

    Graphics::TextureDescription textureDescription = {
        .Dimension             = Graphics::TextureDimension::Texture2D,
        .Size                  = {result.PackedTextureSize.X, result.PackedTextureSize.Y, 1},
        .TextureBinding        = _atlasConfig.Binding | Graphics::TextureBinding::TransferDestination,
        .Format                = Graphics::TextureFormat::UnormR8G8B8A8,
        .MipLevels             = _atlasConfig.GenerateMip ? (Uint32)(std::floor(std::log2(std::max(result.PackedTextureSize.X, result.PackedTextureSize.Y)))) + (Uint32)1 : 1,
        .Sample                = 1,
        .ArraySize             = 1,
        .Usage                 = _atlasConfig.Usage,
        .DeviceQueueFamilyMask = _atlasConfig.DeviceQueueFamilyMask};

    // Creates the texture
    _fontAtlas     = _atlasConfig.GraphicsDevice->CreateTexture(textureDescription);
    _fontAtlasView = _fontAtlas->CreateDefaultTextureView();

    System::HashMap<WChar, Size> charOffsetMap = {};
    charOffsetMap.Reserve(result.SpriteLocations.GetSize());

    // Maps the staging buffer
    {
        Size currentBufferOffset = 0;

        Uint8* mappedMemory = (Uint8*)_atlasConfig.ImmediateDeviceContext->MapBuffer(stagingBuffer,
                                                                                     Graphics::MapAccess::Write,
                                                                                     Graphics::MapType::Overwrite);

        for (auto& characterSpriteInfo : result.SpriteLocations)
        {
            // Loads character glyph
            FT_Load_Char(_fontFace, characterSpriteInfo.First, FT_LOAD_RENDER);

            auto bitmapPixels = (characterSpriteInfo.Second.Width - (OffsetChar * 2)) * (characterSpriteInfo.Second.Height - (OffsetChar * 2));
            auto bitmapSize   = bitmapPixels * 4;

            // Copies the character glyph to the staging buffer
            for (Size i = 0; i < bitmapPixels; ++i)
            {
                mappedMemory[currentBufferOffset + (i * 4) + 0] = std::numeric_limits<Uint8>::max();
                mappedMemory[currentBufferOffset + (i * 4) + 1] = std::numeric_limits<Uint8>::max();
                mappedMemory[currentBufferOffset + (i * 4) + 2] = std::numeric_limits<Uint8>::max();
                mappedMemory[currentBufferOffset + (i * 4) + 3] = _fontFace->glyph->bitmap.buffer[i];

                static_assert(std::is_same_v<Uint8, unsigned char>);
            }

            // Stores the character offset
            charOffsetMap.Insert({characterSpriteInfo.First, currentBufferOffset});

            // Calculates the character offset
            currentBufferOffset += bitmapSize;
        }

        // Unmaps the staging buffer
        _atlasConfig.ImmediateDeviceContext->UnmapBuffer(stagingBuffer);
    }

    // Copies the staging buffer to the texture
    {
        for (auto& characterSpriteInfo : result.SpriteLocations)
        {
            auto offset = charOffsetMap.Find(characterSpriteInfo.First)->Second;

            _atlasConfig.ImmediateDeviceContext->CopyBufferToTexture(stagingBuffer,
                                                                     offset,
                                                                     _fontAtlas,
                                                                     0,
                                                                     1,
                                                                     0,
                                                                     {characterSpriteInfo.Second.X + OffsetChar, characterSpriteInfo.Second.Y + OffsetChar, 0},
                                                                     {characterSpriteInfo.Second.Width - (OffsetChar * 2), characterSpriteInfo.Second.Height - (OffsetChar * 2), 1});

            _charRects.Insert({characterSpriteInfo.First, {characterSpriteInfo.Second.X + OffsetChar, characterSpriteInfo.Second.Y + OffsetChar, characterSpriteInfo.Second.Width - (OffsetChar * 2), characterSpriteInfo.Second.Height - (OffsetChar * 2)}});
        }
    }
}

} // namespace Renderer

} // namespace Axis
