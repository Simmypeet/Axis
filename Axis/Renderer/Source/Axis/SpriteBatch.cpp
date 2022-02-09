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

namespace Renderer
{

SpriteBatch::SpriteBatch(const System::SharedPointer<Graphics::IGraphicsDevice>& graphicsDevice,
                         const System::SharedPointer<Graphics::IDeviceContext>&  immediateGraphicsContext,
                         const System::SharedPointer<Graphics::ISwapChain>&      swapChain,
                         Uint32                                                  maxSpriteCountsPerBatch) :
    _graphicsDevice(graphicsDevice),
    _immediateGraphicsDeviceContext(immediateGraphicsContext),
    _swapChain(swapChain),
    _maxSpriteCountsPerBatch(System::Math::Min(maxSpriteCountsPerBatch, MaximumMaxSpritesPerBatch))
{
    auto resizeEvent = [this](Window::DisplayWindow&, System::Vector2UI) {
        UpdateTranslationMatrix();
    };

    _eventToken = System::EventToken<void(Window::DisplayWindow&, System::Vector2UI)>(swapChain->Description.TargetWindow->GetClientSizeChangedEvent(), resizeEvent);

    // Creates vertex shader
    Graphics::ShaderModuleDescription vertexShaderDescription = {};
    vertexShaderDescription.EntryPoint                        = "main";
    vertexShaderDescription.Language                          = Graphics::ShaderLanguage::GLSL;
    vertexShaderDescription.Stage                             = Graphics::ShaderStage::Vertex;
    _vertexShaderModule                                       = _graphicsDevice->CompileShaderModule(vertexShaderDescription, ::SpriteBatchVertexShaderCode);

    // Creates fragment shader
    Graphics::ShaderModuleDescription fragmentShaderDescription = {};
    fragmentShaderDescription.EntryPoint                        = "main";
    fragmentShaderDescription.Language                          = Graphics::ShaderLanguage::GLSL;
    fragmentShaderDescription.Stage                             = Graphics::ShaderStage::Fragment;
    _fragmentShaderModule                                       = _graphicsDevice->CompileShaderModule(fragmentShaderDescription, ::SpriteBatchFragmentShaderCode);

    Graphics::ResourceHeapLayoutDescription resourceHeapLayoutDescription = {};
    resourceHeapLayoutDescription.ResourceBindings                        = System::List<Graphics::ResourceLayoutBinding>(2);

    // For translation matrix in vertex shader
    auto& vertexTranslationMatrixLayout        = resourceHeapLayoutDescription.ResourceBindings[0];
    vertexTranslationMatrixLayout.ArraySize    = 1;
    vertexTranslationMatrixLayout.Binding      = Graphics::ResourceBinding::UniformBuffer;
    vertexTranslationMatrixLayout.BindingIndex = 0;
    vertexTranslationMatrixLayout.StageFlags   = Graphics::ShaderStage::Vertex;

    // For sampler in fragment shader
    auto& fragmentSamplerLayout        = resourceHeapLayoutDescription.ResourceBindings[1];
    fragmentSamplerLayout.ArraySize    = 1;
    fragmentSamplerLayout.Binding      = Graphics::ResourceBinding::Sampler;
    fragmentSamplerLayout.BindingIndex = 1;
    fragmentSamplerLayout.StageFlags   = Graphics::ShaderStage::Fragment;

    // Creates resource heap layout
    _resourceHeapLayout = _graphicsDevice->CreateResourceHeapLayout(resourceHeapLayoutDescription);

    // Creates resource heap
    _resourceHeap = _graphicsDevice->CreateResourceHeap({_resourceHeapLayout});

    // Creates vertex buffer
    Graphics::BufferDescription vertexBufferDescription = {};
    vertexBufferDescription.BufferBinding               = Graphics::BufferBinding::Vertex;
    vertexBufferDescription.BufferSize                  = maxSpriteCountsPerBatch * sizeof(Vertex) * 4;
    vertexBufferDescription.Usage                       = Graphics::ResourceUsage::Dynamic;
    vertexBufferDescription.DeviceQueueFamilyMask       = System::Math::AssignBitToPosition(vertexBufferDescription.DeviceQueueFamilyMask, _swapChain->Description.ImmediateGraphicsContext->DeviceQueueFamilyIndex, true);
    _vertexBuffer                                       = graphicsDevice->CreateBuffer(vertexBufferDescription, nullptr);

    // Creates index buffer
    Graphics::BufferDescription indexBufferDescription = {};
    indexBufferDescription.BufferBinding               = Graphics::BufferBinding::Index;
    indexBufferDescription.BufferSize                  = maxSpriteCountsPerBatch * sizeof(IndexType) * 6;
    indexBufferDescription.Usage                       = Graphics::ResourceUsage::Dynamic;
    indexBufferDescription.DeviceQueueFamilyMask       = System::Math::AssignBitToPosition(indexBufferDescription.DeviceQueueFamilyMask, _swapChain->Description.ImmediateGraphicsContext->DeviceQueueFamilyIndex, true);
    _indexBuffer                                       = graphicsDevice->CreateBuffer(indexBufferDescription, nullptr);

    // Creates uniform buffer
    Graphics::BufferDescription uniformBufferDescription = {};
    uniformBufferDescription.BufferBinding               = Graphics::BufferBinding::Uniform | Graphics::BufferBinding::TransferDestination;
    uniformBufferDescription.BufferSize                  = sizeof(System::FloatMatrix4x4);
    uniformBufferDescription.Usage                       = Graphics::ResourceUsage::Dynamic;
    uniformBufferDescription.DeviceQueueFamilyMask       = System::Math::AssignBitToPosition(uniformBufferDescription.DeviceQueueFamilyMask, _swapChain->Description.ImmediateGraphicsContext->DeviceQueueFamilyIndex, true);
    _currentMatrixTranslation                            = _graphicsDevice->CreateBuffer(uniformBufferDescription, nullptr);

    UpdateTranslationMatrix();

    System::SharedPointer<Graphics::IBuffer> uniformBuffers[] = {_currentMatrixTranslation};

    _resourceHeap->BindBuffers(0, uniformBuffers);

    _currentPipelineStateKey.Depth      = Graphics::DepthStencilState::GetNone();
    _currentPipelineStateKey.Rasterizer = Graphics::RasterizerState::GetCullNone();
    _currentPipelineStateKey.Blend      = Graphics::AttachmentBlendState::GetAlphaBlend();

    _currentSamplerDescription = Graphics::SamplerDescription::GetPointClamp();

    // Creates white texture
    Graphics::TextureDescription whiteTextureDescription = {
        .Dimension             = Graphics::TextureDimension::Texture2D,
        .Size                  = {1, 1, 1},
        .TextureBinding        = Graphics::TextureBinding::Sampled | Graphics::TextureBinding::TransferDestination,
        .Format                = Graphics::TextureFormat::UnormR8G8B8A8,
        .MipLevels             = 1,
        .Sample                = 1,
        .ArraySize             = 1,
        .Usage                 = Graphics::ResourceUsage::Immutable,
        .DeviceQueueFamilyMask = (Uint64)System::Math::AssignBitToPosition(0, immediateGraphicsContext->DeviceQueueFamilyIndex, true)};

    auto whiteTexture = graphicsDevice->CreateTexture(whiteTextureDescription);

    Graphics::BufferDescription stagingWhiteTextureBufferDescription = {
        .BufferSize            = 4,
        .BufferBinding         = Graphics::BufferBinding::TransferSource,
        .Usage                 = Graphics::ResourceUsage::Dynamic,
        .DeviceQueueFamilyMask = (Uint64)System::Math::AssignBitToPosition(0, immediateGraphicsContext->DeviceQueueFamilyIndex, true)};

    auto stagingWhiteTextureBuffer = graphicsDevice->CreateBuffer(stagingWhiteTextureBufferDescription, nullptr);

    Graphics::ColorUI8 whiteColor = {255, 255, 255, 255};

    auto mappedMemory = _immediateGraphicsDeviceContext->MapBuffer(stagingWhiteTextureBuffer, Graphics::MapAccess::Write, Graphics::MapType::Overwrite);

    std::memcpy(mappedMemory, &whiteColor, sizeof(Graphics::ColorUI8));

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
        throw System::InvalidOperationException("SpriteBatch already begun!");

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

void SpriteBatch::Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
                       const System::Vector2F&                              position,
                       const Graphics::ColorF&                              colorMask)
{
    PreDraw(texture);

    AppendBatch({position.X, position.Y, 0},
                {texture->Description.ViewTexture->Description.Size.X, texture->Description.ViewTexture->Description.Size.Y},
                {0.0f, 0.0f},
                {1.0f, 1.0f},
                colorMask);
}

void SpriteBatch::Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
                       const System::RectangleF&                            destinationRectangle,
                       const Graphics::ColorF&                              colorMask)
{
    PreDraw(texture);

    AppendBatch({destinationRectangle.X, destinationRectangle.Y, 0},
                {destinationRectangle.Width, destinationRectangle.Height},
                {0.0f, 0.0f},
                {1.0f, 1.0f},
                colorMask);
}

void SpriteBatch::Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
                       const System::RectangleF&                            destinationRectangle,
                       const System::RectangleI&                            sourceRectangle,
                       const Graphics::ColorF&                              colorMask)
{
    PreDraw(texture);

    System::Vector2F texCoordTL = {(Float32)sourceRectangle.X / texture->Description.ViewTexture->Description.Size.X,
                                   (Float32)sourceRectangle.Y / texture->Description.ViewTexture->Description.Size.Y};

    System::Vector2F texCoordBR = {texCoordTL.X + ((Float32)sourceRectangle.Width / texture->Description.ViewTexture->Description.Size.X),
                                   texCoordTL.Y + ((Float32)sourceRectangle.Height / texture->Description.ViewTexture->Description.Size.Y)};

    AppendBatch({destinationRectangle.X, destinationRectangle.Y, 0},
                {destinationRectangle.Width, destinationRectangle.Height},
                texCoordTL,
                texCoordBR,
                colorMask);
}

void SpriteBatch::Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
                       const System::Vector2F&                              position,
                       const System::RectangleI&                            sourceRectangle,
                       const Graphics::ColorF&                              colorMask)
{
    PreDraw(texture);

    System::Vector2F texCoordTL = {(Float32)sourceRectangle.X / texture->Description.ViewTexture->Description.Size.X,
                                   (Float32)sourceRectangle.Y / texture->Description.ViewTexture->Description.Size.Y};

    System::Vector2F texCoordBR = {texCoordTL.X + ((Float32)sourceRectangle.Width / texture->Description.ViewTexture->Description.Size.X),
                                   texCoordTL.Y + ((Float32)sourceRectangle.Height / texture->Description.ViewTexture->Description.Size.Y)};

    AppendBatch({position.X, position.Y, 0},
                {sourceRectangle.Width, sourceRectangle.Height},
                texCoordTL,
                texCoordBR,
                colorMask);
}

void SpriteBatch::Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
                       const System::RectangleF&                            destinationRectangle,
                       const System::RectangleI&                            sourceRectangle,
                       const Graphics::ColorF&                              colorMask,
                       Float32                                              rotation,
                       const System::Vector2F&                              origin,
                       SpriteEffectFlags                                    spriteEffect,
                       Float32                                              layerDepth)
{
    PreDraw(texture);

    System::Vector2F spriteOrigin = origin;

    System::Vector2F texCoordTL = {(Float32)sourceRectangle.X / texture->Description.ViewTexture->Description.Size.X,
                                   (Float32)sourceRectangle.Y / texture->Description.ViewTexture->Description.Size.Y};
    System::Vector2F texCoordBR = {(Float32)(sourceRectangle.X + sourceRectangle.Width) / texture->Description.ViewTexture->Description.Size.X,
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
        AppendBatch(System::Vector3F(destinationRectangle.X - spriteOrigin.X, destinationRectangle.Y - spriteOrigin.Y, layerDepth),
                    System::Vector2F(destinationRectangle.Width, destinationRectangle.Height),
                    texCoordTL,
                    texCoordBR,
                    colorMask);
    }
    else
    {
        AppendBatch(System::Vector3F(destinationRectangle.X, destinationRectangle.Y, layerDepth),
                    System::Vector2F(-spriteOrigin.X, -spriteOrigin.Y),
                    System::Vector2F(destinationRectangle.Width, destinationRectangle.Height),
                    std::sin(rotation),
                    std::cos(rotation),
                    texCoordTL,
                    texCoordBR,
                    colorMask);
    }
}

void SpriteBatch::Draw(const System::SharedPointer<Graphics::ITextureView>& texture,
                       const System::Vector2F&                              position,
                       const System::RectangleI&                            sourceRectangle,
                       const Graphics::ColorF&                              colorMask,
                       Float32                                              rotation,
                       const System::Vector2F&                              origin,
                       const System::Vector2F&                              scale,
                       SpriteEffectFlags                                    spriteEffect,
                       Float32                                              layerDepth)
{
    PreDraw(texture);

    auto spriteOrigin = origin;
    spriteOrigin.X *= scale.X;
    spriteOrigin.Y *= scale.Y;

    System::Vector2F size = {
        sourceRectangle.Width * scale.X,
        sourceRectangle.Height * scale.Y,
    };

    System::Vector2F texCoordTL = {(Float32)sourceRectangle.X / texture->Description.ViewTexture->Description.Size.X,
                                   (Float32)sourceRectangle.Y / texture->Description.ViewTexture->Description.Size.Y};
    System::Vector2F texCoordBR = {(Float32)(sourceRectangle.X + sourceRectangle.Width) / texture->Description.ViewTexture->Description.Size.X,
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
        AppendBatch(System::Vector3F(position.X - spriteOrigin.X, position.Y - spriteOrigin.Y, layerDepth),
                    size,
                    texCoordTL,
                    texCoordBR,
                    colorMask);
    }
    else
    {
        AppendBatch(System::Vector3F(position.X, position.Y, layerDepth),
                    System::Vector2F(-spriteOrigin.X, -spriteOrigin.Y),
                    size,
                    std::sin(rotation),
                    std::cos(rotation),
                    texCoordTL,
                    texCoordBR,
                    colorMask);
    }
}

void SpriteBatch::DrawRectangle(const System::RectangleF& rectangle,
                                const Graphics::ColorF&   color)
{
    return Draw(_whiteTextureView, rectangle, color);
}

void SpriteBatch::DrawString(const System::SharedPointer<SpriteFont>& spriteFont,
                             System::StringView<WChar>                text,
                             const System::Vector2F&                  position,
                             const Graphics::ColorF&                  colorMask)
{
    PreDraw<false>(spriteFont->GetFontAtlas());

    System::Vector2F origin = position;

    System::Vector2F fontOrigin = {origin.X,
                                   origin.Y};

    System::Vector2F currentOriginPosition = fontOrigin;

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

        System::Vector2F drawingPos = currentOriginPosition;

        if (!firstGlyphOfLine && glyph->Second.Bearing.X > 0)
            drawingPos.X += (Float32)glyph->Second.Bearing.X;

        drawingPos.Y += spriteFont->GetLineHeight() - (Float32)glyph->Second.Bearing.Y;

        currentOriginPosition.X += (Float32)glyph->Second.Advance.X;

        // Draws the character if it is not a space character
        if (rectangle != endRect)
        {
            if (_spriteCount >= _maxSpriteCountsPerBatch)
                Flush();

            _spriteCount++;

            auto texCoordTL = System::Vector2F(rectangle->Second.X / (Float32)spriteFont->GetFontAtlas()->Description.ViewTexture->Description.Size.X,
                                               rectangle->Second.Y / (Float32)spriteFont->GetFontAtlas()->Description.ViewTexture->Description.Size.Y);

            auto texCoordBR = System::Vector2F((rectangle->Second.X + rectangle->Second.Width) / (Float32)spriteFont->GetFontAtlas()->Description.ViewTexture->Description.Size.X,
                                               (rectangle->Second.Y + rectangle->Second.Height) / (Float32)spriteFont->GetFontAtlas()->Description.ViewTexture->Description.Size.Y);

            AppendBatch(System::Vector3F(drawingPos.X, drawingPos.Y, 0.0f),
                        System::Vector2F(rectangle->Second.Width, rectangle->Second.Height),
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
        throw System::InvalidOperationException("`Begin` must be called before `End`!");

    _isBegun = false;

    Flush();

    _currentTextureView = nullptr;
}

SpriteBatch::Vertex::Vertex(const System::Vector3F& position,
                            const Graphics::ColorF& colorMask,
                            const System::Vector2F& textureCoordinate) noexcept :
    Position(position),
    ColorMask(colorMask),
    TextureCoordinate(textureCoordinate) {}

Size SpriteBatch::PipelineStateKey::Hasher::operator()(const SpriteBatch::PipelineStateKey& pipelineStateKey) const noexcept
{
    Size hash = 0;

    hash = System::Math::HashCombine(hash, (Size)pipelineStateKey.Blend.BlendEnable);
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Blend.DestAlphaBlendFactor));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Blend.DestColorBlendFactor));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Blend.SourceAlphaBlendFactor));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Blend.SourceAlphaBlendFactor));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Blend.AlphaOperation));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Blend.ColorOperation));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Blend.WriteChannelFlags));

    hash = System::Math::HashCombine(hash, (Size)pipelineStateKey.Depth.DepthTestEnable);
    hash = System::Math::HashCombine(hash, (Size)pipelineStateKey.Depth.DepthWriteEnable);
    hash = System::Math::HashCombine(hash, (Size)pipelineStateKey.Depth.StencilEnable);
    hash = System::Math::HashCombine(hash, (Size)pipelineStateKey.Depth.StencilReadMask);
    hash = System::Math::HashCombine(hash, (Size)pipelineStateKey.Depth.StencilWriteMask);
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Depth.DepthCompareFunction));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Depth.BackFaceStencilOperation.StencilCompareFunction));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Depth.BackFaceStencilOperation.StencilFailOperation));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Depth.BackFaceStencilOperation.StencilPassDepthPassOperation));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Depth.BackFaceStencilOperation.StencilPassDepthFailOperation));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Depth.FrontFaceStencilOperation.StencilCompareFunction));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Depth.FrontFaceStencilOperation.StencilFailOperation));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Depth.FrontFaceStencilOperation.StencilPassDepthPassOperation));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Depth.FrontFaceStencilOperation.StencilPassDepthFailOperation));

    Uint32 depthBiasClampHash       = (*(Uint32*)(&pipelineStateKey.Rasterizer.DepthBiasClamp));
    Uint32 slopeScaledDepthBiasHash = (*(Uint32*)(&pipelineStateKey.Rasterizer.SlopeScaledDepthBias));

    hash = System::Math::HashCombine(hash, (Size)pipelineStateKey.Rasterizer.DepthClipEnable);
    hash = System::Math::HashCombine(hash, (Size)pipelineStateKey.Rasterizer.ScissorTestEnable);
    hash = System::Math::HashCombine(hash, (Size)pipelineStateKey.Rasterizer.DepthBias);
    hash = System::Math::HashCombine(hash, (Size)depthBiasClampHash);
    hash = System::Math::HashCombine(hash, (Size)slopeScaledDepthBiasHash);
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Rasterizer.FaceCulling));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Rasterizer.FrontFaceWinding));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(pipelineStateKey.Rasterizer.PrimitiveFillMode));

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

Size SpriteBatch::SamplerDescriptionHasher::operator()(const Graphics::SamplerDescription& samplerDescription) const noexcept
{
    Size hash = 0;
    hash      = System::Math::HashCombine(hash, (Size)samplerDescription.MaxAnisotropyLevel);
    hash      = System::Math::HashCombine(hash, (Size)samplerDescription.AnisotropyEnable);
    hash      = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(samplerDescription.AddressModeU));
    hash      = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(samplerDescription.AddressModeV));
    hash      = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(samplerDescription.AddressModeW));

    Uint32 colorRHash = (*(Uint32*)(&samplerDescription.BorderColor.R));
    Uint32 colorGHash = (*(Uint32*)(&samplerDescription.BorderColor.G));
    Uint32 colorBHash = (*(Uint32*)(&samplerDescription.BorderColor.B));
    Uint32 colorAHash = (*(Uint32*)(&samplerDescription.BorderColor.A));

    Uint32 loadBias = (*(Uint32*)(&samplerDescription.MipLODBias));
    Uint32 minLoad  = (*(Uint32*)(&samplerDescription.MinLOD));
    Uint32 maxLoad  = (*(Uint32*)(&samplerDescription.MaxLOD));

    hash = System::Math::HashCombine(hash, colorRHash);
    hash = System::Math::HashCombine(hash, colorGHash);
    hash = System::Math::HashCombine(hash, colorBHash);
    hash = System::Math::HashCombine(hash, colorAHash);

    hash = System::Math::HashCombine(hash, loadBias);
    hash = System::Math::HashCombine(hash, minLoad);
    hash = System::Math::HashCombine(hash, maxLoad);


    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(samplerDescription.MagFilter));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(samplerDescription.MinFilter));
    hash = System::Math::HashCombine(hash, System::Enum::GetUnderlyingValue(samplerDescription.MipFilter));

    return hash;
}

Bool SpriteBatch::SamplerDescriptionComparer::operator()(const Graphics::SamplerDescription& LHS, const Graphics::SamplerDescription& RHS) const noexcept
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

System::SharedPointer<Graphics::IGraphicsPipeline> SpriteBatch::GetGraphicsPipeline(const PipelineStateKey& key)
{
    if (!_graphicsDevice || !_swapChain)
        return nullptr;

    auto it = _pipelineCaches.Find(key);

    if (it == _pipelineCaches.end())
    {
        Graphics::GraphicsPipelineDescription graphicsPipelineDescription = {};
        graphicsPipelineDescription.VertexShader                          = _vertexShaderModule;
        graphicsPipelineDescription.FragmentShader                        = _fragmentShaderModule;

        graphicsPipelineDescription.VertexBindingDescriptions = System::List<Graphics::VertexBindingDescription>(1);
        Graphics::VertexBindingDescription& vertexBindingDesc = graphicsPipelineDescription.VertexBindingDescriptions[0];
        vertexBindingDesc.BindingSlot                         = 0;
        vertexBindingDesc.Attributes                          = System::List<Graphics::VertexAttribute>(3);

        vertexBindingDesc.Attributes[0].Location = 0;
        vertexBindingDesc.Attributes[0].Type     = Graphics::ShaderDataType::Float3; // Vertex position

        vertexBindingDesc.Attributes[1].Location = 1;
        vertexBindingDesc.Attributes[1].Type     = Graphics::ShaderDataType::Float4; // Color blending

        vertexBindingDesc.Attributes[2].Location = 2;
        vertexBindingDesc.Attributes[2].Type     = Graphics::ShaderDataType::Float2; // UV Coordinate

        graphicsPipelineDescription.Binding = Graphics::PipelineBinding::Graphics;
        graphicsPipelineDescription.Blend   = {
            {key.Blend},
            false,
            Graphics::LogicOperation::NoOperation};
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

System::SharedPointer<Graphics::ISampler> SpriteBatch::GetSampler(const Graphics::SamplerDescription& samplerDesc)
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
    PVoid mappedMemory = _immediateGraphicsDeviceContext->MapBuffer(_currentMatrixTranslation, Graphics::MapAccess::Write, Graphics::MapType::Discard);

    auto windowSize = _swapChain->Description.TargetWindow->GetSize();

    System::FloatMatrix4x4 translationMatrix = {};
    translationMatrix(0, 0)                  = 2.0f / (Float32)windowSize.X;
    translationMatrix(1, 1)                  = 2.0f / (Float32)windowSize.Y;
    translationMatrix(2, 2)                  = 1.0f;
    translationMatrix(0, 3)                  = -1.0f;
    translationMatrix(1, 3)                  = -1.0f;
    translationMatrix(3, 3)                  = 1.0f;

    std::memcpy(mappedMemory, &translationMatrix, sizeof(System::FloatMatrix4x4));

    _immediateGraphicsDeviceContext->UnmapBuffer(_currentMatrixTranslation);
}

template <Bool IncrementCount>
void SpriteBatch::PreDraw(const System::SharedPointer<Graphics::ITextureView>& texture)
{
    // Checks if batch has begun or not
    if (!_isBegun)
        throw System::InvalidOperationException("`Begin` must be called before drawing.");

    // Requires 2D texture
    if (texture->Description.ViewDimension != Graphics::TextureViewDimension::Texture2D)
        throw System::InvalidOperationException("Texture must be 2D!");

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
        PVoid mappedVertexMemory = _immediateGraphicsDeviceContext->MapBuffer(_vertexBuffer,
                                                                              Graphics::MapAccess::Write,
                                                                              Graphics::MapType::Discard);

        std::memcpy(mappedVertexMemory, _vertices.GetData(), _vertices.GetLength() * sizeof(Vertex));

        _immediateGraphicsDeviceContext->UnmapBuffer(_vertexBuffer);
    }

    // Maps index buffer memory
    {
        PVoid mappedIndexMemory = _immediateGraphicsDeviceContext->MapBuffer(_indexBuffer,
                                                                             Graphics::MapAccess::Write,
                                                                             Graphics::MapType::Discard);

        std::memcpy(mappedIndexMemory, _indices.GetData(), _indices.GetLength() * sizeof(IndexType));

        _immediateGraphicsDeviceContext->UnmapBuffer(_indexBuffer);
    }

    System::SharedPointer<Graphics::ISampler>     samplers[]      = {_currentSampler};
    System::SharedPointer<Graphics::ITextureView> textureViews[]  = {_currentTextureView};
    System::SharedPointer<Graphics::IBuffer>      vertexBuffers[] = {_vertexBuffer};

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

void SpriteBatch::AppendBatch(const System::Vector3F& position,
                              const System::Vector2F& size,
                              const System::Vector2F& texCoordTL,
                              const System::Vector2F& texCoordBR,
                              const Graphics::ColorF& colorMask)

{
    _vertices.ReserveFor(_vertices.GetLength() + 4);

    // Top left vertex
    _vertices.EmplaceBack(System::Vector3F(position.X, position.Y, position.Z),
                          colorMask,
                          System::Vector2F(texCoordTL.X, texCoordTL.Y));

    // Top right vertex
    _vertices.EmplaceBack(System::Vector3F(position.X + size.X, position.Y, position.Z),
                          colorMask,
                          System::Vector2F(texCoordBR.X, texCoordTL.Y));

    // Bottom right vertex
    _vertices.EmplaceBack(System::Vector3F(position.X + size.X, position.Y + size.Y, position.Z),
                          colorMask,
                          System::Vector2F(texCoordBR.X, texCoordBR.Y));

    // Bottom left vertex
    _vertices.EmplaceBack(System::Vector3F(position.X, position.Y + size.Y, position.Z),
                          colorMask,
                          System::Vector2F(texCoordTL.X, texCoordBR.Y));

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

void SpriteBatch::AppendBatch(const System::Vector3F& position,
                              const System::Vector2F& delta,
                              const System::Vector2F& size,
                              Float32                 sin,
                              Float32                 cos,
                              const System::Vector2F& texCoordTL,
                              const System::Vector2F& texCoordBR,
                              const Graphics::ColorF& colorMask)
{
    _vertices.ReserveFor(_vertices.GetLength() + 4);

    // Top left vertex
    _vertices.EmplaceBack(System::Vector3F(position.X + delta.X * cos - delta.Y * sin, position.Y + delta.X * sin + delta.Y * cos, position.Z),
                          colorMask,
                          System::Vector2F(texCoordTL.X, texCoordTL.Y));

    // Top right vertex
    _vertices.EmplaceBack(System::Vector3F(position.X + (delta.X + size.X) * cos - delta.Y * sin, position.Y + (delta.X + size.X) * sin + delta.Y * cos, position.Z),
                          colorMask,
                          System::Vector2F(texCoordBR.X, texCoordTL.Y));

    // Bottom right vertex
    _vertices.EmplaceBack(System::Vector3F(position.X + (delta.X + size.X) * cos - (delta.Y + size.Y) * sin, position.Y + (delta.X + size.X) * sin + (delta.Y + size.Y) * cos, position.Z),
                          colorMask,
                          System::Vector2F(texCoordBR.X, texCoordBR.Y));

    // Bottom left vertex
    _vertices.EmplaceBack(System::Vector3F(position.X + delta.X * cos - (delta.Y + size.Y) * sin, position.Y + delta.X * sin + (delta.Y + size.Y) * cos, position.Z),
                          colorMask,
                          System::Vector2F(texCoordTL.X, texCoordBR.Y));

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

} // namespace Renderer

} // namespace Axis
