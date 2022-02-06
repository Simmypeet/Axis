/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_RENDERER_SPRITEBATCH_HPP
#define AXIS_RENDERER_SPRITEBATCH_HPP
#pragma once

#include "../../Graphics/Include/Axis/DeviceContext.hpp"
#include "../../Graphics/Include/Axis/GraphicsPipeline.hpp"
#include "../../Graphics/Include/Axis/Sampler.hpp"
#include "../../Graphics/Include/Axis/ShaderModule.hpp"
#include "../../System/Include/Axis/Event.hpp"
#include "../../System/Include/Axis/HashMap.hpp"
#include "../../System/Include/Axis/SmartPointer.hpp"
#include "../../System/Include/Axis/StringView.hpp"
#include "../../System/Include/Axis/Vector2.hpp"
#include "RendererExport.hpp"


namespace Axis
{

/// Forward declarations
class IGraphicsDevice;
class IDeviceContext;
class IBuffer;
class ITexture;
class ITextureView;
class IFence;
class IPipeline;
class ISampler;
class IResourceHeap;
class IResourceHeapLayout;
class ISwapChain;
class DisplayWindow;
class SpriteFont;

/// \brief Basic sprite rendering effect.
enum class SpriteEffect : Uint8
{
    /// \brief No effect applied.
    None = 0,

    /// \brief Flips sprite image along the Y-axis.
    FlipHorizontally = AXIS_BIT(1),

    /// \brief Flips sprite image along the X-axis.
    FlipVertically = AXIS_BIT(2),

    /// \brief Required for enum reflection.
    MaximumEnumValue = FlipVertically
};

/// \brief Basic sprite rendering effect. (Bit masks)
typedef SpriteEffect SpriteEffectFlags;

/// \brief Helper class for drawing sprites/images in more optimized batches.
class AXIS_RENDERER_API SpriteBatch final : public ISharedFromThis
{
public:
    /// \brief A hard maximum limit of sprites that can be drawn in a single batch.
    static constexpr Uint32 MaximumMaxSpritesPerBatch = (Uint32)(Math::RoundDown(std::numeric_limits<Uint16>::max(), (Uint16)4) / 4);

    /// \brief Constructs sprite batcher object with default batch size.
    ///
    /// \param[in] graphicsDevice Device used in graphics resource creations.
    /// \param[in] immeidateGraphicsContext Graphics device context to flush the draw commands to.
    /// \param[in] swapChain Swap chain which the sprite batch will submit the draw commands and present to.
    /// \param[in] maxSpritesPerBatch Maximum number of sprites that can be drawn in a single batch.
    SpriteBatch(const SharedPointer<IGraphicsDevice>& graphicsDevice,
                const SharedPointer<IDeviceContext>&  immeidateGraphicsContext,
                const SharedPointer<ISwapChain>&      swapChain,
                Uint32                                maxSpritesPerBatch = MaximumMaxSpritesPerBatch);

    /// \brief Begins new batch group with configured render states.
    void Begin();

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] position Position on the screen to draw the sprite
    /// \param[in] colorMask Color mask to apply to the sprite
    void Draw(const SharedPointer<ITextureView>& texture,
              const Vector2F&                    position,
              const ColorF&                      colorMask);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] destinationRectangle The drawing bounds on the screen
    /// \param[in] colorMask Color mask to apply to the sprite
    void Draw(const SharedPointer<ITextureView>& texture,
              const RectangleF&                  destinationRectangle,
              const ColorF&                      colorMask);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] destinationRectangle The drawing bounds on the screen
    /// \param[in] sourceRectangle Region on the texture which will be rendered
    /// \param[in] colorMask Color mask to apply to the sprite
    void Draw(const SharedPointer<ITextureView>& texture,
              const RectangleF&                  destinationRectangle,
              const RectangleI&                  sourceRectangle,
              const ColorF&                      colorMask);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] position Position on the screen to draw the sprite
    /// \param[in] sourceRectangle Region on the texture which will be rendered
    /// \param[in] colorMask Color mask to apply to the sprite
    void Draw(const SharedPointer<ITextureView>& texture,
              const Vector2F&                    position,
              const RectangleI&                  sourceRectangle,
              const ColorF&                      colorMask);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] destinationRectangle The drawing bounds on the screen
    /// \param[in] sourceRectangle Region on the texture which will be rendered
    /// \param[in] colorMask Color mask to apply to the sprite
    /// \param[in] rotation A rotation of this sprite (in randian)
    /// \param[in] origin Center of the rotation in the drawing bound.
    /// \param[in] spriteEffect Basic sprite effect to apply to the texture
    /// \param[in] layerDepth A depth of the layer of this sprite
    void Draw(const SharedPointer<ITextureView>& texture,
              const RectangleF&                  destinationRectangle,
              const RectangleI&                  sourceRectangle,
              const ColorF&                      colorMask,
              Float32                            rotation,
              const Vector2F&                    origin,
              SpriteEffectFlags                  spriteEffect,
              Float32                            layerDepth);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] position Position on the screen to draw the sprite
    /// \param[in] sourceRectangle Region on the texture which will be rendered
    /// \param[in] colorMask Color mask to apply to the sprite
    /// \param[in] rotation A rotation of this sprite (in randian)
    /// \param[in] origin Center of the rotation in the drawing bound.
    /// \param[in] scale A scaling of the sprite in x and y axes.
    /// \param[in] spriteEffect Basic sprite effect to apply to the texture
    /// \param[in] layerDepth A depth of the layer of this sprite
    void Draw(const SharedPointer<ITextureView>& texture,
              const Vector2F&                    position,
              const RectangleI&                  sourceRectangle,
              const ColorF&                      colorMask,
              Float32                            rotation,
              const Vector2F&                    origin,
              const Vector2F&                    scale,
              SpriteEffectFlags                  spriteEffect,
              Float32                            layerDepth);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] rectangle Rectangle to draw on the screen.
    /// \param[in] color Color to fill the rectangle with.
    void DrawRectangle(const RectangleF& rectangle,
                       const ColorF&     color);

    /// \brief Submits a text string of sprites to this batch group.
    ///
    /// \param[in] font Font to use for the text.
    /// \param[in] text Text to draw.
    /// \param[in] position Position on the screen to draw the text.
    /// \param[in] colorMask A color mask to apply to the text.
    ///
    void DrawString(const SharedPointer<SpriteFont>& spriteFont,
                    StringView<WChar>                text,
                    const Vector2F&                  position,
                    const ColorF&                    colorMask);

    /// \brief Flushes all batched sprites to the context.
    void End();

    /// \brief \a `Axis::Renderer::SpriteBatch`'s internal vertex data structure
    struct Vertex final
    {
        /// \brief Constructs vertex object
        Vertex(const Vector3F& position,
               const ColorF&   colorMask,
               const Vector2F& textureCoordinate) noexcept;

        /// \brief Vertex position
        Vector3F Position = {};

        /// \brief Color mask
        ColorF ColorMask = {};

        /// \brief UV Texture coordinate
        Vector2F TextureCoordinate = {};
    };

private:
    /// Used in pipeline caching
    struct PipelineStateKey final
    {
        AttachmentBlendState Blend;      // User-defined custom blend state
        DepthStencilState    Depth;      // User-defined custom depth stencil state
        RasterizerState      Rasterizer; // User-defined custom rasterizer state

        /// Equality comparison
        AXIS_NODISCARD Bool operator==(const PipelineStateKey&) const noexcept;

        /// Calculates hash
        struct Hasher final
        {
            AXIS_NODISCARD Size operator()(const PipelineStateKey&) const noexcept;
        };
    };

    /// Used in sampler caching
    struct SamplerDescriptionHasher
    {
        AXIS_NODISCARD Size operator()(const SamplerDescription&) const noexcept;
    };

    /// Used in sampler caching
    struct SamplerDescriptionComparer
    {
        AXIS_NODISCARD Bool operator()(const SamplerDescription&, const SamplerDescription&) const noexcept;
    };

    /// Using
    using PipelineCache = HashMap<PipelineStateKey, SharedPointer<IGraphicsPipeline>, PipelineStateKey::Hasher>;
    using SamplerCache  = HashMap<SamplerDescription, SharedPointer<ISampler>, SamplerDescriptionHasher, SamplerDescriptionComparer>;
    using IndexType     = Uint16; // Index buffer's index type

    /// Private method
    SharedPointer<IGraphicsPipeline>           GetGraphicsPipeline(const PipelineStateKey& key);
    SharedPointer<ISampler>                    GetSampler(const SamplerDescription& samplerDesc);
    void                                       UpdateTranslationMatrix();
    template <Bool IncrementCount = true> void PreDraw(const SharedPointer<ITextureView>& texture);
    void                                       Flush();

    /// Appends sprite to the batch (Draw texture with no rotation)
    void AppendBatch(const Vector3F& position,
                     const Vector2F& size,
                     const Vector2F& texCoordTL,
                     const Vector2F& texCoordBR,
                     const ColorF&   colorMask);

    /// Appends sprite to the batch (Draw texture with rotation and sprite effect)
    void AppendBatch(const Vector3F& position,
                     const Vector2F& delta,
                     const Vector2F& size,
                     Float32         sin,
                     Float32         cos,
                     const Vector2F& texCoordTL,
                     const Vector2F& texCoordBR,
                     const ColorF&   colorMask);

    /// Private members
    SharedPointer<IGraphicsDevice>     _graphicsDevice                 = nullptr;
    SharedPointer<IDeviceContext>      _immediateGraphicsDeviceContext = nullptr;
    SharedPointer<ISwapChain>          _swapChain                      = nullptr;
    SharedPointer<IShaderModule>       _vertexShaderModule             = nullptr;
    SharedPointer<IShaderModule>       _fragmentShaderModule           = nullptr;
    SharedPointer<IResourceHeap>       _resourceHeap                   = nullptr;
    SharedPointer<IResourceHeapLayout> _resourceHeapLayout             = nullptr;
    SharedPointer<ITextureView>        _whiteTextureView               = nullptr;
    Uint32                             _maxSpriteCountsPerBatch        = 0;

    SamplerCache  _samplerCaches  = {};
    PipelineCache _pipelineCaches = {};

    /// Batch states
    SharedPointer<IBuffer> _vertexBuffer = {};
    SharedPointer<IBuffer> _indexBuffer  = {};
    List<Vertex>           _vertices     = {}; // Contains vertices data
    List<IndexType>        _indices      = {}; // Contains indices data
    Bool                   _isBegun      = false;
    Size                   _spriteCount  = 0;

    /// Graphics states
    PipelineStateKey                 _currentPipelineStateKey   = {};      // Current pipeline state key (Used in caching)
    SamplerDescription               _currentSamplerDescription = {};      // Current sampler description
    SharedPointer<ISampler>          _currentSampler            = nullptr; // Current configured sampler object
    SharedPointer<IGraphicsPipeline> _currentPipeline           = nullptr; // Current configured pipeline object
    SharedPointer<ITextureView>      _currentTextureView        = nullptr; // Next texture to draw
    SharedPointer<IBuffer>           _currentMatrixTranslation  = nullptr; // Uniform buffer contains matrix translation.
    Bool                             _graphicsStateChanged      = true;    // Specifies whether graphics state has changed or not.
    Bool                             _samplerStateChanged       = true;    // Specifies whether sampler state has changed or not.

    EventToken<void(DisplayWindow&, Vector2UI)> _eventToken = {}; // Event token for window resize event

    static constexpr ::Axis::IndexType IndexEnumType = ::Axis::IndexType::Uint16; // `Axis::IndexType` enumeration used in index buffer binding
};

} // namespace Axis

#endif // AXIS_RENDERER_SPRITEBATCH_HPP