/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/Core>
#include <Axis/Graphics>
#include <Axis/System>
#include <Axis/Window>
#include <optional>

using namespace Axis;
using namespace Axis::System;
using namespace Axis::Window;
using namespace Axis::Graphics;
using namespace Axis::Core;

// GLSL vertex shader code
constexpr const char* VertexShaderCode = R"""(
    #version 450

    layout(location = 0) in vec3 VertPositionIn;
    layout(location = 1) in vec4 VertColorIn;
    layout(location = 2) in vec2 VertTexCoord;

    layout(location = 0) out vec4 FragColor;
    layout(location = 1) out vec2 FragTexCoord;

    layout(binding = 0) uniform UniformBufferObject {
        mat4 Mvp;
    } Ubo;

    void main() 
    {
        gl_Position = Ubo.Mvp * vec4(VertPositionIn, 1.0);
        FragColor = VertColorIn;
        FragTexCoord = VertTexCoord;
    }
    )""";

// GLSL vertex shader code
constexpr const char* FragmentShaderCode = R"""(
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

int main(int argc, char** argv)
{
    // Derived application
    class Texturing : public Application
    {
    public:
        Texturing() noexcept = default;

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
            ResourceHeapLayoutDescription resourceHeapLayoutDescription = {};
            resourceHeapLayoutDescription.ResourceBindings              = List<ResourceLayoutBinding>(2);

            resourceHeapLayoutDescription.ResourceBindings[0].ArraySize    = 1;
            resourceHeapLayoutDescription.ResourceBindings[0].Binding      = ResourceBinding::UniformBuffer;
            resourceHeapLayoutDescription.ResourceBindings[0].BindingIndex = 0;
            resourceHeapLayoutDescription.ResourceBindings[0].StageFlags   = ShaderStage::Vertex;

            resourceHeapLayoutDescription.ResourceBindings[1].ArraySize    = 1;
            resourceHeapLayoutDescription.ResourceBindings[1].Binding      = ResourceBinding::Sampler;
            resourceHeapLayoutDescription.ResourceBindings[1].BindingIndex = 1;
            resourceHeapLayoutDescription.ResourceBindings[1].StageFlags   = ShaderStage::Fragment;

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
            vertexBindingDescription.Attributes  = List<VertexAttribute>(3);

            // Specifies vertex position attribute
            vertexBindingDescription.Attributes[0].Location = 0;
            vertexBindingDescription.Attributes[0].Type     = ShaderDataType::Float3; // Vector3F equivalent

            // Specifies vertex color attribute
            vertexBindingDescription.Attributes[1].Location = 1;
            vertexBindingDescription.Attributes[1].Type     = ShaderDataType::Float4; // ColorF equivalent

            // Specifies texture coordinate attribute
            vertexBindingDescription.Attributes[2].Location = 2;
            vertexBindingDescription.Attributes[2].Type     = ShaderDataType::Float2; // Vector2F equivalent

            _graphicsPipeline = GetGraphicsDevice()->CreateGraphicsPipeline(graphicsPipelineDescription);

            /// Vertex data structure.
            struct Vertex
            {
                // Defines vertex position
                Vector3F Position; // vec3 equivalent in GLSL

                // Defines vertex color
                ColorF Color; // vec4 equivalent in GLSL

                // Defines texture coordinate
                Vector2F TexCoord; // vec2 equivalent in GLSL
            };

            // Vertex data

            // clang-format off

            static const Vertex vertrices[] = {
                {{-1.0f,-1.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f,1.0f}},
                {{-1.0f,+1.0f,-1.0f}, {1.0f, 0.5f, 0.5f, 1.0f}, {0.0f,0.0f}},
                {{+1.0f,+1.0f,-1.0f}, {0.5f, 1.0f, 0.0f, 1.0f}, {1.0f,0.0f}},
                {{+1.0f,-1.0f,-1.0f}, {0.5f, 0.5f, 1.0f, 1.0f}, {1.0f,1.0f}},

                {{-1.0f,-1.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f,1.0f}},
                {{-1.0f,-1.0f,+1.0f}, {1.0f, 0.5f, 0.5f, 1.0f}, {0.0f,0.0f}},
                {{+1.0f,-1.0f,+1.0f}, {0.5f, 1.0f, 0.0f, 1.0f}, {1.0f,0.0f}},
                {{+1.0f,-1.0f,-1.0f}, {0.5f, 0.5f, 1.0f, 1.0f}, {1.0f,1.0f}},

                {{+1.0f,-1.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f,1.0f}},
                {{+1.0f,-1.0f,+1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f,1.0f}},
                {{+1.0f,+1.0f,+1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f,0.0f}},
                {{+1.0f,+1.0f,-1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f,0.0f}},

                {{+1.0f,+1.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f,1.0f}},
                {{+1.0f,+1.0f,+1.0f}, {1.0f, 0.5f, 0.5f, 1.0f}, {0.0f,0.0f}},
                {{-1.0f,+1.0f,+1.0f}, {0.5f, 1.0f, 0.0f, 1.0f}, {1.0f,0.0f}},
                {{-1.0f,+1.0f,-1.0f}, {0.5f, 0.5f, 1.0f, 1.0f}, {1.0f,1.0f}},

                {{-1.0f,+1.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f,0.0f}},
                {{-1.0f,+1.0f,+1.0f}, {1.0f, 0.5f, 0.5f, 1.0f}, {0.0f,0.0f}},
                {{-1.0f,-1.0f,+1.0f}, {0.5f, 1.0f, 0.0f, 1.0f}, {0.0f,1.0f}},
                {{-1.0f,-1.0f,-1.0f}, {0.5f, 0.5f, 1.0f, 1.0f}, {1.0f,1.0f}},

                {{-1.0f,-1.0f,+1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f,1.0f}},
                {{+1.0f,-1.0f,+1.0f}, {1.0f, 0.5f, 0.5f, 1.0f}, {0.0f,1.0f}},
                {{+1.0f,+1.0f,+1.0f}, {0.5f, 1.0f, 0.0f, 1.0f}, {0.0f,0.0f}},
                {{-1.0f,+1.0f,+1.0f}, {0.5f, 0.5f, 1.0f, 1.0f}, {1.0f,0.0f}}
            };

            // clang-format on

            // Creates vertex buffer which contains our vertex data.
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

            // clang-format off
            
            static const Uint16 indices[] = {
                2,  0,  1,  2,  3,  0,
                4,  6,  5,  4,  7,  6,
                8,  10, 9,  8,  11, 10,
                12, 14, 13, 12, 15, 14,
                16, 18, 17, 16, 19, 18,
                20, 21, 22, 20, 22, 23
            };

            // clang-format on

            // Creates index buffer which contains our indices.
            BufferDescription indexBufferDescription = {};
            indexBufferDescription.BufferBinding     = BufferBinding::Index;
            indexBufferDescription.BufferSize        = sizeof(indices);          // Size of our index data.
            indexBufferDescription.Usage             = ResourceUsage::Immutable; // Immutable buffer (GPU local memory, fastest).

            // Specifies that our immediate graphics device context can use the buffer.
            indexBufferDescription.DeviceQueueFamilyMask = Math::AssignBitToPosition(indexBufferDescription.DeviceQueueFamilyMask, GetImmediateGraphicsContext()->DeviceQueueFamilyIndex, true);

            // Specifies the data to initialize into our immutable index buffer.
            BufferInitialData indexBufferData = {};
            indexBufferData.Data              = indices;
            indexBufferData.DataSize          = sizeof(indices);
            indexBufferData.ImmediateContext  = GetImmediateGraphicsContext();
            indexBufferData.Offset            = 0;

            _indexBuffer = GetGraphicsDevice()->CreateBuffer(indexBufferDescription, &indexBufferData);

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

            WString executableDirectoryPath = Path::GetExecutableDirectoryPath();
            WString paths[]                 = {executableDirectoryPath, L"Asset\\CPPLogo.png"};

            // Path to the asset file
            WString assetPath = Path::CombinePath(paths);

            TextureLoadConfiguration loadConfiguration = {};
            loadConfiguration.Binding                  = TextureBinding::Sampled;
            loadConfiguration.GraphicsDevice           = GetGraphicsDevice();
            loadConfiguration.ImmediateDeviceContext   = GetImmediateGraphicsContext();
            loadConfiguration.IsSRGB                   = false;
            loadConfiguration.GenerateMip              = true;
            loadConfiguration.Usage                    = ResourceUsage::Immutable;

            TextureLoader loader = TextureLoader(FileStream(assetPath, FileMode::Read | FileMode::Binary),
                                                 loadConfiguration);

            // Creates the texture from the loaded image.
            _loadedImage = loader.CreateTexture();

            // Creates texture view for the loaded image.
            _loadedImageView = _loadedImage->CreateDefaultTextureView();

            // Creates sampler object for shader.
            _sampler = GetGraphicsDevice()->CreateSampler(SamplerDescription::GetLinearClamp());

            SharedPointer<ISampler>     samplers[]     = {_sampler};
            SharedPointer<ITextureView> textureViews[] = {_loadedImageView};

            _resourceHeap->BindSamplers(1,
                                        samplers,
                                        textureViews);
        }

        // Updates loop goes here!
        void Update(const TimePeriod& deltaTime) noexcept override final
        {
            // If users presses escape, the application will exit.
            if (Keyboard::GetKeyboardState().IsKeyDown(Key::Escape))
                Exit();

            _period += deltaTime;

            // Gets the window size for the aspect ratio.
            auto windowSize = GetWindow()->GetSize();

            // Maps the buffer in discard mode (Discards the old buffer and create new one)
            PVoid mappedMemory = GetImmediateGraphicsContext()->MapBuffer(_uniformBuffer,
                                                                          MapAccess::Write,
                                                                          MapType::Discard);

            // View to perspective
            FloatMatrix4x4 MVP = FloatMatrix4x4::GetMatrixPerspective((Float32)Math::ToRadians(45.0f),
                                                                      (windowSize.X / (Float32)windowSize.Y),
                                                                      0.1f,
                                                                      100.0f);

            // World to view
            MVP *= FloatMatrix4x4::GetMatrixLookAt(Vector3F(4.0f, 3.0f, -3.0f),
                                                   Vector3F(0.0f, 0.0f, -0.0f),
                                                   Vector3F(0.0f, -1.0f, 0.0f));

            // Model to world
            MVP *= FloatMatrix4x4::GetMatrixRotationY((Float32)_period.GetTotalSeconds() * (Float32)Math::ToRadians(45.0f));

            // Copies MVP data to mapped memory
            std::memcpy(mappedMemory, &MVP, sizeof(MVP));

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

            // Binds the index buffer.
            GetImmediateGraphicsContext()->BindIndexBuffer(_indexBuffer, 0, IndexType::Uint16);

            // Draws the cube.
            GetImmediateGraphicsContext()->DrawIndexed(36, 1, 0, 0, 0);
        }

        // Private members
        SharedPointer<IShaderModule>       _vertexShader       = {};
        SharedPointer<IShaderModule>       _fragmentShader     = {};
        SharedPointer<IResourceHeapLayout> _resourceHeapLayout = {};
        SharedPointer<IGraphicsPipeline>   _graphicsPipeline   = {};
        SharedPointer<IResourceHeap>       _resourceHeap       = {};
        SharedPointer<IBuffer>             _vertexBuffer       = {};
        SharedPointer<IBuffer>             _uniformBuffer      = {};
        SharedPointer<IBuffer>             _indexBuffer        = {};
        SharedPointer<ITexture>            _loadedImage        = {};
        SharedPointer<ITextureView>        _loadedImageView    = {};
        SharedPointer<ISampler>            _sampler            = {};
        TimePeriod                         _period             = {};
    };

    Texturing application = {};

    application.Run();

    return 0;
}
