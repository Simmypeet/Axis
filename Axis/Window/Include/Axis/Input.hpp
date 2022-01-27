/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Input.hpp
/// 
/// \brief Contains input related data structures and enums.

#ifndef AXIS_WINDOW_INPUT_HPP
#define AXIS_WINDOW_INPUT_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"

namespace Axis
{

/// \brief States of the buttons.
enum class ButtonState : Uint8
{
    /// \brief The button is pressed.
    Pressed,

    /// \brief The button is released.
    Released,
};

} // namespace Axis

#endif // AXIS_WINDOW_INPUT_HPP