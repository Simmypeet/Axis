/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_RENDERER_SPRITEFONT_HPP
#define AXIS_RENDERER_SPRITEFONT_HPP
#pragma once

#include "../../../Graphics/Include/Axis/Texture.hpp"
#include "../../../System/Include/Axis/FileStream.hpp"
#include "../../../System/Include/Axis/HashMap.hpp"
#include "../../../System/Include/Axis/Rectangle.hpp"
#include "../../../System/Include/Axis/SmartPointer.hpp"
#include "../../../System/Include/Axis/Vector2.hpp"
#include "RendererExport.hpp"

namespace Axis
{

/// Forward declarations
class IGraphicsDevice;
class IDeviceContext;
class ITexture;

/// \brief Contains font's glyph information in a character
struct Glyph
{
    /// \brief Offset of the bitmap font (Assumed if the font is drawn in the top-left corner position) (in pixel).
    Vector2F Bearing = {};

    /// \brief Advance of the bitmap font (in pixel).
    Vector2F Advance = {};

    /// \brief Area in the texture where the glyph is located.
    RectangleF SourceRectangle = {};
};

/// \brief Specifies how sprite font should create font atlas texture.
struct FontAtlasConfiguration
{
    /// \brief Graphics device used in texture creations.
    SharedPointer<IGraphicsDevice> GraphicsDevice = {};

    /// \brief Immediate device context used in data transferation.
    SharedPointer<IDeviceContext> ImmediateDeviceContext = {};

    /// \brief Specifies whether to generate mip level chains or not.
    Bool GenerateMip = {};

    /// \brief Specifies the resource usage of the created textures.
    ResourceUsage Usage = {};

    /// \brief Specifies the binding of the created textures.
    TextureBinding Binding = {};

    /// \brief Specifies which device queue family index (at specifies bit position) can use this resource.
    ///
    /// \note Only specifies the device queue family which will use this resource.
    ///       Do not set any device queue family index which won't use this resource unnecessary, this will cause extra overhead.
    ///
    /// \see Axis::Math::AssignBitToPosition
    Uint64 DeviceQueueFamilyMask = {};

    /// \brief Specifies the initial range of the character set.
    WChar StartCharacterRange = {};

    /// \brief Specifies the final range of the character set.
    WChar EndCharacterRange = {};

    /// \brief Generates the font atlas using the specified character range
    ///        or generates the font atlas by all available characters in the font file.
    Bool UseCharacterRange = {};
};

/// \brief Contains loaded font information and font atlas texture.
///
class AXIS_RENDERER_API SpriteFont : public ISharedFromThis
{
public:
    /// \brief Passes this value to \a `Axis::SpriteFont::LoadFont` to
    ///        let only passed immeidateDeviceContext can use sprite font atlas.
    static constexpr Uint64 DefaultDeviceQueueFamilyMask = 0;

    /// \brief Loads font from the specified file stream.
    ///
    /// \param[in] fileStream True type font file stream. The filestream must be opened in binary mode and must be readable.
    /// \param[in] size Size or height of the font (in pixel).
    /// \param[in] atlasConfiguration Configuration of the font atlas texture.
    SpriteFont(FileStream&                   fileStream,
               Uint32                        size,
               const FontAtlasConfiguration& atlasConfiguration);

    /// \brief Loads font from the specified font data.
    ///
    /// \param[in] fontData True type font data. The font data will be copied.
    /// \param[in] fontDataSize Size (in bytes) of `fontData`.
    /// \param[in] size Size or height of the font (in pixel).
    /// \param[in] atlasConfiguration Configuration of the font atlas texture.
    SpriteFont(CPVoid                        fontData,
               Size                          fontDataSize,
               Uint32                        fontSize,
               const FontAtlasConfiguration& atlasConfiguration);

    /// \brief Destructor
    ~SpriteFont() noexcept;

    /// \brief Gets the font atlas texture view.
    ///
    /// \return Font atlas texture.
    inline const SharedPointer<ITextureView>& GetFontAtlas() const noexcept { return _fontAtlasView; }

    /// \brief Gets characters atlas texture rectangle map.
    ///
    /// \return Characters atlas texture rectangle map.
    inline const HashMap<WChar, RectangleI>& GetCharactersRectangleMap() const noexcept { return _charRects; }

    /// \brief Gets characters glyph map.
    ///
    /// \return Characters glyph map.
    inline const HashMap<WChar, Glyph>& GetCharacterGlyphsMap() const noexcept { return _charGlyphs; }

    /// \brief Gets font size.
    ///
    /// \return Font size.
    inline Size GetFontSize() const noexcept { return _fontSize; }

    /// \brief Gets line spacing height (in pixel).
    ///
    /// \return Line spacing height.
    inline Float32 GetLineHeight() const noexcept { return _lineHeight; }

    /// \brief Sets new line spacing height (in pixel).
    ///
    /// \param[in] lineHeight New line spacing height.
    inline void SetLineHeight(Float32 lineHeight) noexcept { _lineHeight = lineHeight; }

    /// \brief Calculates the size of the specified string (in pixel)
    ///        if it was rendered with this font.
    ///
    /// \param[in] string String to calculate size of.
    ///
    /// \return Size of the specified string (in pixel).
    inline Vector2F MeasureString(const StringView<WChar>& string) const noexcept;

private:
    /// Private methods
    void Initialize(); ///< Initializes the font

    PVoid                       _fontFace      = nullptr; // FT_Face
    UniquePointer<Byte[]>       _fontByte      = nullptr;
    Size                        _fontByteSize  = 0;
    Uint32                      _fontSize      = 0;       // Font Size
    SharedPointer<ITexture>     _fontAtlas     = nullptr; // Font atlas texture
    SharedPointer<ITextureView> _fontAtlasView = nullptr; // Font atlas texture view
    HashMap<WChar, RectangleI>  _charRects     = {};      // Character rectangles
    HashMap<WChar, Glyph>       _charGlyphs    = {};      // Character glyphs
    FontAtlasConfiguration      _atlasConfig   = {};      // Font atlas configuration
    Float32                     _lineHeight    = 0;       // Line height
};

} // namespace Axis

#endif // AXIS_RENDERER_SPRITEFONT_HPP
