/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/Core>
#include <Axis/Graphics>
#include <Axis/System>
#include <Axis/Window>
#include <iostream>

using namespace Axis;

// GLSL vertex shader code
constexpr const char* VertexShaderCode = R"""(
#version 450

layout(location = 0) in vec3 VertPositionIn;
layout(location = 1) in vec4 VertColorIn;

layout(location = 0) out vec4 FragColor;

void main() 
{
    gl_Position = vec4(VertPositionIn, 1.0);
    FragColor = VertColorIn;
}
)""";

// GLSL vertex shader code
constexpr const char* FragmentShaderCode = R"""(
#version 450

layout(location = 0) in vec4 FragColorIn;

layout(location = 0) out vec4 OutColor;

void main() 
{
    OutColor = FragColorIn;
}
)""";

int main()
{
    try
    {
        // Derived application
        class HelloTriangle : public Application
        {
        public:
            HelloTriangle() noexcept = default;

        private:
            // Loads graphics content here!
            void LoadContent() noexcept override final
            {
                // Compiles vertex shader source code.
                ShaderModuleDescription vertexShaderDescription = {};
                vertexShaderDescription.EntryPoint              = "main";
                vertexShaderDescription.Language                = ShaderLanguage::GLSL;
                vertexShaderDescription.Stage                   = ShaderStage::Vertex;

                _vertexShader = GetGraphicsDevice()->CompileShaderModule(vertexShaderDescription, VertexShaderCode);

                // Compiles fragment shader source code.
                ShaderModuleDescription fragmentShaderDescription = {};
                fragmentShaderDescription.EntryPoint              = "main";
                fragmentShaderDescription.Language                = ShaderLanguage::GLSL;
                fragmentShaderDescription.Stage                   = ShaderStage::Fragment;

                _fragmentShader = GetGraphicsDevice()->CompileShaderModule(fragmentShaderDescription, FragmentShaderCode);

                // Creates graphics pipeline.
                GraphicsPipelineDescription graphicsPipelineDescription   = {};
                graphicsPipelineDescription.Binding                       = PipelineBinding::Graphics;
                graphicsPipelineDescription.DepthStencil.DepthTestEnable  = false; // Disables depth testing
                graphicsPipelineDescription.DepthStencil.StencilEnable    = false; // Disables stencil testing
                graphicsPipelineDescription.FragmentShader                = _fragmentShader;
                graphicsPipelineDescription.VertexShader                  = _vertexShader;
                graphicsPipelineDescription.RenderTargetViewFormats       = {GetSwapChain()->Description.RenderTargetFormat}; // Passes swapchain's render target view format.
                graphicsPipelineDescription.DepthStencilViewFormat        = {GetSwapChain()->Description.DepthStencilFormat}; // Passes swapchain's depth stencil view format.
                graphicsPipelineDescription.SampleCount                   = 1;
                graphicsPipelineDescription.VertexBindingDescriptions     = List<VertexBindingDescription>(1); // Only 1 vertex binding slot.
                graphicsPipelineDescription.Blend.LogicOperationEnable    = false;
                graphicsPipelineDescription.Blend.LogicOp                 = LogicOperation::NoOperation;
                graphicsPipelineDescription.Blend.RenderTargetBlendStates = List<AttachmentBlendState>(1, AttachmentBlendState::GetAlphaBlend());
                graphicsPipelineDescription.Rasterizer                    = RasterizerState::GetCullNone();
                graphicsPipelineDescription.DepthStencil                  = DepthStencilState::GetNone();

                auto& vertexBindingDescription = graphicsPipelineDescription.VertexBindingDescriptions[0];

                // Specifies the vertex inputs
                vertexBindingDescription.BindingSlot = 0;
                vertexBindingDescription.Attributes  = List<VertexAttribute>(2);

                // Specifies vertex position attribute
                vertexBindingDescription.Attributes[0].Location = 0;
                vertexBindingDescription.Attributes[0].Type     = ShaderDataType::Float3; // Vector3F equivalent

                // Specifies vertex color attribute
                vertexBindingDescription.Attributes[1].Location = 1;
                vertexBindingDescription.Attributes[1].Type     = ShaderDataType::Float4; // ColorF equivalent

                _graphicsPipeline = GetGraphicsDevice()->CreateGraphicsPipeline(graphicsPipelineDescription);

                /// Vertex data structure.
                struct Vertex
                {
                    // Defines vertex position
                    Vector3F Position; // vec3 equivalent in GLSL

                    // Defines vertex color
                    ColorF Color; // vec4 equivalent in GLSL
                };

                // clang-format off

                // Vertex data
                Vertex vertrices[] = {
                    {{ 0.0f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
                    {{ 0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
                    {{-0.5f,  0.5f,  0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
                };

                // clang-format on

                // Creates vertex buffer which contains out vertex data.
                BufferDescription vertexBufferDescription = {};
                vertexBufferDescription.BufferBinding     = BufferBinding::Vertex;
                vertexBufferDescription.BufferSize        = sizeof(vertrices);        // Size of our vertex data.
                vertexBufferDescription.Usage             = ResourceUsage::Immutable; // Immutable buffer (GPU local memory, fastest).

                // Specifies that our immediate graphics device context can use the buffer.
                vertexBufferDescription.DeviceQueueFamilyMask = Math::AssignBitToPosition(vertexBufferDescription.DeviceQueueFamilyMask, GetImmediateGraphicsContext()->DeviceQueueFamilyIndex, true);

                // Specifies the data to initialize into our immutable vertex buffer.
                BufferInitialData vertexBufferData = {};
                vertexBufferData.Data              = vertrices;
                vertexBufferData.DataSize          = sizeof(vertrices);
                vertexBufferData.ImmediateContext  = GetImmediateGraphicsContext();
                vertexBufferData.Offset            = 0;

                _vertexBuffer = GetGraphicsDevice()->CreateBuffer(vertexBufferDescription, &vertexBufferData);
            }

            // Updates loop goes here!
            void Update(const TimePeriod& deltaTime) noexcept override final {}

            // Render loop goes here!
            void Render(const TimePeriod& deltaTime) noexcept override final
            {
                // The window clear color value (XNA cornflower blue, good old days.....)
                const ColorF clearColor = ColorF(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f);

                // Gets the current swap chain's back buffer. (Color view)
                auto currentBackBuffer = GetSwapChain()->GetCurrentRenderTargetView();

                // Clears the swapChain's back buffer to the specified color.
                GetImmediateGraphicsContext()->ClearRenderTarget(currentBackBuffer, clearColor);

                // Binds the graphics pipeline.
                GetImmediateGraphicsContext()->BindPipeline(_graphicsPipeline);

                // Binds the vertex buffer.
                SharedPointer<IBuffer> vertexBuffers[] = {_vertexBuffer};
                GetImmediateGraphicsContext()->BindVertexBuffers(0,             // Starts at vertex binding 0
                                                                 vertexBuffers, // Only one vertex buffer is bound.
                                                                 nullptr);      // No offsets

                // Draws the triangle.
                GetImmediateGraphicsContext()->Draw(3, 1, 0, 0);
            }

            // Private members
            SharedPointer<IShaderModule>     _vertexShader     = {};
            SharedPointer<IShaderModule>     _fragmentShader   = {};
            SharedPointer<IGraphicsPipeline> _graphicsPipeline = {};
            SharedPointer<IBuffer>           _vertexBuffer     = {};
        };

        HelloTriangle application = {};

        application.Run();
    }
    catch (...)
    {
    }

    return 0;
}
