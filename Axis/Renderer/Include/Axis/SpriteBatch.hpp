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

namespace Graphics
{

// Forward declarations
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

} // namespace Graphics

namespace Window
{

class DisplayWindow;

} // namespace Window

namespace Renderer
{

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
class AXIS_RENDERER_API SpriteBatch final : public System::ISharedFromThis
{
public:
    /// \brief A hard maximum limit of sprites that can be drawn in a single batch.
    static constexpr Uint32 MaximumMaxSpritesPerBatch = (Uint32)(System::Math::RoundDown(std::numeric_limits<Uint16>::max(), (Uint16)4) / 4);

    /// \brief Constructs sprite batcher object with default batch size.
    ///
    /// \param[in] graphicsDevice Device used in graphics resource creations.
    /// \param[in] immeidateGraphicsContext Graphics device context to flush the draw commands to.
    /// \param[in] swapChain Swap chain which the sprite batch will submit the draw commands and present to.
    /// \param[in] maxSpritesPerBatch Maximum number of sprites that can be drawn in a single batch.
    SpriteBatch(const System::SharedPointer<Graphics::IGraphicsDevice>& graphicsDevice,
                const System::SharedPointer<Graphics::IDeviceContext>&  immeidateGraphicsContext,
                const System::SharedPointer<Graphics::ISwapChain>&      swapChain,
                Uint32                                                  maxSpritesPerBatch = MaximumMaxSpritesPerBatch);

    /// \brief Begins new batch group with configured render states.
    void Begin();

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] position Position on the screen to draw the sprite
    /// \param[in] colorMask Color mask to apply to the sprite
    void Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
              const System::Vector2F&                              position,
              const Graphics::ColorF&                              colorMask);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] destinationRectangle The drawing bounds on the screen
    /// \param[in] colorMask Color mask to apply to the sprite
    void Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
              const System::RectangleF&                            destinationRectangle,
              const Graphics::ColorF&                              colorMask);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] destinationRectangle The drawing bounds on the screen
    /// \param[in] sourceRectangle Region on the texture which will be rendered
    /// \param[in] colorMask Color mask to apply to the sprite
    void Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
              const System::RectangleF&                            destinationRectangle,
              const System::RectangleI&                            sourceRectangle,
              const Graphics::ColorF&                              colorMask);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] texture 2D texture to draw
    /// \param[in] position Position on the screen to draw the sprite
    /// \param[in] sourceRectangle Region on the texture which will be rendered
    /// \param[in] colorMask Color mask to apply to the sprite
    void Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
              const System::Vector2F&                              position,
              const System::RectangleI&                            sourceRectangle,
              const Graphics::ColorF&                              colorMask);

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
    void Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
              const System::RectangleF&                            destinationRectangle,
              const System::RectangleI&                            sourceRectangle,
              const Graphics::ColorF&                              colorMask,
              Float32                                              rotation,
              const System::Vector2F&                              origin,
              SpriteEffectFlags                                    spriteEffect,
              Float32                                              layerDepth);

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
    void Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
              const System::Vector2F&                              position,
              const System::RectangleI&                            sourceRectangle,
              const Graphics::ColorF&                              colorMask,
              Float32                                              rotation,
              const System::Vector2F&                              origin,
              const System::Vector2F&                              scale,
              SpriteEffectFlags                                    spriteEffect,
              Float32                                              layerDepth);

    /// \brief Submits draw command to this batch group.
    ///
    /// \param[in] rectangle Rectangle to draw on the screen.
    /// \param[in] color Color to fill the rectangle with.
    void DrawRectangle(const System::RectangleF& rectangle,
                       const Graphics::ColorF&   color);

    /// \brief Submits a text string of sprites to this batch group.
    ///
    /// \param[in] spriteFont Font to use for the text.
    /// \param[in] text Text to draw.
    /// \param[in] position Position on the screen to draw the text.
    /// \param[in] colorMask A color mask to apply to the text.
    void DrawString(const System::SharedPointer<SpriteFont>& spriteFont,
                    System::StringView<WChar>                text,
                    const System::Vector2F&                  position,
                    const Graphics::ColorF&                  colorMask);

    /// \brief Flushes all batched sprites to the context.
    void End();

    /// \brief \a `SpriteBatch`'s internal vertex data structure
    struct Vertex final
    {
        /// \brief Constructs vertex object
        Vertex(const System::Vector3F& position,
               const Graphics::ColorF& colorMask,
               const System::Vector2F& textureCoordinate) noexcept;

        /// \brief Vertex position
        System::Vector3F Position = {};

        /// \brief Color mask
        Graphics::ColorF ColorMask = {};

        /// \brief UV Texture coordinate
        System::Vector2F TextureCoordinate = {};
    };

private:
    /// Used in pipeline caching
    struct PipelineStateKey final
    {
        Graphics::AttachmentBlendState Blend;      // User-defined custom blend state
        Graphics::DepthStencilState    Depth;      // User-defined custom depth stencil state
        Graphics::RasterizerState      Rasterizer; // User-defined custom rasterizer state

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
        AXIS_NODISCARD Size operator()(const Graphics::SamplerDescription&) const noexcept;
    };

    /// Used in sampler caching
    struct SamplerDescriptionComparer
    {
        AXIS_NODISCARD Bool operator()(const Graphics::SamplerDescription&,
                                       const Graphics::SamplerDescription&) const noexcept;
    };

    /// Using
    using PipelineCache = System::HashMap<PipelineStateKey, System::SharedPointer<Graphics::IGraphicsPipeline>, PipelineStateKey::Hasher>;
    using SamplerCache  = System::HashMap<Graphics::SamplerDescription, System::SharedPointer<Graphics::ISampler>, SamplerDescriptionHasher, SamplerDescriptionComparer>;
    using IndexType     = Uint16; // Index buffer's index type

    /// Private method
    System::SharedPointer<Graphics::IGraphicsPipeline> GetGraphicsPipeline(const PipelineStateKey& key);
    System::SharedPointer<Graphics::ISampler>          GetSampler(const Graphics::SamplerDescription& samplerDesc);
    void                                               UpdateTranslationMatrix();
    template <Bool IncrementCount = true> void         PreDraw(const System::SharedPointer<Graphics::ITextureView>& texture);
    void                                               Flush();

    /// Appends sprite to the batch (Draw texture with no rotation)
    void AppendBatch(const System::Vector3F& position,
                     const System::Vector2F& size,
                     const System::Vector2F& texCoordTL,
                     const System::Vector2F& texCoordBR,
                     const Graphics::ColorF& colorMask);

    /// Appends sprite to the batch (Draw texture with rotation and sprite effect)
    void AppendBatch(const System::Vector3F& position,
                     const System::Vector2F& delta,
                     const System::Vector2F& size,
                     Float32                 sin,
                     Float32                 cos,
                     const System::Vector2F& texCoordTL,
                     const System::Vector2F& texCoordBR,
                     const Graphics::ColorF& colorMask);

    /// Private members
    System::SharedPointer<Graphics::IGraphicsDevice>     _graphicsDevice                 = nullptr;
    System::SharedPointer<Graphics::IDeviceContext>      _immediateGraphicsDeviceContext = nullptr;
    System::SharedPointer<Graphics::ISwapChain>          _swapChain                      = nullptr;
    System::SharedPointer<Graphics::IShaderModule>       _vertexShaderModule             = nullptr;
    System::SharedPointer<Graphics::IShaderModule>       _fragmentShaderModule           = nullptr;
    System::SharedPointer<Graphics::IResourceHeap>       _resourceHeap                   = nullptr;
    System::SharedPointer<Graphics::IResourceHeapLayout> _resourceHeapLayout             = nullptr;
    System::SharedPointer<Graphics::ITextureView>        _whiteTextureView               = nullptr;
    Uint32                                               _maxSpriteCountsPerBatch        = 0;

    SamplerCache  _samplerCaches  = {};
    PipelineCache _pipelineCaches = {};

    /// Batch states
    System::SharedPointer<Graphics::IBuffer> _vertexBuffer = {};
    System::SharedPointer<Graphics::IBuffer> _indexBuffer  = {};
    System::List<Vertex>                     _vertices     = {}; // Contains vertices data
    System::List<IndexType>                  _indices      = {}; // Contains indices data
    Bool                                     _isBegun      = false;
    Size                                     _spriteCount  = 0;

    /// Graphics states
    PipelineStateKey                                   _currentPipelineStateKey   = {};      // Current pipeline state key (Used in caching)
    Graphics::SamplerDescription                       _currentSamplerDescription = {};      // Current sampler description
    System::SharedPointer<Graphics::ISampler>          _currentSampler            = nullptr; // Current configured sampler object
    System::SharedPointer<Graphics::IGraphicsPipeline> _currentPipeline           = nullptr; // Current configured pipeline object
    System::SharedPointer<Graphics::ITextureView>      _currentTextureView        = nullptr; // Next texture to draw
    System::SharedPointer<Graphics::IBuffer>           _currentMatrixTranslation  = nullptr; // Uniform buffer contains matrix translation.
    Bool                                               _graphicsStateChanged      = true;    // Specifies whether graphics state has changed or not.
    Bool                                               _samplerStateChanged       = true;    // Specifies whether sampler state has changed or not.

    System::EventToken<void(Window::DisplayWindow&, System::Vector2UI)> _eventToken = {}; // Event token for window resize event

    static constexpr ::Axis::Graphics::IndexType IndexEnumType = ::Axis::Graphics::IndexType::Uint16; // `Axis::IndexType` enumeration used in index buffer binding
};

} // namespace Renderer

} // namespace Axis

#endif // AXIS_RENDERER_SPRITEBATCH_HPP
