/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/Core>
#include <Axis/Graphics>
#include <Axis/System>
#include <Axis/Window>

// GLSL vertex shader code
constexpr const char* VertexShaderCode = R"""(
#version 450
layout(location = 0) in vec3 VertPositionIn;
layout(location = 1) in vec4 VertColorIn;
layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform UniformBufferObject {
    mat4 Mvp;
} Ubo;
void main() 
{
    gl_Position = Ubo.Mvp * vec4(VertPositionIn, 1.0);
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

int main(int argc, char** argv)
{
    // using namespace Axis
    using namespace Axis;

    // Derived application
    class RotatingCube : public Application
    {
    public:
        RotatingCube() noexcept = default;

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

            // Creates graphics pipeline layout for MVP.
            ResourceHeapLayoutDescription resourceHeapLayoutDescription    = {};
            resourceHeapLayoutDescription.ResourceBindings                 = List<ResourceLayoutBinding>(1);
            resourceHeapLayoutDescription.ResourceBindings[0].ArraySize    = 1;
            resourceHeapLayoutDescription.ResourceBindings[0].Binding      = ResourceBinding::UniformBuffer;
            resourceHeapLayoutDescription.ResourceBindings[0].BindingIndex = 0;
            resourceHeapLayoutDescription.ResourceBindings[0].StageFlags   = ShaderStage::Vertex;

            // Creates empty pipeline layout (no resource binding)
            _resourceHeapLayout = GetGraphicsDevice()->CreateResourceHeapLayout(resourceHeapLayoutDescription);

            // Creates resource heap from the pipeline layout.
            _resourceHeap = GetGraphicsDevice()->CreateResourceHeap({_resourceHeapLayout});

            // Creates graphics pipeline.
            GraphicsPipelineDescription graphicsPipelineDescription   = {};
            graphicsPipelineDescription.Binding                       = PipelineBinding::Graphics;
            graphicsPipelineDescription.FragmentShader                = _fragmentShader;
            graphicsPipelineDescription.VertexShader                  = _vertexShader;
            graphicsPipelineDescription.ResourceHeapLayouts           = {_resourceHeapLayout};
            graphicsPipelineDescription.RenderTargetViewFormats       = {GetSwapChain()->Description.RenderTargetFormat}; // Passes swapchain's render target view format.
            graphicsPipelineDescription.DepthStencilViewFormat        = {GetSwapChain()->Description.DepthStencilFormat}; // Passes swapchain's depth stencil view format.
            graphicsPipelineDescription.SampleCount                   = 1;
            graphicsPipelineDescription.VertexBindingDescriptions     = List<VertexBindingDescription>(1);
            graphicsPipelineDescription.Blend.LogicOperationEnable    = false;
            graphicsPipelineDescription.Blend.LogicOp                 = LogicOperation::NoOperation;
            graphicsPipelineDescription.Blend.RenderTargetBlendStates = List<AttachmentBlendState>(1, AttachmentBlendState::GetAlphaBlend());
            graphicsPipelineDescription.Rasterizer                    = RasterizerState::GetCullCounterClockwise();
            graphicsPipelineDescription.DepthStencil                  = DepthStencilState::GetDefault();

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
            static const Vertex vertrices[] = {
                {{-1.0f,-1.0f,-1.0f}, {0.583f, 0.771f, 0.014f, 1.0f}},
                {{-1.0f,-1.0f, 1.0f}, {0.609f, 0.115f, 0.436f, 1.0f}},
                {{-1.0f, 1.0f, 1.0f}, {0.327f, 0.483f, 0.844f, 1.0f}},
                {{ 1.0f, 1.0f,-1.0f}, {0.822f, 0.569f, 0.201f, 1.0f}},
                {{-1.0f,-1.0f,-1.0f}, {0.435f, 0.602f, 0.223f, 1.0f}},
                {{-1.0f, 1.0f,-1.0f}, {0.310f, 0.747f, 0.185f, 1.0f}},
                {{ 1.0f,-1.0f, 1.0f}, {0.597f, 0.770f, 0.761f, 1.0f}},
                {{-1.0f,-1.0f,-1.0f}, {0.559f, 0.436f, 0.730f, 1.0f}},
                {{ 1.0f,-1.0f,-1.0f}, {0.359f, 0.583f, 0.152f, 1.0f}},
                {{ 1.0f, 1.0f,-1.0f}, {0.483f, 0.596f, 0.789f, 1.0f}},
                {{ 1.0f,-1.0f,-1.0f}, {0.559f, 0.861f, 0.639f, 1.0f}},
                {{-1.0f,-1.0f,-1.0f}, {0.195f, 0.548f, 0.859f, 1.0f}},
                {{-1.0f,-1.0f,-1.0f}, {0.014f, 0.184f, 0.576f, 1.0f}},
                {{-1.0f, 1.0f, 1.0f}, {0.771f, 0.328f, 0.970f, 1.0f}},
                {{-1.0f, 1.0f,-1.0f}, {0.406f, 0.615f, 0.116f, 1.0f}},
                {{ 1.0f,-1.0f, 1.0f}, {0.676f, 0.977f, 0.133f, 1.0f}},
                {{-1.0f,-1.0f, 1.0f}, {0.971f, 0.572f, 0.833f, 1.0f}},
                {{-1.0f,-1.0f,-1.0f}, {0.140f, 0.616f, 0.489f, 1.0f}},
                {{-1.0f, 1.0f, 1.0f}, {0.997f, 0.513f, 0.064f, 1.0f}},
                {{-1.0f,-1.0f, 1.0f}, {0.945f, 0.719f, 0.592f, 1.0f}},
                {{ 1.0f,-1.0f, 1.0f}, {0.543f, 0.021f, 0.978f, 1.0f}},
                {{ 1.0f, 1.0f, 1.0f}, {0.279f, 0.317f, 0.505f, 1.0f}},
                {{ 1.0f,-1.0f,-1.0f}, {0.167f, 0.620f, 0.077f, 1.0f}},
                {{ 1.0f, 1.0f,-1.0f}, {0.347f, 0.857f, 0.137f, 1.0f}},
                {{ 1.0f,-1.0f,-1.0f}, {0.055f, 0.953f, 0.042f, 1.0f}},
                {{ 1.0f, 1.0f, 1.0f}, {0.714f, 0.505f, 0.345f, 1.0f}},
                {{ 1.0f,-1.0f, 1.0f}, {0.783f, 0.290f, 0.734f, 1.0f}},
                {{ 1.0f, 1.0f, 1.0f}, {0.722f, 0.645f, 0.174f, 1.0f}},
                {{ 1.0f, 1.0f,-1.0f}, {0.302f, 0.455f, 0.848f, 1.0f}},
                {{-1.0f, 1.0f,-1.0f}, {0.225f, 0.587f, 0.040f, 1.0f}},
                {{ 1.0f, 1.0f, 1.0f}, {0.517f, 0.713f, 0.338f, 1.0f}},
                {{-1.0f, 1.0f,-1.0f}, {0.053f, 0.959f, 0.120f, 1.0f}},
                {{-1.0f, 1.0f, 1.0f}, {0.393f, 0.621f, 0.362f, 1.0f}},
                {{ 1.0f, 1.0f, 1.0f}, {0.673f, 0.211f, 0.457f, 1.0f}},
                {{-1.0f, 1.0f, 1.0f}, {0.820f, 0.883f, 0.371f, 1.0f}},
                {{ 1.0f,-1.0f, 1.0f}, {0.982f, 0.099f, 0.879f, 1.0f}}};

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

            // Creates uniform buffer for our MVP.
            BufferDescription uniformBufferDescription = {};
            uniformBufferDescription.BufferBinding     = BufferBinding::Uniform;
            uniformBufferDescription.BufferSize        = sizeof(FloatMatrix4x4); // Size of our vertex data.
            uniformBufferDescription.Usage             = ResourceUsage::Dynamic; // Dynamic buffer (mappable, slower but still preferably fast).

            // Specifies that our immediate graphics device context can use the buffer.
            uniformBufferDescription.DeviceQueueFamilyMask = Math::AssignBitToPosition(uniformBufferDescription.DeviceQueueFamilyMask, GetImmediateGraphicsContext()->DeviceQueueFamilyIndex, true);

            _uniformBuffer = GetGraphicsDevice()->CreateBuffer(uniformBufferDescription, nullptr);

            SharedPointer<IBuffer> uniformBuffers[] = {_uniformBuffer};

            // Binds our uniform buffer to the resource heap at binding index 0.
            _resourceHeap->BindBuffers(0,
                                       uniformBuffers);
        }

        // Updates loop goes here!
        void Update(const TimePeriod& deltaTime) noexcept override final
        {
            _period += deltaTime;

            // Gets the window size for the aspect ratio.
            auto windowSize = GetWindow()->GetSize();

            // Maps the buffer in discard mode (Discards the old buffer and create new one)
            void* mappedMemory = GetImmediateGraphicsContext()->MapBuffer(_uniformBuffer,
                                                                          MapAccess::Write,
                                                                          MapType::Discard);

            // View to perspective
            FloatMatrix4x4 MVP = FloatMatrix4x4::GetMatrixPerspective((float)Math::ToRadians(45.0f),
                                                                      (windowSize.X / (float)windowSize.Y),
                                                                      0.1f,
                                                                      100.0f);

            // World to view
            MVP *= FloatMatrix4x4::GetMatrixLookAt(Vector3F(4.0f, 3.0f, -3.0f),
                                                   Vector3F(0.0f, 0.0f, -0.0f),
                                                   Vector3F(0.0f, -1.0f, 0.0f));

            // Model to world
            MVP *= FloatMatrix4x4::GetMatrixRotationY((float)_period.GetTotalSeconds() * (float)Math::ToRadians(45.0f));

            // Copies MVP data to mapped memory
            std::memcpy(mappedMemory, &MVP, sizeof(MVP));

            // Unmaps the memory
            GetImmediateGraphicsContext()->UnmapBuffer(_uniformBuffer);
        }

        // Render loop goes here!
        void Render(const TimePeriod& deltaTime) noexcept override final
        {
            // Gets the current swap chain's back buffer. (Color view)
            auto currentRenderTargetBackBuffer = GetSwapChain()->GetCurrentRenderTargetView();
            // Gets the current swap chain's back buffer. (Depth stencil view)
            auto currentDepthStencilBackBuffer = GetSwapChain()->GetCurrentDepthStencilView();

            // The window clear color value (XNA cornflower blue, good old days.....)
            const ColorF clearColor = ColorF(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f);

            // Clears the swapChain's back buffer to the specified color.
            GetImmediateGraphicsContext()->ClearRenderTarget(currentRenderTargetBackBuffer, clearColor);

            // Clears the swapChain's back buffer depth stencil value.
            GetImmediateGraphicsContext()->ClearDepthStencilView(currentDepthStencilBackBuffer, 1, 1.0f, ClearDepthStencil::Depth);

            // Binds the graphics pipeline.
            GetImmediateGraphicsContext()->BindPipeline(_graphicsPipeline);

            // Binds the resource heap
            GetImmediateGraphicsContext()->BindResourceHeap(_resourceHeap);

            SharedPointer<IBuffer> vertexBuffers[] = {_vertexBuffer};
            // Binds the vertex buffer.
            GetImmediateGraphicsContext()->BindVertexBuffers(0,             // Starts at vertex binding 0
                                                             vertexBuffers, // Only one vertex buffer is bound.
                                                             nullptr);      // No offsets

            // Draws the cube.
            GetImmediateGraphicsContext()->Draw(36, 1, 0, 0);
        }

        // Private members
        SharedPointer<IShaderModule>       _vertexShader       = {};
        SharedPointer<IShaderModule>       _fragmentShader     = {};
        SharedPointer<IResourceHeapLayout> _resourceHeapLayout = {};
        SharedPointer<IGraphicsPipeline>   _graphicsPipeline   = {};
        SharedPointer<IResourceHeap>       _resourceHeap       = {};
        SharedPointer<IBuffer>             _vertexBuffer       = {};
        SharedPointer<IBuffer>             _uniformBuffer      = {};
        TimePeriod                         _period             = {};
    };

    RotatingCube application = {};

    application.Run();

    return 0;
}