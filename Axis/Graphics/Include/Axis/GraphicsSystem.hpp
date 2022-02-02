/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file GraphicsSystem.hpp
///
/// \brief Contains `Axis::IGraphicsSystem` interaface and various
///        Graphics APIs info data structures.

#ifndef AXIS_GRAPHICS_GRAPHICSSYSTEM_HPP
#define AXIS_GRAPHICS_GRAPHICSSYSTEM_HPP
#pragma once

#include "../../../System/Include/Axis/List.hpp"
#include "../../../System/Include/Axis/SmartPointer.hpp"
#include "../../../System/Include/Axis/Span.hpp"
#include "../../../System/Include/Axis/String.hpp"
#include "../../../System/Include/Axis/Utility.hpp"
#include "../../../System/Include/Axis/Vector2.hpp"
#include "../../../System/Include/Axis/Vector3.hpp"
#include "GraphicsCommon.hpp"
#include "GraphicsExport.hpp"
#include "SwapChain.hpp"

namespace Axis
{

/// Forward declarations
class IGraphicsDevice;
class IDeviceContext;
class ISwapChain;
struct SwapChainDescription;
class DisplayWindow;

/// \brief Graphics APIs for renderer backend.
enum class GraphicsAPI : Uint8
{
    /// \brief Unkown or not supported.
    Unknown = 0,

    /// \brief Khronos Vulkan Graphics API.
    Vulkan = AXIS_BIT(1),
};

/// \brief Contains engine's settings and configurations e.g., Graphics API and version.
struct GraphicsSystemDescription final
{
    /// \brief Graphics APIs for renderer backend.
    GraphicsAPI EngineGraphicsAPI = {};
};

/// \brief Defines the capabilities and limitation of the graphics adapter.
struct GraphicsCapability final
{
    /// \brief Maximum size of Texture2D either width or height (in pixels).
    Uint32 MaxTexture2DSize = {};

    /// \brief Maximum vertex input binding slot.
    Uint32 MaxVertexInputBinding = {};

    /// \brief The maximum dimension of frame buffer. X and Y for width and height, Z for layer count.
    Vector3UI MaxFramebufferDimension = {};

    /// \brief Max pipeline layout binding index count.
    Uint32 MaxPipelineLayoutBinding = {};

    /// \brief Gets all supported texture format
    List<TextureFormat> SupportedTextureFormats = {};
};

/// \brief Types of the graphics adapter.
enum class GraphicsAdapterType : Uint16
{
    /// \brief Unknown device type.
    Unknown,

    /// \brief The device is a separate processor connected to the CPU via an interlink.
    Dedicated,

    /// \brief The device is a running on the CPU
    CPU,

    /// \brief The device is an one embedded in or tightly coupled with CPU.
    Integrated,

    /// \brief The device is in a virtualized environment.
    Virtual,
};

/// \brief Represents a collection of device queue that have the same specified supported operations.
struct DeviceQueueFamily final
{
    /// \brief Specifies the queue supported operations and capabilities.
    QueueOperationFlags QueueType = {};

    /// \brief The number of queues in this family.
    Uint32 QueueCount = {};
};

/// \brief Represents the available swap chain specification.
struct SwapChainSpecification
{
    /// \brief Contains a list of supported texture formats for swap chain.
    List<TextureFormat> SupportedFormats = {};

    /// \brief Maximum number of back buffers that swap chain could have.
    Uint32 MaxBackBufferCount = {};

    /// \brief Minimum number of back buffers that swap chain could have.
    Uint32 MinBackBufferCount = {};
};

/// \brief Contains the information of physical graphics adapter (graphics card).
struct GraphicsAdapter final
{
    /// \brief Defines the capabilities and limitation of the GraphicsDevice.
    GraphicsCapability Capability = {};

    /// \brief Types of the graphics adapter.
    GraphicsAdapterType AdapterType = {};

    /// \brief All device queue families that this GraphicsAdapter contains.
    List<DeviceQueueFamily> DeviceQueueFamilies = {};

    /// \brief Name of the GraphicsAdapter.
    String8 Name = {};
};

/// \brief Used in immediate device context creation
struct ImmediateContextCreateInfo final
{
    /// \brief Index in GraphicsAdapter::DeviceQueueFamilies used in device context creation.
    Uint32 DeviceQueueFamilyIndex = {};
};

/// \brief Represents the GraphicsAPI which will be used in various graphics operations.
class AXIS_GRAPHICS_API IGraphicsSystem : public ISharedFromThis
{
public:
    /// \brief Description of the graphics system.
    AXIS_NODISCARD virtual GraphicsSystemDescription GetGraphicsSystemDescription() const = 0;

    /// \brief Contains all graphics adapters info which supports this graphics system.
    AXIS_NODISCARD virtual List<GraphicsAdapter> GetGraphicsAdapters() const = 0;

    /// \brief Creates graphics device and immediate device contexts.
    ///
    /// \param[in] adapterIndex Graphics adapter index in the \a `Axis::IGraphicsSystem::GetGraphicsAdapters` array, uses in graphics device creation.
    /// \param[in] pImmediateContextCreateInfos Array of ImmediateContextCreateInfo used in immediate device contexts creation.
    /// \param[in] immediateContextCount The number of immediate device context to create and pDeviceQueueFamilyIndices index count.
    /// \param[out] graphicsDeviceOut Pointer to the IGraphics* pointer.
    /// \param[out] pDeviceContextsOut Pointer to the array IDeviceContext* pointer.
    ///
    /// \note The result in the pDeviceContextsOut are not guaranteed to be exactly corresponded to the pImmediateContextCreateInfos.
    AXIS_NODISCARD virtual Pair<SharedPointer<IGraphicsDevice>, List<SharedPointer<IDeviceContext>>> CreateGraphicsDeviceAndContexts(Uint32                                  adapterIndex,
                                                                                                                                     const Span<ImmediateContextCreateInfo>& immediateContextCreateInfos) = 0;

    /// \brief Gets the swap chain specification to the specified target window.
    ///
    /// \param[in] adapterIndex Index of the GraphicsAdapter coorespoding to the IGraphicsSystem::GetGraphicsAdapters.
    /// \param[in] targetWindow The window which the swap chain will present their images to.
    AXIS_NODISCARD virtual SwapChainSpecification GetSwapChainSpecification(Uint32                              adapterIndex,
                                                                            const SharedPointer<DisplayWindow>& targetWindow) const = 0;

protected:
    /// \brief constructor
    IGraphicsSystem() noexcept;

    // Throws the exceptions upon errors
    void ValidateCreateGraphicsDeviceAndContexts(Uint32                                  adapterIndex,
                                                 const Span<ImmediateContextCreateInfo>& immediateContextCreateInfos);
};

} // namespace Axis

#endif // AXIS_GRAPHICS_GRAPHICSSYSTEM_HPP