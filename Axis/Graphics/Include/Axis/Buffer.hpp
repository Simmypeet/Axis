/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.
///

/// \file Buffer.hpp
///
/// \brief Contains `Axis::IBuffer` interface class.
///

#ifndef AXIS_GRAPHICS_BUFFER_HPP
#define AXIS_GRAPHICS_BUFFER_HPP
#pragma once

#include "GraphicsCommon.hpp"
#include "GraphicsExport.hpp"
#include "StatedGraphicsResource.hpp"

namespace Axis
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
    /// \see Axis::Math::AssignBitToPosition
    Uint64 DeviceQueueFamilyMask = {};
};

/// \brief Represents the contagious block of memory which can be used in GPU.
///
class AXIS_GRAPHICS_API IBuffer : public StatedGraphicsResource
{
public:
    /// \brief The description of IBuffer resource.
    const BufferDescription Description;

    /// \brief Maps the resource's memory. The resource must contains the flag of ResourceUsage::Dynamic or ResourceUsage::StagingSource.
    ///        The user should taking care of synchronization issues that might happen e.g., mapping the data and write to the vertex buffer while the
    ///        memory is being read by the vertex shader.
    ///
    /// \param[in] mapType Specifies the texture mapping behaviour.
    ///
    /// \return Mapped memory region. The user should be aware of accessing the mapped memory, or else
    ///         memory access violation might occur.
    AXIS_NODISCARD virtual PVoid MapMemory(ResourceMapType mapType = ResourceMapType::Default) = 0;

    /// \brief Unmaps the resource memory.
    ///
    virtual void UnmapMemory() = 0;

    /// \brief Flushes the mapped memory, makes sure that all written memory from the CPU appears correctly on the GPU.
    ///
    /// \param[in] offset Offset (in bytes) of the buffer to flush the memory.
    /// \param[in] size Size (in bytes) of memory to be flushed after the offset.
    ///
    virtual void FlushMappedMemoryRange(Size offset,
                                        Size size) = 0;

    /// \brief Invalidates the mapped memory, makes sure that all written memory from the GPU appears correctly on the CPU.
    ///
    /// \param[in] offset Offset (in bytes) of the buffer to invalidate the memory.
    /// \param[in] size Size (in bytes) of memory to be invalidated after the offset.
    ///
    virtual void InvalidateMappedMemoryRange(Size offset,
                                             Size size) = 0;

protected:
    /// \brief Constructor
    ///
    IBuffer(const BufferDescription& textureDescription);
};

} // namespace Axis

#endif // AXIS_GRAPHICS_BUFFER_HPP