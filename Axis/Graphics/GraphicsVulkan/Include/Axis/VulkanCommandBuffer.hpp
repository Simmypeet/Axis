/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANCOMMANDBUFFER_HPP
#define AXIS_VULKANCOMMANDBUFFER_HPP
#pragma once

#include "../../../../System/Include/Axis/HashSet.hpp"
#include "../../../Include/Axis/DeviceChild.hpp"
#include "VkPtr.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class IRenderPass;
class IFramebuffer;
class VulkanGraphicsDevice;

// Encapsulation of VkCommandBuffer.
struct VulkanCommandBuffer final : public DeviceChild
{
public:
    // Constructor
    VulkanCommandBuffer(VkCommandPool         commandPool,
                        VkCommandBufferLevel  commandBufferLevel,
                        VulkanGraphicsDevice& vulkanGraphicsDevice);

    // Destructor
    ~VulkanCommandBuffer() noexcept;

    // Begins recording command buffer.
    void BeginRecording() noexcept;

    // Ends recording command buffer.
    void EndRecording() noexcept;

    // Resets command buffer and also release all the resource references bound to this command buffer.
    void ResetCommandBuffer() noexcept;

    // Binds the resource to this command buffer.
    void AddResourceStrongReference(const System::SharedPointer<void>& reference);

    // Checks if command buffer is ready for submission.
    Bool IsCommandBufferAvailable() const noexcept;

    // Checks if command buffer is recording.
    inline Bool IsRecording() const noexcept { return _isRecording; }

    // Begins the render pass an stores the state.
    void BeginRenderPass(const System::SharedPointer<IRenderPass>&  renderPass,
                         const System::SharedPointer<IFramebuffer>& framebuffer,
                         VkRenderPassBeginInfo*                     beginInfo) noexcept;

    // Ends the current render pass and stores the state.
    void EndRenderPass() noexcept;

    // Checks whether if the render has begun or not.
    inline Bool IsRenderPassActivating() const noexcept { return _activatingRenderPass; }

    // Gets the internal VkCommandBuffer handle.
    inline VkCommandBuffer GetVkCommandBufferHandle() const noexcept { return _commandBuffer; }

    // Gets semaphore which will be signaled everytime it gets submitted.
    inline VkSemaphore GetSignalVkSemaphore() const noexcept { return _signalSemaphore; }

private:
    // Void reference pointer hasher
    struct ReferencePointerHasher
    {
        // Gets the hash for the void reference pointer
        Size operator()(const System::SharedPointer<void>& referencePointer) const noexcept
        {
            return (Size)((PVoid)referencePointer);
        }
    };

    VkPtr<VkCommandBuffer>                                               _commandBuffer         = {};
    VkPtr<VkSemaphore>                                                   _signalSemaphore       = {};
    VkPtr<VkFence>                                                       _submitFence           = {};
    Bool                                                                 _isRecording           = false;
    System::HashSet<System::SharedPointer<void>, ReferencePointerHasher> _resourceReference     = {}; // Keeps the strong reference
    System::SharedPointer<IRenderPass>                                   _activatingRenderPass  = {};
    System::SharedPointer<IFramebuffer>                                  _activatingFramebuffer = {};

    friend struct VulkanDeviceQueue;
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANCOMMANDBUFFER_HPP