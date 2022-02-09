/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_WINDOW_WIN32MOUSE_HPP
#define AXIS_WINDOW_WIN32MOUSE_HPP
#pragma once

#include "../../../../../System/Include/Axis/SmartPointer.hpp"
#include "../../Mouse.hpp"
#include "../../WindowExport.hpp"

namespace Axis
{

namespace Window
{

// Forward declarations
class DisplayWindow;

/// \brief Allows retrieving mouse informations, Mouse clicks, scroll wheel
///        value and position.
class AXIS_WINDOW_API Mouse final : public System::ISharedFromThis
{
public:
    /// \brief Constructs a mouse object.
    ///
    /// \param[in] window Window in which mouse will receive the events from.
    /// \param[out] resultCode ResultCode for reporting errors
    explicit Mouse(const System::SharedPointer<DisplayWindow>& window);

    /// \brief Destructor
    ~Mouse() noexcept;

    /// \brief Retrieves a \a `MouseState` which contains mouse
    ///        buttons' states and position.
    AXIS_NODISCARD MouseState GetMouseState() const;

private:
    /// Private members
    System::SharedPointer<DisplayWindow> _window                     = nullptr;
    Float32                              _verticalScrollWheelValue   = 0;
    Float32                              _horizontalScrollWheelValue = 0;
    Size                                 _eventToken                 = 0;
};

} // namespace Window

} // namespace Axis

#endif // AXIS_WINDOW_WIN32MOUSE_HPP