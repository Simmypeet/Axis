/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/CorePch.hpp>

#include <Axis/Application.hpp>
#include <Axis/Assembly.hpp>
#include <Axis/DeviceContext.hpp>
#include <Axis/DisplayWindow.hpp>
#include <Axis/GraphicsDevice.hpp>
#include <Axis/GraphicsSystem.hpp>
#include <Axis/StaticArray.hpp>
#include <Axis/System.hpp>
#include <Axis/Timer.hpp>

namespace Axis
{

namespace Core
{

// Default constructor
Application::Application() noexcept :
    Components(*this) {}

// Default Application::CreateWindow
System::SharedPointer<Window::DisplayWindow> Application::CreateWindow() const
{
    // Creates default window
    Window::WindowDescription windowDescription = {};
    windowDescription.WindowPosition            = {Window::WindowDescription::UndefinedPosition, Window::WindowDescription::UndefinedPosition};
    windowDescription.WindowSize                = {800, 480};
    windowDescription.WindowStyle               = Window::WindowStyle::TitleBar | Window::WindowStyle::Resizeable | Window::WindowStyle::CloseButton;
    windowDescription.Title                     = L"Axis";

    return Axis::System::MakeShared<Window::DisplayWindow>(windowDescription);
}

static System::Pair<System::SharedPointer<Graphics::IGraphicsSystem>, System::SharedPointer<System::Assembly>> LoadGraphicsSystem(Graphics::GraphicsAPI graphicsApi)
{
#ifdef AXIS_PLATFORM_WINDOWS
    const wchar_t* libName = nullptr;

    switch (graphicsApi)
    {
        case Graphics::GraphicsAPI::Vulkan:
            libName = L"Axis-GraphicsVulkan.dll";
            break;
        default:
            AXIS_ASSERT(false, "Unsupported graphics API");
    }
#endif

    System::SharedPointer<System::Assembly> loadedDyLib = Axis::System::MakeShared<System::Assembly>(libName);

    const Char* symbolName = nullptr;

    switch (graphicsApi)
    {
        case Graphics::GraphicsAPI::Vulkan:
            symbolName = "AxisCreateVulkanGraphicsSystem";
            break;

        default:
            AXIS_ASSERT(false, "Unsupported graphics API");
    }

    auto createGraphicsSystemPFN = (Axis::Graphics::IGraphicsSystem * (*)(void)) loadedDyLib->LoadSymbol(symbolName);

    System::SharedPointer<Graphics::IGraphicsSystem> graphicsSystem = System::SharedPointer<Graphics::IGraphicsSystem>(createGraphicsSystemPFN());

    return {std::move(graphicsSystem), std::move(loadedDyLib)};
}

// Default Application::CreateGraphicsSystem
System::Pair<System::SharedPointer<Graphics::IGraphicsSystem>, System::SharedPointer<System::Assembly>> Application::CreateGraphicsSystem() const
{
    // Axis framework loads the graphics system dynamically via dynamic library
#ifdef AXIS_PLATFORM_WINDOWS
    System::StaticArray<Graphics::GraphicsAPI, 1> preferredGraphicsApis = {Graphics::GraphicsAPI::Vulkan};
#endif

    for (auto graphicsApi : preferredGraphicsApis)
    {
        auto graphicsSystem = LoadGraphicsSystem(graphicsApi);

        if (graphicsSystem.First && graphicsSystem.Second)
        {
            return graphicsSystem;
        }
    }

    throw System::Exception("Failed to load graphics system");
}

// Default Application::CreateGraphicsDeviceAndContexts
System::Pair<System::SharedPointer<Graphics::IGraphicsDevice>, System::List<System::SharedPointer<Graphics::IDeviceContext>>> Application::CreateGraphicsDeviceAndContexts(const System::SharedPointer<Graphics::IGraphicsSystem>& graphicsSystem) const
{
    auto graphicsAdapters = graphicsSystem->GetGraphicsAdapters();

    auto GetBestGraphicsAdapterIndex = [&](const System::SharedPointer<Graphics::IGraphicsSystem>& graphicsSystem) {
        System::List<Uint32> adapterIndexRatingPair;
        adapterIndexRatingPair.ReserveFor(graphicsAdapters.GetLength());

        for (const auto& adapter : graphicsAdapters)
        {
            // Rates the graphics adapter by its properties and capabilities.
            constexpr const auto RateGraphicsAdapter = [](const Graphics::GraphicsAdapter& adapter) {
                Uint32 rating = 0;

                switch (adapter.AdapterType)
                {
                    case Graphics::GraphicsAdapterType::Dedicated:
                        rating += 1000;
                        break;

                    case Graphics::GraphicsAdapterType::Integrated:
                        rating += 500;
                        break;

                    case Graphics::GraphicsAdapterType::Virtual:
                    case Graphics::GraphicsAdapterType::CPU:
                        rating += 250;
                        break;

                    default:
                        AXIS_ASSERT(false, "GraphicsAdapterType is Unkown?");
                }

                rating += adapter.Capability.MaxTexture2DSize / 100;

                rating += adapter.Capability.MaxVertexInputBinding * 20;

                rating += (Uint32)(adapter.Capability.MaxFramebufferDimension.GetMagnitude() / 100);

                Bool foundGraphics = false;

                for (const auto& deviceQueueFamily : adapter.DeviceQueueFamilies)
                {
                    Uint32 multiplier = 0;

                    auto queueOperationFlag = System::Enum::GetUnderlyingValue(deviceQueueFamily.QueueType);

                    while (queueOperationFlag)
                    {
                        Graphics::QueueOperation currentFlag = (Graphics::QueueOperation)System::Math::GetLeastSignificantBit(queueOperationFlag);

                        switch (currentFlag)
                        {
                            case Graphics::QueueOperation::Compute:
                                multiplier += 1;
                                break;

                            case Graphics::QueueOperation::Transfer:
                                multiplier += 2;
                                break;

                            case Graphics::QueueOperation::Graphics:
                                multiplier += 3;
                                foundGraphics = true;
                                break;

                            default:
                                AXIS_ASSERT(false, "QueueOperationFlag is Unkown?");
                        }

                        queueOperationFlag &= ~System::Enum::GetUnderlyingValue(currentFlag);
                    }

                    rating += multiplier * 5 * deviceQueueFamily.QueueCount;
                }

                // Needs to support graphics operation!
                if (!foundGraphics)
                    return Uint32(0);

                return rating;
            };

            // Rates the graphics adapter, finds the best one.
            adapterIndexRatingPair.Append(RateGraphicsAdapter(adapter));
        }

        Uint32 currentMaxScore      = 0;
        Uint32 currentChoosingIndex = 0;

        Uint32 index = 0;

        for (const auto& rating : adapterIndexRatingPair)
        {
            if (currentMaxScore > rating)
            {
                currentChoosingIndex = index;
                currentMaxScore      = rating;
            }
            index++;
        }

        return currentChoosingIndex;
    };

    Uint32                               currentChoosingIndex                  = GetBestGraphicsAdapterIndex(graphicsSystem);
    Graphics::ImmediateContextCreateInfo graphicsImmediateContextCreateInfo[1] = {};

    const Graphics::GraphicsAdapter& choosenGraphicsAdapter = graphicsAdapters[currentChoosingIndex];

    Size index = 0;

    for (const auto& deviceQueueFamily : choosenGraphicsAdapter.DeviceQueueFamilies)
    {
        if (System::Enum::GetUnderlyingValue(deviceQueueFamily.QueueType & Graphics::QueueOperation::Graphics))
        {
            graphicsImmediateContextCreateInfo[0].DeviceQueueFamilyIndex = (Uint32)index;
            break;
        }
        index++;
    }

    System::Span<Graphics::ImmediateContextCreateInfo> graphicsImmediateContextCreateInfoSpan = graphicsImmediateContextCreateInfo;

    // Creates graphics device and immediate graphics context.
    auto result = graphicsSystem->CreateGraphicsDeviceAndContexts(currentChoosingIndex,
                                                                  graphicsImmediateContextCreateInfoSpan);

    return result;
}

// Default Application::CreateSwapChain
System::SharedPointer<Graphics::ISwapChain> Application::CreateSwapChain(const System::SharedPointer<Graphics::IGraphicsDevice>&              graphicsDevice,
                                                                         const System::List<System::SharedPointer<Graphics::IDeviceContext>>& availableDeviceContexts,
                                                                         const System::SharedPointer<Window::DisplayWindow>&                  targetWindow) const
{
    System::SharedPointer<Graphics::IDeviceContext> graphicsDeviceContext;
    for (const auto& deviceContext : availableDeviceContexts)
    {
        if ((Bool)(deviceContext->SupportedQueueOperations & Graphics::QueueOperation::Graphics))
        {
            graphicsDeviceContext = deviceContext;
            break;
        }
    }

    auto                              expectedSpecification = GetGraphicsSystem()->GetSwapChainSpecification(graphicsDevice->GraphicsAdapterIndex, targetWindow);
    Graphics::SwapChainSpecification& specification         = expectedSpecification;

    Graphics::TextureFormat renderTargetFormat = Graphics::TextureFormat::Unknown;

    for (const auto& format : specification.SupportedFormats)
    {
        // BGRA8 is the best format....
        if (format == Graphics::TextureFormat::UnormB8G8R8A8)
        {
            renderTargetFormat = format;
            break;
        }
    }

    if (renderTargetFormat == Graphics::TextureFormat::Unknown)
        // Takes the first supported format
        renderTargetFormat = specification.SupportedFormats[0];


    Graphics::SwapChainDescription swapChainDescription = {};
    swapChainDescription.TargetWindow                   = targetWindow;
    swapChainDescription.ImmediateGraphicsContext       = graphicsDeviceContext;
    swapChainDescription.BackBufferCount                = System::Math::Clamp(specification.MinBackBufferCount + 1, specification.MinBackBufferCount, specification.MaxBackBufferCount);
    swapChainDescription.RenderTargetFormat             = renderTargetFormat;
    swapChainDescription.DepthStencilFormat             = Graphics::TextureFormat::UnormDepth24Stencil8;

    return graphicsDevice->CreateSwapChain(swapChainDescription);
}

void Application::Exit() noexcept
{
    _shouldExit = true;
}

void Application::Run()
{
    // Checks if the application started or not
    if (_started)
        throw System::InvalidOperationException("The application has been started already!");

    // Marks as true
    _started = true;

    // Creates window
    _window = CreateWindow();

    if (!_window)
        throw System::InvalidOperationException("Failed to create window!");

    // Creates graphics system
    auto loadedGraphicsSystem = CreateGraphicsSystem();

    if (!loadedGraphicsSystem.First || !loadedGraphicsSystem.Second)
        throw System::InvalidOperationException("Failed to create graphics system!");

    _graphicsSystem      = std::move(loadedGraphicsSystem.First);
    _graphicsSystemDyLib = std::move(loadedGraphicsSystem.Second);

    // Creates graphics device and context
    auto  graphicsDeviceAndContexts = CreateGraphicsDeviceAndContexts(_graphicsSystem);
    auto& graphicsDevice            = graphicsDeviceAndContexts.First;
    auto& deviceContexts            = graphicsDeviceAndContexts.Second;

    if (!graphicsDevice || !deviceContexts)
        throw System::InvalidOperationException("Failed to create graphics device and contexts!");

    Bool foundGraphicsDeviceContext = false;
    for (const auto& deviceContext : deviceContexts)
    {
        if ((Bool)(deviceContext->SupportedQueueOperations & Graphics::QueueOperation::Graphics))
        {
            foundGraphicsDeviceContext = true;
            break;
        }
    }

    // Needs at least 1 device context which supported graphics operation
    if (!foundGraphicsDeviceContext)
        throw System::InvalidOperationException("No graphics device context found!");

    _graphicsDevice          = std::move(graphicsDevice);
    _immediateDeviceContexts = std::move(deviceContexts);

    // Creates the swap chain
    _swapChain = CreateSwapChain(_graphicsDevice,
                                 _immediateDeviceContexts,
                                 _window);

    if (!_swapChain)
        throw System::InvalidOperationException("Failed to create swap chain!");

    LoadContent();

    Bool windowShouldClose = false;

    // Assigns window close event.
    _window->GetUserClosedWindowEvent().Add([&](Window::DisplayWindow&) {
        windowShouldClose = true;
    });

    _window->ShowWindow();

    // Timer for delta time
    System::Timer timer;

    auto Tick = [&]() {
        const System::TimePeriod deltaTime = timer.Reset();

        Components.UpdateAll(deltaTime);
        Update(deltaTime);

        const auto& swapChainGraphicsContext = _swapChain->Description.ImmediateGraphicsContext;

        auto renderTargetView = _swapChain->GetCurrentRenderTargetView();
        auto depthStencilView = _swapChain->GetCurrentDepthStencilView();

        swapChainGraphicsContext->SetRenderTarget({{renderTargetView}, depthStencilView});

        Components.RenderAll(deltaTime);
        Render(deltaTime);

        _swapChain->Present(_vSync ? 1 : 0);
    };

    _window->GetClientSizeChangedEvent().Add([&](Window::DisplayWindow&, System::Vector2UI) -> void {
        Tick();
    });

    _window->GetClientSizeChangedEvent().Add([&](Window::DisplayWindow&, System::Vector2I) -> void {
        Tick();
    });

    while (true)
    {
        _window->PollEvent();

        if (windowShouldClose || _shouldExit)
            break;

        if (_fixedTimeStep && !_vSync)
        {
            System::TimePeriod timePassed = timer.GetElapsedTimePeriod();
            if (timePassed < _timeStep)
                System::Sleep(_timeStep - timePassed);
        }

        Tick();
    }
}

} // namespace Core

} // namespace Axis
