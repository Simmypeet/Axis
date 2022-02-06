/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/RendererPch.hpp>

#include <Axis/Buffer.hpp>
#include <Axis/DeviceContext.hpp>
#include <Axis/DisplayWindow.hpp>
#include <Axis/Exception.hpp>
#include <Axis/Fence.hpp>
#include <Axis/GraphicsDevice.hpp>
#include <Axis/GraphicsPipeline.hpp>
#include <Axis/Matrix.hpp>
#include <Axis/Pipeline.hpp>
#include <Axis/RendererPch.hpp>
#include <Axis/ResourceHeap.hpp>
#include <Axis/ResourceHeapLayout.hpp>
#include <Axis/ShaderModule.hpp>
#include <Axis/SpriteBatch.hpp>
#include <Axis/SpriteFont.hpp>
#include <Axis/SwapChain.hpp>
#include <Axis/Texture.hpp>

// GLSL vertex shader code
constexpr const char* SpriteBatchVertexShaderCode = R"""(
#version 450

layout(location = 0) in vec3 VertPositionIn;
layout(location = 1) in vec4 VertColorIn;
layout(location = 2) in vec2 VertTexCoord;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec2 FragTexCoord;

layout(binding = 0) uniform TranslationMatrix {
    mat4 matrix;
} translationMatrix;

void main() 
{
    gl_Position = translationMatrix.matrix * vec4(VertPositionIn, 1.0);
    FragColor = VertColorIn;
    FragTexCoord = VertTexCoord;
}
)""";

// GLSL vertex shader code
constexpr const char* SpriteBatchFragmentShaderCode = R"""(
#version 450

layout(location = 0) in vec4 FragColorIn;
layout(location = 1) in vec2 FragTexCoordIn;

layout(location = 0) out vec4 OutColor;

layout(binding = 1) uniform sampler2D TextSampler;

void main() 
{
    OutColor = FragColorIn * texture(TextSampler, FragTexCoordIn);
}
)""";

namespace Axis
{

SpriteBatch::SpriteBatch(const SharedPointer<IGraphicsDevice>& graphicsDevice,
                         const SharedPointer<IDeviceContext>&  immediateGraphicsContext,
                         const SharedPointer<ISwapChain>&      swapChain,
                         Uint32                                maxSpriteCountsPerBatch) :
    _graphicsDevice(graphicsDevice),
    _immediateGraphicsDeviceContext(immediateGraphicsContext),
    _swapChain(swapChain),
    _maxSpriteCountsPerBatch(Math::Min(maxSpriteCountsPerBatch, MaximumMaxSpritesPerBatch))
{
    auto resizeEvent = [this](DisplayWindow&, Vector2UI) {
        UpdateTranslationMatrix();
    };

    _eventToken = EventToken<void(DisplayWindow&, Vector2UI)>(swapChain->Description.TargetWindow->GetClientSizeChangedEvent(), resizeEvent);

    // Creates vertex shader
    ShaderModuleDescription vertexShaderDescription = {};
    vertexShaderDescription.EntryPoint              = "main";
    vertexShaderDescription.Language                = ShaderLanguage::GLSL;
    vertexShaderDescription.Stage                   = ShaderStage::Vertex;
    _vertexShaderModule                             = _graphicsDevice->CompileShaderModule(vertexShaderDescription, ::SpriteBatchVertexShaderCode);

    // Creates fragment shader
    ShaderModuleDescription fragmentShaderDescription = {};
    fragmentShaderDescription.EntryPoint              = "main";
    fragmentShaderDescription.Language                = ShaderLanguage::GLSL;
    fragmentShaderDescription.Stage                   = ShaderStage::Fragment;
    _fragmentShaderModule                             = _graphicsDevice->CompileShaderModule(fragmentShaderDescription, ::SpriteBatchFragmentShaderCode);

    ResourceHeapLayoutDescription resourceHeapLayoutDescription = {};
    resourceHeapLayoutDescription.ResourceBindings              = List<ResourceLayoutBinding>(2);

    // For translation matrix in vertex shader
    auto& vertexTranslationMatrixLayout        = resourceHeapLayoutDescription.ResourceBindings[0];
    vertexTranslationMatrixLayout.ArraySize    = 1;
    vertexTranslationMatrixLayout.Binding      = ResourceBinding::UniformBuffer;
    vertexTranslationMatrixLayout.BindingIndex = 0;
    vertexTranslationMatrixLayout.StageFlags   = ShaderStage::Vertex;

    // For sampler in fragment shader
    auto& fragmentSamplerLayout        = resourceHeapLayoutDescription.ResourceBindings[1];
    fragmentSamplerLayout.ArraySize    = 1;
    fragmentSamplerLayout.Binding      = ResourceBinding::Sampler;
    fragmentSamplerLayout.BindingIndex = 1;
    fragmentSamplerLayout.StageFlags   = ShaderStage::Fragment;

    // Creates resource heap layout
    _resourceHeapLayout = _graphicsDevice->CreateResourceHeapLayout(resourceHeapLayoutDescription);

    // Creates resource heap
    _resourceHeap = _graphicsDevice->CreateResourceHeap({_resourceHeapLayout});

    // Creates vertex buffer
    BufferDescription vertexBufferDescription     = {};
    vertexBufferDescription.BufferBinding         = BufferBinding::Vertex;
    vertexBufferDescription.BufferSize            = maxSpriteCountsPerBatch * sizeof(Vertex) * 4;
    vertexBufferDescription.Usage                 = ResourceUsage::Dynamic;
    vertexBufferDescription.DeviceQueueFamilyMask = Math::AssignBitToPosition(vertexBufferDescription.DeviceQueueFamilyMask, _swapChain->Description.ImmediateGraphicsContext->DeviceQueueFamilyIndex, true);
    _vertexBuffer                                 = graphicsDevice->CreateBuffer(vertexBufferDescription, nullptr);

    // Creates index buffer
    BufferDescription indexBufferDescription     = {};
    indexBufferDescription.BufferBinding         = BufferBinding::Index;
    indexBufferDescription.BufferSize            = maxSpriteCountsPerBatch * sizeof(IndexType) * 6;
    indexBufferDescription.Usage                 = ResourceUsage::Dynamic;
    indexBufferDescription.DeviceQueueFamilyMask = Math::AssignBitToPosition(indexBufferDescription.DeviceQueueFamilyMask, _swapChain->Description.ImmediateGraphicsContext->DeviceQueueFamilyIndex, true);
    _indexBuffer                                 = graphicsDevice->CreateBuffer(indexBufferDescription, nullptr);

    // Creates uniform buffer
    BufferDescription uniformBufferDescription     = {};
    uniformBufferDescription.BufferBinding         = BufferBinding::Uniform | BufferBinding::TransferDestination;
    uniformBufferDescription.BufferSize            = sizeof(FloatMatrix4x4);
    uniformBufferDescription.Usage                 = ResourceUsage::Dynamic;
    uniformBufferDescription.DeviceQueueFamilyMask = Math::AssignBitToPosition(uniformBufferDescription.DeviceQueueFamilyMask, _swapChain->Description.ImmediateGraphicsContext->DeviceQueueFamilyIndex, true);
    _currentMatrixTranslation                      = _graphicsDevice->CreateBuffer(uniformBufferDescription, nullptr);

    UpdateTranslationMatrix();

    SharedPointer<IBuffer> uniformBuffers[] = {_currentMatrixTranslation};

    _resourceHeap->BindBuffers(0, uniformBuffers);

    _currentPipelineStateKey.Depth      = DepthStencilState::GetNone();
    _currentPipelineStateKey.Rasterizer = RasterizerState::GetCullNone();
    _currentPipelineStateKey.Blend      = AttachmentBlendState::GetAlphaBlend();

    _currentSamplerDescription = SamplerDescription::GetPointClamp();

    // Creates white texture
    TextureDescription whiteTextureDescription = {
        .Dimension             = TextureDimension::Texture2D,
        .Size                  = {1, 1, 1},
        .TextureBinding        = TextureBinding::Sampled | TextureBinding::TransferDestination,
        .Format                = TextureFormat::UnormR8G8B8A8,
        .MipLevels             = 1,
        .Sample                = 1,
        .ArraySize             = 1,
        .Usage                 = ResourceUsage::Immutable,
        .DeviceQueueFamilyMask = (Uint64)Math::AssignBitToPosition(0, immediateGraphicsContext->DeviceQueueFamilyIndex, true)};

    auto whiteTexture = graphicsDevice->CreateTexture(whiteTextureDescription);

    BufferDescription stagingWhiteTextureBufferDescription = {
        .BufferSize            = 4,
        .BufferBinding         = BufferBinding::TransferSource,
        .Usage                 = ResourceUsage::Dynamic,
        .DeviceQueueFamilyMask = (Uint64)Math::AssignBitToPosition(0, immediateGraphicsContext->DeviceQueueFamilyIndex, true)};

    auto stagingWhiteTextureBuffer = graphicsDevice->CreateBuffer(stagingWhiteTextureBufferDescription, nullptr);

    ColorUI8 whiteColor = {255, 255, 255, 255};

    auto mappedMemory = _immediateGraphicsDeviceContext->MapBuffer(stagingWhiteTextureBuffer, MapAccess::Write, MapType::Overwrite);

    std::memcpy(mappedMemory, &whiteColor, sizeof(ColorUI8));

    _immediateGraphicsDeviceContext->UnmapBuffer(stagingWhiteTextureBuffer);

    _immediateGraphicsDeviceContext->CopyBufferToTexture(stagingWhiteTextureBuffer,
                                                         0,
                                                         whiteTexture,
                                                         0,
                                                         1,
                                                         0,
                                                         {0, 0, 0},
                                                         {1, 1, 1});

    _whiteTextureView = whiteTexture->CreateDefaultTextureView();

    _immediateGraphicsDeviceContext->Flush();
}

void SpriteBatch::Begin()
{
    if (_isBegun)
        throw InvalidOperationException("SpriteBatch already begun!");

    if (_samplerStateChanged)
    {
        _currentSampler      = GetSampler(_currentSamplerDescription);
        _samplerStateChanged = false;
    }

    if (_graphicsStateChanged)
    {
        _currentPipeline      = GetGraphicsPipeline(_currentPipelineStateKey);
        _graphicsStateChanged = false;
    }

    _isBegun = true;
}

void SpriteBatch::Draw(const SharedPointer<ITextureView>& texture,
                       const Vector2F&                    position,
                       const ColorF&                      colorMask)
{
    PreDraw(texture);

    AppendBatch({position.X, position.Y, 0},
                {texture->Description.ViewTexture->Description.Size.X, texture->Description.ViewTexture->Description.Size.Y},
                {0.0f, 0.0f},
                {1.0f, 1.0f},
                colorMask);
}

void SpriteBatch::Draw(const SharedPointer<ITextureView>& texture,
                       const RectangleF&                  destinationRectangle,
                       const ColorF&                      colorMask)
{
    PreDraw(texture);

    AppendBatch({destinationRectangle.X, destinationRectangle.Y, 0},
                {destinationRectangle.Width, destinationRectangle.Height},
                {0.0f, 0.0f},
                {1.0f, 1.0f},
                colorMask);
}

void SpriteBatch::Draw(const SharedPointer<ITextureView>& texture,
                       const RectangleF&                  destinationRectangle,
                       const RectangleI&                  sourceRectangle,
                       const ColorF&                      colorMask)
{
    PreDraw(texture);

    Vector2F texCoordTL = {(Float32)sourceRectangle.X / texture->Description.ViewTexture->Description.Size.X,
                           (Float32)sourceRectangle.Y / texture->Description.ViewTexture->Description.Size.Y};

    Vector2F texCoordBR = {texCoordTL.X + ((Float32)sourceRectangle.Width / texture->Description.ViewTexture->Description.Size.X),
                           texCoordTL.Y + ((Float32)sourceRectangle.Height / texture->Description.ViewTexture->Description.Size.Y)};

    AppendBatch({destinationRectangle.X, destinationRectangle.Y, 0},
                {destinationRectangle.Width, destinationRectangle.Height},
                texCoordTL,
                texCoordBR,
                colorMask);
}

void SpriteBatch::Draw(const SharedPointer<ITextureView>& texture,
                       const Vector2F&                    position,
                       const RectangleI&                  sourceRectangle,
                       const ColorF&                      colorMask)
{
    PreDraw(texture);

    Vector2F texCoordTL = {(Float32)sourceRectangle.X / texture->Description.ViewTexture->Description.Size.X,
                           (Float32)sourceRectangle.Y / texture->Description.ViewTexture->Description.Size.Y};

    Vector2F texCoordBR = {texCoordTL.X + ((Float32)sourceRectangle.Width / texture->Description.ViewTexture->Description.Size.X),
                           texCoordTL.Y + ((Float32)sourceRectangle.Height / texture->Description.ViewTexture->Description.Size.Y)};

    AppendBatch({position.X, position.Y, 0},
                {sourceRectangle.Width, sourceRectangle.Height},
                texCoordTL,
                texCoordBR,
                colorMask);
}

void SpriteBatch::Draw(const SharedPointer<ITextureView>& texture,
                       const RectangleF&                  destinationRectangle,
                       const RectangleI&                  sourceRectangle,
                       const ColorF&                      colorMask,
                       Float32                            rotation,
                       const Vector2F&                    origin,
                       SpriteEffectFlags                  spriteEffect,
                       Float32                            layerDepth)
{
    PreDraw(texture);

    Vector2F spriteOrigin = origin;

    Vector2F texCoordTL = {(Float32)sourceRectangle.X / texture->Description.ViewTexture->Description.Size.X,
                           (Float32)sourceRectangle.Y / texture->Description.ViewTexture->Description.Size.Y};
    Vector2F texCoordBR = {(Float32)(sourceRectangle.X + sourceRectangle.Width) / texture->Description.ViewTexture->Description.Size.X,
                           (Float32)(sourceRectangle.Y + sourceRectangle.Height) / texture->Description.ViewTexture->Description.Size.Y};

    if ((Bool)(spriteEffect & SpriteEffect::FlipVertically))
    {
        auto temp    = texCoordBR.Y;
        texCoordBR.Y = texCoordTL.Y;
        texCoordTL.Y = temp;
    }

    if ((Bool)(spriteEffect & SpriteEffect::FlipHorizontally))
    {
        auto temp    = texCoordBR.X;
        texCoordBR.X = texCoordTL.X;
        texCoordTL.X = temp;
    }

    if (rotation == 0)
    {
        AppendBatch(Vector3F(destinationRectangle.X - spriteOrigin.X, destinationRectangle.Y - spriteOrigin.Y, layerDepth),
                    Vector2F(destinationRectangle.Width, destinationRectangle.Height),
                    texCoordTL,
                    texCoordBR,
                    colorMask);
    }
    else
    {
        AppendBatch(Vector3F(destinationRectangle.X, destinationRectangle.Y, layerDepth),
                    Vector2F(-spriteOrigin.X, -spriteOrigin.Y),
                    Vector2F(destinationRectangle.Width, destinationRectangle.Height),
                    std::sin(rotation),
                    std::cos(rotation),
                    texCoordTL,
                    texCoordBR,
                    colorMask);
    }
}

void SpriteBatch::Draw(const SharedPointer<ITextureView>& texture,
                       const Vector2F&                    position,
                       const RectangleI&                  sourceRectangle,
                       const ColorF&                      colorMask,
                       Float32                            rotation,
                       const Vector2F&                    origin,
                       const Vector2F&                    scale,
                       SpriteEffectFlags                  spriteEffect,
                       Float32                            layerDepth)
{
    PreDraw(texture);

    auto spriteOrigin = origin;
    spriteOrigin.X *= scale.X;
    spriteOrigin.Y *= scale.Y;

    Vector2F size = {
        sourceRectangle.Width * scale.X,
        sourceRectangle.Height * scale.Y,
    };

    Vector2F texCoordTL = {(Float32)sourceRectangle.X / texture->Description.ViewTexture->Description.Size.X,
                           (Float32)sourceRectangle.Y / texture->Description.ViewTexture->Description.Size.Y};
    Vector2F texCoordBR = {(Float32)(sourceRectangle.X + sourceRectangle.Width) / texture->Description.ViewTexture->Description.Size.X,
                           (Float32)(sourceRectangle.Y + sourceRectangle.Height) / texture->Description.ViewTexture->Description.Size.Y};

    if ((Bool)(spriteEffect & SpriteEffect::FlipVertically))
    {
        auto temp    = texCoordBR.Y;
        texCoordBR.Y = texCoordTL.Y;
        texCoordTL.Y = temp;
    }

    if ((Bool)(spriteEffect & SpriteEffect::FlipHorizontally))
    {
        auto temp    = texCoordBR.X;
        texCoordBR.X = texCoordTL.X;
        texCoordTL.X = temp;
    }

    if (rotation == 0)
    {
        AppendBatch(Vector3F(position.X - spriteOrigin.X, position.Y - spriteOrigin.Y, layerDepth),
                    size,
                    texCoordTL,
                    texCoordBR,
                    colorMask);
    }
    else
    {
        AppendBatch(Vector3F(position.X, position.Y, layerDepth),
                    Vector2F(-spriteOrigin.X, -spriteOrigin.Y),
                    size,
                    std::sin(rotation),
                    std::cos(rotation),
                    texCoordTL,
                    texCoordBR,
                    colorMask);
    }
}

void SpriteBatch::DrawRectangle(const RectangleF& rectangle, const ColorF& color)
{
    return Draw(_whiteTextureView, rectangle, color);
}

void SpriteBatch::DrawString(const SharedPointer<SpriteFont>& spriteFont,
                             StringView<WChar>                text,
                             const Vector2F&                  position,
                             const ColorF&                    colorMask)
{
    PreDraw<false>(spriteFont->GetFontAtlas());

    Vector2F origin = position;

    Vector2F fontOrigin = {origin.X,
                           origin.Y};

    Vector2F currentOriginPosition = fontOrigin;

    Bool firstGlyphOfLine = true;
    auto endGlyph         = spriteFont->GetCharacterGlyphsMap().end();
    auto endRect          = spriteFont->GetCharactersRectangleMap().end();

    for (const auto& character : text)
    {
        if (character == '\r')
            continue;

        if (character == '\n')
        {
            fontOrigin.Y += spriteFont->GetLineHeight();
            currentOriginPosition = fontOrigin;
            firstGlyphOfLine      = true;
            continue;
        }

        auto glyph     = spriteFont->GetCharacterGlyphsMap().Find(character);
        auto rectangle = spriteFont->GetCharactersRectangleMap().Find(character);

        if (glyph == endGlyph && rectangle == endRect)
            continue;

        Vector2F drawingPos = currentOriginPosition;
        Vector2F offset     = {};

        if (!firstGlyphOfLine && glyph->Second.Bearing.X > 0)
            drawingPos.X += (Float32)glyph->Second.Bearing.X;

        drawingPos.Y += (Float32)glyph->Second.Bearing.Y;

        currentOriginPosition.X = drawingPos.X + (Float32)glyph->Second.Advance.X;

        // Draws the character if it is not a space character
        if (rectangle != endRect)
        {
            if (_spriteCount >= _maxSpriteCountsPerBatch)
                Flush();

            _spriteCount++;

            auto texCoordTL = Vector2F(rectangle->Second.X / (Float32)spriteFont->GetFontAtlas()->Description.ViewTexture->Description.Size.X,
                                       rectangle->Second.Y / (Float32)spriteFont->GetFontAtlas()->Description.ViewTexture->Description.Size.Y);

            auto texCoordBR = Vector2F((rectangle->Second.X + rectangle->Second.Width) / (Float32)spriteFont->GetFontAtlas()->Description.ViewTexture->Description.Size.X,
                                       (rectangle->Second.Y + rectangle->Second.Height) / (Float32)spriteFont->GetFontAtlas()->Description.ViewTexture->Description.Size.Y);

            AppendBatch(Vector3F(drawingPos.X, drawingPos.Y, 0.0f),
                        Vector2F(rectangle->Second.Width, rectangle->Second.Height),
                        texCoordTL,
                        texCoordBR,
                        colorMask);
        }

        firstGlyphOfLine = false;
    }
}

void SpriteBatch::End()
{
    if (!_isBegun)
        throw InvalidOperationException("`Begin` must be called before `End`!");

    _isBegun = false;

    Flush();

    _currentTextureView = nullptr;
}

SpriteBatch::Vertex::Vertex(const Vector3F& position,
                            const ColorF&   colorMask,
                            const Vector2F& textureCoordinate) noexcept :
    Position(position),
    ColorMask(colorMask),
    TextureCoordinate(textureCoordinate) {}

Size SpriteBatch::PipelineStateKey::Hasher::operator()(const SpriteBatch::PipelineStateKey& pipelineStateKey) const noexcept
{
    Size hash = 0;

    hash = Math::HashCombine(hash, (Size)pipelineStateKey.Blend.BlendEnable);
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Blend.DestAlphaBlendFactor));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Blend.DestColorBlendFactor));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Blend.SourceAlphaBlendFactor));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Blend.SourceAlphaBlendFactor));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Blend.AlphaOperation));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Blend.ColorOperation));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Blend.WriteChannelFlags));

    hash = Math::HashCombine(hash, (Size)pipelineStateKey.Depth.DepthTestEnable);
    hash = Math::HashCombine(hash, (Size)pipelineStateKey.Depth.DepthWriteEnable);
    hash = Math::HashCombine(hash, (Size)pipelineStateKey.Depth.StencilEnable);
    hash = Math::HashCombine(hash, (Size)pipelineStateKey.Depth.StencilReadMask);
    hash = Math::HashCombine(hash, (Size)pipelineStateKey.Depth.StencilWriteMask);
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Depth.DepthCompareFunction));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Depth.BackFaceStencilOperation.StencilCompareFunction));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Depth.BackFaceStencilOperation.StencilFailOperation));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Depth.BackFaceStencilOperation.StencilPassDepthPassOperation));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Depth.BackFaceStencilOperation.StencilPassDepthFailOperation));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Depth.FrontFaceStencilOperation.StencilCompareFunction));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Depth.FrontFaceStencilOperation.StencilFailOperation));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Depth.FrontFaceStencilOperation.StencilPassDepthPassOperation));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Depth.FrontFaceStencilOperation.StencilPassDepthFailOperation));

    Uint32 depthBiasClampHash       = (*(Uint32*)(&pipelineStateKey.Rasterizer.DepthBiasClamp));
    Uint32 slopeScaledDepthBiasHash = (*(Uint32*)(&pipelineStateKey.Rasterizer.SlopeScaledDepthBias));

    hash = Math::HashCombine(hash, (Size)pipelineStateKey.Rasterizer.DepthClipEnable);
    hash = Math::HashCombine(hash, (Size)pipelineStateKey.Rasterizer.ScissorTestEnable);
    hash = Math::HashCombine(hash, (Size)pipelineStateKey.Rasterizer.DepthBias);
    hash = Math::HashCombine(hash, (Size)depthBiasClampHash);
    hash = Math::HashCombine(hash, (Size)slopeScaledDepthBiasHash);
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Rasterizer.FaceCulling));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Rasterizer.FrontFaceWinding));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(pipelineStateKey.Rasterizer.PrimitiveFillMode));

    return hash;
}

Bool SpriteBatch::PipelineStateKey::operator==(const SpriteBatch::PipelineStateKey& pipelineStateKey) const noexcept
{
    const Bool blendEqual = Blend.BlendEnable == pipelineStateKey.Blend.BlendEnable &&
        Blend.DestAlphaBlendFactor == pipelineStateKey.Blend.DestAlphaBlendFactor &&
        Blend.DestColorBlendFactor == pipelineStateKey.Blend.DestColorBlendFactor &&
        Blend.SourceAlphaBlendFactor == pipelineStateKey.Blend.SourceAlphaBlendFactor &&
        Blend.SourceColorBlendFactor == pipelineStateKey.Blend.SourceColorBlendFactor &&
        Blend.AlphaOperation == pipelineStateKey.Blend.AlphaOperation &&
        Blend.ColorOperation == pipelineStateKey.Blend.ColorOperation &&
        Blend.WriteChannelFlags == pipelineStateKey.Blend.WriteChannelFlags;

    const Bool depthEqual = Depth.DepthTestEnable == pipelineStateKey.Depth.DepthTestEnable &&
        Depth.DepthWriteEnable == pipelineStateKey.Depth.DepthWriteEnable &&
        Depth.StencilEnable == pipelineStateKey.Depth.StencilEnable &&
        Depth.BackFaceStencilOperation.StencilCompareFunction == pipelineStateKey.Depth.BackFaceStencilOperation.StencilCompareFunction &&
        Depth.BackFaceStencilOperation.StencilFailOperation == pipelineStateKey.Depth.BackFaceStencilOperation.StencilFailOperation &&
        Depth.BackFaceStencilOperation.StencilPassDepthPassOperation == pipelineStateKey.Depth.BackFaceStencilOperation.StencilPassDepthPassOperation &&
        Depth.BackFaceStencilOperation.StencilPassDepthFailOperation == pipelineStateKey.Depth.BackFaceStencilOperation.StencilPassDepthFailOperation &&
        Depth.FrontFaceStencilOperation.StencilCompareFunction == pipelineStateKey.Depth.FrontFaceStencilOperation.StencilCompareFunction &&
        Depth.FrontFaceStencilOperation.StencilFailOperation == pipelineStateKey.Depth.FrontFaceStencilOperation.StencilFailOperation &&
        Depth.FrontFaceStencilOperation.StencilPassDepthFailOperation == pipelineStateKey.Depth.FrontFaceStencilOperation.StencilPassDepthFailOperation &&
        Depth.FrontFaceStencilOperation.StencilPassDepthPassOperation == pipelineStateKey.Depth.FrontFaceStencilOperation.StencilPassDepthPassOperation &&
        Depth.DepthCompareFunction == pipelineStateKey.Depth.DepthCompareFunction &&
        Depth.StencilReadMask == pipelineStateKey.Depth.StencilReadMask &&
        Depth.StencilWriteMask == pipelineStateKey.Depth.StencilWriteMask;

    const Bool rasterizerEqual = Rasterizer.DepthClipEnable == pipelineStateKey.Rasterizer.DepthClipEnable &&
        Rasterizer.ScissorTestEnable == pipelineStateKey.Rasterizer.ScissorTestEnable &&
        Rasterizer.DepthBias == pipelineStateKey.Rasterizer.DepthBias &&
        Rasterizer.DepthBiasClamp == pipelineStateKey.Rasterizer.DepthBiasClamp &&
        Rasterizer.SlopeScaledDepthBias == pipelineStateKey.Rasterizer.SlopeScaledDepthBias &&
        Rasterizer.FaceCulling == pipelineStateKey.Rasterizer.FaceCulling &&
        Rasterizer.FrontFaceWinding == pipelineStateKey.Rasterizer.FrontFaceWinding &&
        Rasterizer.PrimitiveFillMode == pipelineStateKey.Rasterizer.PrimitiveFillMode;

    return depthEqual && rasterizerEqual && blendEqual;
}

Size SpriteBatch::SamplerDescriptionHasher::operator()(const SamplerDescription& samplerDescription) const noexcept
{
    Size hash = 0;
    hash      = Math::HashCombine(hash, (Size)samplerDescription.MaxAnisotropyLevel);
    hash      = Math::HashCombine(hash, (Size)samplerDescription.AnisotropyEnable);
    hash      = Math::HashCombine(hash, Enum::GetUnderlyingValue(samplerDescription.AddressModeU));
    hash      = Math::HashCombine(hash, Enum::GetUnderlyingValue(samplerDescription.AddressModeV));
    hash      = Math::HashCombine(hash, Enum::GetUnderlyingValue(samplerDescription.AddressModeW));

    Uint32 colorRHash = (*(Uint32*)(&samplerDescription.BorderColor.R));
    Uint32 colorGHash = (*(Uint32*)(&samplerDescription.BorderColor.G));
    Uint32 colorBHash = (*(Uint32*)(&samplerDescription.BorderColor.B));
    Uint32 colorAHash = (*(Uint32*)(&samplerDescription.BorderColor.A));

    Uint32 loadBias = (*(Uint32*)(&samplerDescription.MipLODBias));
    Uint32 minLoad  = (*(Uint32*)(&samplerDescription.MinLOD));
    Uint32 maxLoad  = (*(Uint32*)(&samplerDescription.MaxLOD));

    hash = Math::HashCombine(hash, colorRHash);
    hash = Math::HashCombine(hash, colorGHash);
    hash = Math::HashCombine(hash, colorBHash);
    hash = Math::HashCombine(hash, colorAHash);

    hash = Math::HashCombine(hash, loadBias);
    hash = Math::HashCombine(hash, minLoad);
    hash = Math::HashCombine(hash, maxLoad);


    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(samplerDescription.MagFilter));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(samplerDescription.MinFilter));
    hash = Math::HashCombine(hash, Enum::GetUnderlyingValue(samplerDescription.MipFilter));

    return hash;
}

Bool SpriteBatch::SamplerDescriptionComparer::operator()(const SamplerDescription& LHS, const SamplerDescription& RHS) const noexcept
{
    return LHS.MaxAnisotropyLevel == RHS.MaxAnisotropyLevel &&
        LHS.AnisotropyEnable == RHS.AnisotropyEnable &&
        LHS.AddressModeU == RHS.AddressModeU &&
        LHS.AddressModeV == RHS.AddressModeV &&
        LHS.AddressModeW == RHS.AddressModeW &&
        LHS.BorderColor.R == RHS.BorderColor.R &&
        LHS.BorderColor.G == RHS.BorderColor.G &&
        LHS.BorderColor.B == RHS.BorderColor.B &&
        LHS.BorderColor.A == RHS.BorderColor.A &&
        LHS.MipLODBias == RHS.MipLODBias &&
        LHS.MinLOD == RHS.MinLOD &&
        LHS.MaxLOD == RHS.MaxLOD &&
        LHS.MagFilter == RHS.MagFilter &&
        LHS.MinFilter == RHS.MinFilter &&
        LHS.MipFilter == RHS.MipFilter;
}

SharedPointer<IGraphicsPipeline> SpriteBatch::GetGraphicsPipeline(const PipelineStateKey& key)
{
    if (!_graphicsDevice || !_swapChain)
        return nullptr;

    auto it = _pipelineCaches.Find(key);

    if (it == _pipelineCaches.end())
    {
        GraphicsPipelineDescription graphicsPipelineDescription = {};
        graphicsPipelineDescription.VertexShader                = _vertexShaderModule;
        graphicsPipelineDescription.FragmentShader              = _fragmentShaderModule;

        graphicsPipelineDescription.VertexBindingDescriptions = List<VertexBindingDescription>(1);
        VertexBindingDescription& vertexBindingDesc           = graphicsPipelineDescription.VertexBindingDescriptions[0];
        vertexBindingDesc.BindingSlot                         = 0;
        vertexBindingDesc.Attributes                          = List<VertexAttribute>(3);

        vertexBindingDesc.Attributes[0].Location = 0;
        vertexBindingDesc.Attributes[0].Type     = ShaderDataType::Float3; // Vertex position

        vertexBindingDesc.Attributes[1].Location = 1;
        vertexBindingDesc.Attributes[1].Type     = ShaderDataType::Float4; // Color blending

        vertexBindingDesc.Attributes[2].Location = 2;
        vertexBindingDesc.Attributes[2].Type     = ShaderDataType::Float2; // UV Coordinate

        graphicsPipelineDescription.Binding = PipelineBinding::Graphics;
        graphicsPipelineDescription.Blend   = {
            {key.Blend},
            false,
            LogicOperation::NoOperation};
        graphicsPipelineDescription.DepthStencil = key.Depth;
        graphicsPipelineDescription.Rasterizer   = key.Rasterizer;

        graphicsPipelineDescription.DepthStencilViewFormat  = _swapChain->Description.DepthStencilFormat;
        graphicsPipelineDescription.RenderTargetViewFormats = {_swapChain->Description.RenderTargetFormat};
        graphicsPipelineDescription.SampleCount             = 1;
        graphicsPipelineDescription.ResourceHeapLayouts     = {_resourceHeapLayout};

        auto graphicsPipeline = _graphicsDevice->CreateGraphicsPipeline(graphicsPipelineDescription);

        _pipelineCaches.Insert({key, graphicsPipeline});

        return graphicsPipeline;
    }

    return it->Second;
}

SharedPointer<ISampler> SpriteBatch::GetSampler(const SamplerDescription& samplerDesc)
{
    if (!_graphicsDevice || !_swapChain)
        return nullptr;

    auto it = _samplerCaches.Find(samplerDesc);

    if (it == _samplerCaches.end())
    {
        auto sampler = _graphicsDevice->CreateSampler(samplerDesc);

        if (!sampler)
            return nullptr;

        _samplerCaches.Insert({samplerDesc, sampler});

        return sampler;
    }

    return it->Second;
}

void SpriteBatch::UpdateTranslationMatrix()
{
    PVoid mappedMemory = _immediateGraphicsDeviceContext->MapBuffer(_currentMatrixTranslation, MapAccess::Write, MapType::Discard);

    auto windowSize = _swapChain->Description.TargetWindow->GetSize();

    FloatMatrix4x4 translationMatrix = {};
    translationMatrix(0, 0)          = 2.0f / (Float32)windowSize.X;
    translationMatrix(1, 1)          = 2.0f / (Float32)windowSize.Y;
    translationMatrix(2, 2)          = 1.0f;
    translationMatrix(0, 3)          = -1.0f;
    translationMatrix(1, 3)          = -1.0f;
    translationMatrix(3, 3)          = 1.0f;

    std::memcpy(mappedMemory, &translationMatrix, sizeof(FloatMatrix4x4));

    _immediateGraphicsDeviceContext->UnmapBuffer(_currentMatrixTranslation);
}

template <Bool IncrementCount>
void SpriteBatch::PreDraw(const SharedPointer<ITextureView>& texture)
{
    // Checks if batch has begun or not
    if (!_isBegun)
        throw InvalidOperationException("`Begin` must be called before drawing.");

    // Requires 2D texture
    if (texture->Description.ViewDimension != TextureViewDimension::Texture2D)
        throw InvalidOperationException("Texture must be 2D!");

    if (_currentTextureView)
    {
        // The texture has changed, flushes the current batches now.
        if (_currentTextureView != texture)
        {
            Flush();

            // Changes the current texture
            _currentTextureView = texture;
        }
    }
    else
        // Assigns new texture
        _currentTextureView = texture;

    // Exceeds batch limit
    if (_spriteCount >= _maxSpriteCountsPerBatch)
        Flush();

    // Increments sprite count
    if constexpr (IncrementCount)
        _spriteCount++;
}

void SpriteBatch::Flush()
{
    if (!_currentTextureView)
        return;

    // Maps vertex buffer memory
    {
        PVoid mappedVertexMemory = _immediateGraphicsDeviceContext->MapBuffer(_vertexBuffer, MapAccess::Write, MapType::Discard);

        std::memcpy(mappedVertexMemory, _vertices.GetData(), _vertices.GetLength() * sizeof(Vertex));

        _immediateGraphicsDeviceContext->UnmapBuffer(_vertexBuffer);
    }

    // Maps index buffer memory
    {
        PVoid mappedIndexMemory = _immediateGraphicsDeviceContext->MapBuffer(_indexBuffer, MapAccess::Write, MapType::Discard);

        std::memcpy(mappedIndexMemory, _indices.GetData(), _indices.GetLength() * sizeof(IndexType));

        _immediateGraphicsDeviceContext->UnmapBuffer(_indexBuffer);
    }

    SharedPointer<ISampler>     samplers[]      = {_currentSampler};
    SharedPointer<ITextureView> textureViews[]  = {_currentTextureView};
    SharedPointer<IBuffer>      vertexBuffers[] = {_vertexBuffer};

    // Binds sampler to the resource heap
    _resourceHeap->BindSamplers(1, samplers, textureViews);

    // Binds graphics pipeline
    _immediateGraphicsDeviceContext->BindPipeline(_currentPipeline);

    // Binds index buffer
    _immediateGraphicsDeviceContext->BindIndexBuffer(_indexBuffer, 0, IndexEnumType);

    // Binds vertex buffer
    _immediateGraphicsDeviceContext->BindVertexBuffers(0, vertexBuffers, nullptr);

    // Binds resource heap
    _immediateGraphicsDeviceContext->BindResourceHeap(_resourceHeap);

    // Issues draw commands
    _immediateGraphicsDeviceContext->DrawIndexed((Uint32)(_spriteCount * 6), 1, 0, 0, 0);

    // Clear all data
    _spriteCount = 0;
    _vertices.Clear();
    _indices.Clear();
}

void SpriteBatch::AppendBatch(const Vector3F& position,
                              const Vector2F& size,
                              const Vector2F& texCoordTL,
                              const Vector2F& texCoordBR,
                              const ColorF&   colorMask)

{
    _vertices.ReserveFor(_vertices.GetLength() + 4);

    // Top left vertex
    _vertices.EmplaceBack(Vector3F(position.X, position.Y, position.Z),
                          colorMask,
                          Vector2F(texCoordTL.X, texCoordTL.Y));

    // Top right vertex
    _vertices.EmplaceBack(Vector3F(position.X + size.X, position.Y, position.Z),
                          colorMask,
                          Vector2F(texCoordBR.X, texCoordTL.Y));

    // Bottom right vertex
    _vertices.EmplaceBack(Vector3F(position.X + size.X, position.Y + size.Y, position.Z),
                          colorMask,
                          Vector2F(texCoordBR.X, texCoordBR.Y));

    // Bottom left vertex
    _vertices.EmplaceBack(Vector3F(position.X, position.Y + size.Y, position.Z),
                          colorMask,
                          Vector2F(texCoordTL.X, texCoordBR.Y));

    try
    {
        _indices.ReserveFor(_indices.GetLength() + 6);
    }
    catch (...)
    {
        // Pops the last vertex group
        _vertices.PopBack();
        _vertices.PopBack();
        _vertices.PopBack();
        _vertices.PopBack();
    }

    // Should not throw the exception here, as the indices are reserved.
    // 6 indices
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 0));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 1));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 2));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 0));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 2));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 3));
}

void SpriteBatch::AppendBatch(const Vector3F& position,
                              const Vector2F& delta,
                              const Vector2F& size,
                              Float32         sin,
                              Float32         cos,
                              const Vector2F& texCoordTL,
                              const Vector2F& texCoordBR,
                              const ColorF&   colorMask)
{
    _vertices.ReserveFor(_vertices.GetLength() + 4);

    // Top left vertex
    _vertices.EmplaceBack(Vector3F(position.X + delta.X * cos - delta.Y * sin, position.Y + delta.X * sin + delta.Y * cos, position.Z),
                          colorMask,
                          Vector2F(texCoordTL.X, texCoordTL.Y));

    // Top right vertex
    _vertices.EmplaceBack(Vector3F(position.X + (delta.X + size.X) * cos - delta.Y * sin, position.Y + (delta.X + size.X) * sin + delta.Y * cos, position.Z),
                          colorMask,
                          Vector2F(texCoordBR.X, texCoordTL.Y));

    // Bottom right vertex
    _vertices.EmplaceBack(Vector3F(position.X + (delta.X + size.X) * cos - (delta.Y + size.Y) * sin, position.Y + (delta.X + size.X) * sin + (delta.Y + size.Y) * cos, position.Z),
                          colorMask,
                          Vector2F(texCoordBR.X, texCoordBR.Y));

    // Bottom left vertex
    _vertices.EmplaceBack(Vector3F(position.X + delta.X * cos - (delta.Y + size.Y) * sin, position.Y + delta.X * sin + (delta.Y + size.Y) * cos, position.Z),
                          colorMask,
                          Vector2F(texCoordTL.X, texCoordBR.Y));

    try
    {
        _indices.ReserveFor(_indices.GetLength() + 6);
    }
    catch (...)
    {
        // Pops the last vertex group
        _vertices.PopBack();
        _vertices.PopBack();
        _vertices.PopBack();
        _vertices.PopBack();
    }

    // Should not throw the exception here, as the indices are reserved.
    // 6 indices
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 0));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 1));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 2));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 0));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 2));
    _indices.EmplaceBack((IndexType)(((_spriteCount - 1) * 4) + 3));
}

} // namespace Axis