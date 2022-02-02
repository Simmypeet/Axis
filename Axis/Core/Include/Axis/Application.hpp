/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

/// \file Application.hpp
///
/// \brief Contains the definition of `Axis::Application` class.

#ifndef AXIS_CORE_APPLICATION_HPP
#define AXIS_CORE_APPLICATION_HPP
#pragma once

#include "../../Graphics/Include/Axis/SwapChain.hpp"
#include "../../System/Include/Axis/TimePeriod.hpp"
#include "ComponentCollection.hpp"
#include "CoreExport.hpp"

namespace Axis
{

/// Forward declarations
class DisplayWindow;
class IGraphicsSystem;
class IGraphicsDevice;
class IDeviceContext;
class Assembly;

/// \brief The entry point for the program. Handles setting
///        up a window and low-level graphics system and runs an application loop.
class AXIS_CORE_API Application : public ISharedFromThis
{
public:
    /// Default constructor
    Application() noexcept;

    /// Virtual destructor
    virtual ~Application() noexcept override = default;

    /// \brief Starts the application.
    void Run();

protected:
    /// \brief Collections of \a `Axis::ApplicationComponent`s which belong
    ///        to this application
    ComponentCollection Components;

    /// \brief Gets the window of the application.
    AXIS_NODISCARD inline const SharedPointer<DisplayWindow>& GetWindow() const noexcept { return _window; }

    /// \brief Gets the application loaded graphics system.
    AXIS_NODISCARD inline const SharedPointer<IGraphicsSystem>& GetGraphicsSystem() const noexcept { return _graphicsSystem; }

    /// \brief Gets the created graphics device.
    AXIS_NODISCARD inline const SharedPointer<IGraphicsDevice>& GetGraphicsDevice() const noexcept { return _graphicsDevice; }

    /// \brief Gets the created swap chain.
    AXIS_NODISCARD inline const SharedPointer<ISwapChain>& GetSwapChain() const noexcept { return _swapChain; }

    /// \brief Gets the immediate graphics device context used by swap chain.
    AXIS_NODISCARD inline const SharedPointer<IDeviceContext>& GetImmediateGraphicsContext() const noexcept { return _swapChain->Description.ImmediateGraphicsContext; }

    /// \brief Gets all created immediate device contexts.
    AXIS_NODISCARD inline const List<SharedPointer<IDeviceContext>>& GetImmediateDeviceContexts() const noexcept { return _immediateDeviceContexts; }

    /// \brief Indicates whether the application will be run in the specified fixed time step or not.
    inline void SetFixedTimeStepEnabled(Bool fixedTimeStep) noexcept { _fixedTimeStep = fixedTimeStep; }

    /// \brief Gets the current fixed time step mode.
    AXIS_NODISCARD inline Bool IsFixedTimeStepEnabled() const noexcept { return _fixedTimeStep; }

    /// \brief Specifies the value of time step for the application loop if
    ///        the application is running in fixed time step mode.
    inline void SetApplicationTimeStep(const TimePeriod& timeStep) noexcept { _timeStep = timeStep; }

    /// \brief Gets the current application's time step value.
    AXIS_NODISCARD inline TimePeriod GetApplicationTimeStep() const noexcept { return _timeStep; }

    /// \brief Specifies whether to present the image in VSync mode.
    inline void SetVSyncEnabled(Bool enable) noexcept { _vSync = enable; }

    /// \brief Indicates whether the application enabling Vsync or not.
    AXIS_NODISCARD inline Bool IsVsyncEnabled() const noexcept { return _vSync; }

    /// \brief Creates the window, Gets called once before \a LoadContent.
    ///
    /// \note The client can override this function to adjust their \a CreateWindow
    ///       function to their needs.
    ///
    /// \warning Return value should be valid SharedPointer<DisplayWindow> pointer.
    AXIS_NODISCARD virtual SharedPointer<DisplayWindow> CreateWindow() const;

    /// \brief Creates the graphics system, Gets called once before \a LoadContent and after \a CreateWindow
    ///
    /// \note The client can override this function to adjust their \a CreateGraphicsSystem
    ///       function to their needs.
    AXIS_NODISCARD virtual Pair<SharedPointer<IGraphicsSystem>, SharedPointer<Assembly>> CreateGraphicsSystem() const;

    /// \brief Creates the graphics device and immediate contexts, Gets called once before \a LoadContent and after \a CreateGraphicsSystem.
    ///
    /// \note The client can override this function to adjust their \a CreateGraphicsDeviceAndContexts
    ///       function to their needs.
    AXIS_NODISCARD virtual Pair<SharedPointer<IGraphicsDevice>, List<SharedPointer<IDeviceContext>>> CreateGraphicsDeviceAndContexts(const SharedPointer<IGraphicsSystem>& graphicsSystem) const;

    /// \brief Creates the swap chain, Gets called once before \a LoadContent and after \a CreateGraphicsDeviceAndContexts
    ///
    /// \note The client can override this function to adjust their \a CreateSwapChain
    ///       function to their needs.
    AXIS_NODISCARD virtual SharedPointer<ISwapChain> CreateSwapChain(const SharedPointer<IGraphicsDevice>&      graphicsDevice,
                                                                     const List<SharedPointer<IDeviceContext>>& availableDeviceContexts,
                                                                     const SharedPointer<DisplayWindow>&        targetWindow) const;

    /// \brief Gets called once after the setting up of the window and
    ///        graphics system is done.
    virtual void LoadContent() = 0;

    /// \brief Defines the application update's logic loop.
    virtual void Update(const TimePeriod& deltaTime) = 0;

    /// \brief Defines the application render's logic loop.
    virtual void Render(const TimePeriod& deltaTime) = 0;

    /// \brief Requests to exit the application before next \a Application::Update call.
    void Exit() noexcept;

private:
    SharedPointer<Assembly>             _graphicsSystemDyLib     = nullptr;                               ///< Application's created dynamic library (destruct the last)
    SharedPointer<DisplayWindow>        _window                  = nullptr;                               ///< Application's window
    SharedPointer<IGraphicsSystem>      _graphicsSystem          = nullptr;                               ///< Application's loaded graphics system
    SharedPointer<IGraphicsDevice>      _graphicsDevice          = nullptr;                               ///< Application's created graphics device
    List<SharedPointer<IDeviceContext>> _immediateDeviceContexts = nullptr;                               ///< All created immediate device context
    SharedPointer<ISwapChain>           _swapChain               = nullptr;                               ///< Swap chain targetted to the application's window
    Bool                                _shouldExit              = false;                                 ///< Checks if the application requests to exit
    Bool                                _started                 = false;                                 ///< Indicates whether the application is already started or not.
    Bool                                _vSync                   = true;                                  ///< Indicates whether vsync is enabled or not.
    TimePeriod                          _timeStep                = TimePeriod::FromSeconds(1.0f / 60.0f); ///< Specifies fixed time step if the application run in fixed time step. (Default value is 60 Fps)
    Bool                                _fixedTimeStep           = {};                                    ///< Specifies whether application is running
};

} // namespace Axis

#endif // AXIS_CORE_APPLICATION_HPP