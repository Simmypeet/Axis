/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Fence.hpp
///
/// \brief Contains `Axis::IFence` interface class.

#ifndef AXIS_GRAPHICS_FENCE_HPP
#define AXIS_GRAPHICS_FENCE_HPP
#pragma once

#include "DeviceChild.hpp"

namespace Axis
{

/// \brief Synchronization primitives which can be used to insert dependencies between queues and CPUs or multiples queues.
class AXIS_GRAPHICS_API IFence : public DeviceChild
{
public:
    /// \brief Gets the current value of the fence.
    AXIS_NODISCARD virtual Uint64 GetCurrentValue() const = 0;

    /// \brief Sets the value to the fence. The new value to set into the fence
    ///        should be greater than the current value.
    virtual void SetValue(Uint64 value) = 0;

    /// \brief Blocks the current CPU thread until the value of fence is satisfied.
    virtual void WaitForValue(Uint64 value) const = 0;

protected:
    /// \brief Default constructor
    IFence() noexcept;
};

} // namespace Axis

#endif // AXIS_GRAPHICS_FENCE_HPP