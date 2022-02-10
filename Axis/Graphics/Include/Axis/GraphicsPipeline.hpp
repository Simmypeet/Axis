/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_GRAPHICSPIPELINE_HPP
#define AXIS_GRAPHICS_GRAPHICSPIPELINE_HPP
#pragma once

#include "Pipeline.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class IShaderModule;
class IRenderPass;

/// \brief Describes the interpretation of the Vertex data in the Vertex shader.
struct VertexAttribute final
{
    /// \brief The location of the attributes in the Vertex shader.
    Uint32 Location = {};

    /// \brief The data type to receive in the specified location.
    ShaderDataType Type = {};
};

/// \brief Describes the vertex input from the VertexBuffer.
struct AXIS_GRAPHICS_API VertexBindingDescription final
{
    /// \brief Describes how the vertex data will be interpreted in the Vertex shader.
    System::List<VertexAttribute> Attributes = {};

    /// \brief The binding slot to receive the data from the VertexBuffer.
    ///        The binding slot number should not exceeded the IGraphicsDevice::Capability::MaxVertexInputBinding.
    Uint32 BindingSlot = {};

    /// \brief Gets the total size (in bytes) of Attribute array.
    ///
    /// \return 0 if the VertexBindingDescription::Attributes array is nullptr, otherwise the actual size of Attributes.
    Size GetStride() const;
};

/// \brief Available operations to execute to the stencil value.
enum class StencilOperation : Uint8
{
    /// \brief Increments the stencil value, if it hits the maximum value, the value is limited at maximum.
    Increment,

    /// \brief Increments the stencil value, if it hits the maximum value, the value is wrapped to the minimum.
    IncrementWrap,

    /// \brief Decrements the stencil value, if it hits the minimum value, the value is limited at minimum.
    Decrement,

    /// \brief Decrements the stencil value, if it hits the minimum value, the value is wrapped to the maximum.
    DecrementWrap,

    /// \brief Keeps the value
    Keep,

    /// \brief Sets the value to the zero.
    Zero,

    /// \brief Replaces the stored value by the new value.
    Replace,

    /// \brief Do bitwise INVERT(~) operation to the value.
    Invert
};

/// \brief An operation to execute upon stencil testing.
struct StencilOperationDescription final
{
    /// \brief Operation to execute when fails stencil test.
    StencilOperation StencilFailOperation = {};

    /// \brief Operation to execute when passes stencil test, but fails depth test.
    StencilOperation StencilPassDepthFailOperation = {};

    /// \brief Operation to execute when passes stencil and depth tests.
    StencilOperation StencilPassDepthPassOperation = {};

    /// \brief Comparison functions used in stencil testing.
    CompareFunction StencilCompareFunction = {};
};

/// \brief Specifies the depth stencil operation in the pipeline.
struct DepthStencilState final
{
    /// \brief If depth test is disable all depth tests will always pass,
    ///        and depth write will be disabled regardless of user provided settings.
    Bool DepthTestEnable = {};

    /// \brief Specifies whether after passed the depth test, should the
    ///        passed depth value be written to the memory.
    Bool DepthWriteEnable = {};

    /// \brief Comparison functions used in depth testing.
    CompareFunction DepthCompareFunction = {};

    /// \brief Specifies whether to enable stencil operation or not.
    Bool StencilEnable = {};

    /// \brief Mask uses with bitwise AND(&) operation when accessing the read stencil value.
    Uint8 StencilReadMask = {};

    /// \brief Mask uses with bitwise AND(&) operation when accessing the write stencil value.
    Uint8 StencilWriteMask = {};

    /// \brief Stencil operation to execute upon stencil testing for front face.
    StencilOperationDescription FrontFaceStencilOperation = {};

    /// \brief Stencil operation to execute upon stencil testing for back face.
    StencilOperationDescription BackFaceStencilOperation = {};

    /// \brief Gets \a `DepthStencilState` for not using a depth stencil buffer.
    constexpr static DepthStencilState GetNone() noexcept;

    /// \brief Gets \a `DepthStencilState` for using a depth stencil buffer.
    constexpr static DepthStencilState GetDefault() noexcept;

    /// \brief Gets \a `DepthStencilState` for enabling a read-only depth stencil buffer.
    constexpr static DepthStencilState GetDepthRead() noexcept;
};

/// \brief Describes the options for filling the vertrices and lines that define a primitive.
enum class FillMode : Uint8
{
    /// \brief Draws solid face for each primitives.
    Solid,

    /// \brief Draws the lines that define a primitive face.
    WiredFrame
};

/// \brief Specifies how to determine the front face of the premitives.
enum class FrontFace : Uint8
{
    /// \brief A primitive which has clockwise winding will be considered as front face.
    Clockwise,

    /// \brief A primitive which has counter clockwise winding will be considered as front face.
    CounterClockwise
};

/// \brief Specifies the primitives culling.
enum class CullMode : Uint8
{
    /// \brief No culling at all
    None = 0,

    /// \brief Culls the primitives which are considered as front face.
    FrontFace = AXIS_BIT(1),

    /// \brief Culls the primitives which are considered as front face.
    BackFace = AXIS_BIT(2)
};

/// \brief Specifies the primitives culling. (Bit mask)
typedef CullMode CullModeFlags;

/// \brief Determines how to convert vector data (shapes) into raster data (pixels).
struct RasterizerState final
{
    /// \brief Specifies how the primitive will be drawn (solid or line).
    FillMode PrimitiveFillMode = {};

    /// \brief Specifies how to determine the front face of the premitives.
    FrontFace FrontFaceWinding = {};

    /// \brief Specifies the condition for culling or removing the primitive.
    CullModeFlags FaceCulling = {};

    /// \brief Indicates whether scissor testing is enabled.
    ///        Culls all pixels that are outside of the active scissor rectangle.
    Bool ScissorTestEnable = {};

    /// \brief The amount of depth value to apply to pixel.
    Int32 DepthBias = {};

    /// \brief Scalar on a given pixel's slope.
    Float32 SlopeScaledDepthBias = {};

    /// \brief Maximum / minimum value of depth bias.
    Float32 DepthBiasClamp = {};

    /// \brief Indicates whether to clips the depth value against near and far clip planes.
    Bool DepthClipEnable = {};

    /// \brief Gets \a `RasterizerState` with settings for culling primitives with clockwise winding order.
    constexpr static RasterizerState GetCullClockwise() noexcept;

    /// \brief Gets \a `RasterizerState` with settings for culling primitives with counter-clockwise winding order.
    constexpr static RasterizerState GetCullCounterClockwise() noexcept;

    /// \brief Gets \a `RasterizerState` with settings for not culling any primitives.
    constexpr static RasterizerState GetCullNone() noexcept;
};

/// \brief Specifies the blending factor in the blending formulars.
enum class BlendFactor : Uint8
{
    /// \brief Constant value: 0
    Zero,

    /// \brief Constant value: 1
    One,

    /// \brief Uses the corresponded source color chanel value as the factor.
    SourceColor,

    /// \brief Uses one and minus with the corresponded source color chanel value as the factor.
    OneMinusSourceColor,

    /// \brief Uses the corresponded destination color chanel value as the factor.
    DestColor,

    /// \brief Uses one and minus with the corresponded destination color chanel value as the factor.
    OneMinusDestColor,

    /// \brief Uses the alpha value in the source color as the factor.
    SourceAlpha,

    /// \brief Uses one and minus with the alpha value in the source color as the factor.
    OneMinusSourceAlpha,

    /// \brief Uses the alpha value in the destination color as the factor.
    DestAlpha,

    /// \brief Uses one and minus with the alpha value in the destination color as the factor.
    OneMinusDestAlpha,

    /// \brief Uses the corresponded constant color chanel value as the factor.
    ConstantColor,

    /// \brief Uses one and minus with the corresponded constant color chanel value as the factor.
    OneMinusConstantColor,

    /// \brief Uses the alpha value in the constant color as the factor.
    ConstantAlpha,

    /// \brief Uses one and minus with the alpha value in the constant color as the factor.
    OneMinusConstantAlpha,

    /// \brief Uses the corresponded second source color chanel value from the as the factor.
    Source1Color,

    /// \brief Uses the second alpha value in the source color as the factor.
    Source1Alpha,

    /// \brief Uses one and minus with the corresponded second source color chanel value as the factor.
    OneMinusSource1Color,

    /// \brief Uses one and minus with the alpha value in the second source color as the factor.
    OneMinusSource1Alpha
};

/// \brief The available equations for blending.
enum class BlendOperation : Uint8
{
    /// \brief The formula: Result = Source * SourceFactor + Dest * DestFactor;
    Add,

    /// \brief The formula: Result = Source * SourceFactor - Dest * DestFactor;
    Subtract,

    /// \brief The formula: Result = Dest * DestFactor - Source * SourceFactor;
    SubtractReverse,

    /// \brief The formula: Result = Min(Source, Dest);
    Min,

    /// \brief The formula: Result = Max(Source, Dest);
    Max
};

/// \brief Logical operations to apply in color blending.
enum class LogicOperation : Uint8
{
    /// \brief Clears the attachment.
    Clear,

    /// \brief Formula: Result = Source & Dest;
    And,

    /// \brief Formula: Result = Source & ~Dest;
    AndReverse,

    /// \brief Formula: Result = Source;
    Copy,

    /// \brief Formula: Result = ~Source & Dest;
    AndInverted,

    /// \brief Formula: Result = Dest;
    NoOperation,

    /// \brief Formula: Result = Source ^ Dest;
    ExclusiveOr,

    /// \brief Formula: Result = Source | Dest;
    Or,

    /// \brief Formula: Result = ~(Source | Dest);
    NOR,

    /// \brief Formula: Result = ~(Source ^ Dest);
    Equivalent,

    /// \brief Formula: Result = ~Dest;
    Invert,

    /// \brief Formula: Result = Source | ~Dest;
    OrReverse,

    /// \brief Formula: Result = ~Source
    CopyInverted,

    /// \brief Formula: Result = ~(Source | Dest)
    OrInverted,

    /// \brief Formula: Result = ~(Source & Dest)
    NAND,

    /// \brief Sets value to 1
    Set
};

/// \brief Availables graphics color channels.
enum class ColorChannel : Uint8
{
    /// \brief Default value, no color channel.
    None = 0,

    /// \brief Red channel
    Red = AXIS_BIT(1),

    /// \brief Green channel
    Green = AXIS_BIT(2),

    /// \brief Blue channel
    Blue = AXIS_BIT(3),

    /// \brief Alpha channel
    Alpha = AXIS_BIT(4)
};

/// \brief Availables graphics color channels. (Bit mask)
typedef ColorChannel ColorChannelFlags;

/// \brief Describes the blending operation for a render target view.
struct AttachmentBlendState
{
    /// \brief Indicates whether to enable blending for this render target.
    Bool BlendEnable = {};

    /// \brief Blend factor for the source in the color channels.
    BlendFactor SourceColorBlendFactor = {};

    /// \brief Blend factor for the destination in the color channels.
    BlendFactor DestColorBlendFactor = {};

    /// \brief Blend factor for the source in the alpha channel.
    BlendFactor SourceAlphaBlendFactor = {};

    /// \brief Blend factor for the destination in the alpha channel.
    BlendFactor DestAlphaBlendFactor = {};

    /// \brief Equation used in color channels blending.
    BlendOperation ColorOperation = {};

    /// \brief Equation used in alpha channel blending.
    BlendOperation AlphaOperation = {};

    /// \brief Specifies which color channels can store the data.
    ColorChannelFlags WriteChannelFlags = {};

    /// \brief Gets \a `AttachmentBlendState` for alpha blend,
    ///        that is blending the source and destination data using alpha.
    constexpr static AttachmentBlendState GetAlphaBlend() noexcept;

    /// \brief Gets \a `AttachmentBlendState` for additive blend,
    ///        which is adding the destination data to the source data without using alpha.
    constexpr static AttachmentBlendState GetAdditiveBlend() noexcept;

    /// \brief Gets \a `AttachmentBlendState` with non-premultipled alpha,
    ///        that is blending source and destination data using alpha while assuming the color data contains no alpha information.
    constexpr static AttachmentBlendState GetNonPremultiplied() noexcept;

    /// \brief Gets \a `AttachmentBlendState` for opaque blend,
    ///        that is overwriting the source with the destination data.
    constexpr static AttachmentBlendState GetOpaque() noexcept;
};

/// \brief Specifies the blending operations for all render targets.
struct BlendState
{
    /// \brief Specifies the blending state for each render target's color attachments.
    System::List<AttachmentBlendState> RenderTargetBlendStates = {};

    /// \brief Indicates whether to enable logic operation for this render target.
    Bool LogicOperationEnable = {};

    /// \brief Specifies the logical operation in color blending if \a Axis::BlendState::LogicOperationEnable is \a `true`.
    LogicOperation LogicOp = {};
};

/// \brief Describes the specification of IGraphicsPipeline resource.
struct GraphicsPipelineDescription final : public BasePipelineDescription
{
    /// \brief The shader that handles the processing of individual vertices fed into the pipeline.
    ///        Vertex shader stage typically performs the transformation on the vertices.
    System::SharedPointer<IShaderModule> VertexShader = {};

    /// \brief (AKA: PixelShader) The shader that handles the color fragments outputted from the rasterization.
    System::SharedPointer<IShaderModule> FragmentShader = {};

    /// \brief Describes the environment that the pipeline will be used in.
    ///        If RenderPass is null, the variable RenderTargetViewFormats and RenderTargetViewFormats are required.
    System::SharedPointer<IRenderPass> RenderPass = {};

    /// \brief The starting index of subpass.
    Uint32 SubpassIndex = {};

    /// \brief Specifies this variable if PipelineDescription::RenderPass is nullptr,
    ///        TextureFormat of render targets also specifies the count of render target attachments.
    System::List<TextureFormat> RenderTargetViewFormats = {};

    /// \brief Specifies this variable if PipelineDescription::RenderPass is nullptr,
    ///        TextureFormat of depth stencil attachment. Specifies TextureFormat::Unkown for no depth stencil attachment.
    TextureFormat DepthStencilViewFormat = {};

    /// \brief Specifies this variable if PipelineDescription::RenderPass is nullptr,
    ///        The number of sampling of the render target view.
    Uint8 SampleCount = {};

    /// \brief The binding type of Pipeline.
    PipelineBinding Binding = {};

    /// \brief Describes the data of VertexInput in each binding.
    ///        The binding number in each description should be unique and should not exceed the GraphicsCapability::MaxVertexInputBinding;
    System::List<VertexBindingDescription> VertexBindingDescriptions = {};

    /// \brief Specifies the depth / stencil test operations.
    DepthStencilState DepthStencil = {};

    /// \brief Specifies the overall operations in converting vertex data to raster data.
    RasterizerState Rasterizer = {};

    /// \brief Specifies the blending operations for all render targets.
    BlendState Blend = {};
};

/// \brief Pipeline for the graphics operations.
class AXIS_GRAPHICS_API IGraphicsPipeline : public IPipeline
{
public:
    /// \brief The description of IGraphicsPipeline resource.
    const GraphicsPipelineDescription Description;

protected:
    /// \brief Constructor
    IGraphicsPipeline(const GraphicsPipelineDescription& description);
};

} // namespace Graphics

} // namespace Axis

#include "../../Private/Axis/GraphicsPipelineImpl.inl"

#endif // AXIS_GRAPHICS_GRAPHICSPIPELINE_HPP