/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_BUFFER_HPP
#define AXIS_GRAPHICS_BUFFER_HPP
#pragma once

#include "GraphicsCommon.hpp"
#include "GraphicsExport.hpp"
#include "StatedGraphicsResource.hpp"

namespace Axis
{

namespace Graphics
{

/// \brief Specifies the binding that buffer can be bound as.
enum class BufferBinding : Uint8
{
    /// \brief The buffer can be used as the destination of data transferation.
    TransferDestination = AXIS_BIT(1),

    /// \brief The buffer can be used as the source of data transferation.
    TransferSource = AXIS_BIT(2),

    /// \brief The buffer can be bound as Vertex buffer.
    Vertex = AXIS_BIT(3),

    /// \brief The buffer can be bound as Index buffer.
    Index = AXIS_BIT(4),

    /// \brief The buffer can be bound as Uniform buffer.
    Uniform = AXIS_BIT(5),

    /// \brief Required for enum reflection.
    MaximumEnumValue = Uniform,
};

/// \brief Specifies the binding that buffer can be bound as. (Bit mask)
typedef BufferBinding BufferBindingFlags;

/// \brief Describes the specification of IBuffer resource.
struct BufferDescription final
{
    /// \brief The size (in bytes) of the buffer.
    Size BufferSize = {};

    /// \brief Specifies the binding that buffer can be bound as.
    ///
    /// \note The BufferBindFlag can be combined by using bitwise OR operator.
    BufferBindingFlags BufferBinding = {};

    /// \brief Specifies the usage of the buffer.
    ResourceUsage Usage = {};

    /// \brief Specifies which device queue family index (at specifies bit position) can use this resource.
    ///
    /// \note Only specifies the device queue family which will use this resource.
    ///       Do not set any device queue family index which won't use this resource unnecessary, this will cause extra overhead.
    ///
    /// \see Axis::System::Math::AssignBitToPosition
    Uint64 DeviceQueueFamilyMask = {};
};

/// \brief Represents the contagious block of memory which can be used in GPU.
class AXIS_GRAPHICS_API IBuffer : public StatedGraphicsResource
{
public:
    /// \brief The description of IBuffer resource.
    const BufferDescription Description;

protected:
    /// \brief Constructor
    IBuffer(const BufferDescription& textureDescription);
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_BUFFER_HPP