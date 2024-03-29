/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanBuffer.hpp>
#include <Axis/VulkanDeviceContext.hpp>
#include <Axis/VulkanDeviceQueueFamily.hpp>
#include <Axis/VulkanFence.hpp>
#include <Axis/VulkanFramebuffer.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanGraphicsPipeline.hpp>
#include <Axis/VulkanGraphicsSystem.hpp>
#include <Axis/VulkanRenderPass.hpp>
#include <Axis/VulkanResourceHeap.hpp>
#include <Axis/VulkanResourceHeapLayout.hpp>
#include <Axis/VulkanSampler.hpp>
#include <Axis/VulkanShaderModule.hpp>
#include <Axis/VulkanSwapChain.hpp>
#include <Axis/VulkanTexture.hpp>

namespace Axis
{

namespace Graphics
{

const System::List<const char*> VulkanGraphicsDevice::DeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    VK_KHR_MAINTENANCE2_EXTENSION_NAME,
    VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME,
    VK_KHR_MAINTENANCE1_EXTENSION_NAME};

VulkanGraphicsDevice::VulkanGraphicsDevice(const System::SharedPointer<VulkanGraphicsSystem>& vulkanGraphicsSystem,
                                           Uint32                                             adapterIndex,
                                           const System::Span<ImmediateContextCreateInfo>&    pImmediateContextCreateInfos) :
    IGraphicsDevice(vulkanGraphicsSystem, adapterIndex)
{
    System::HashMap<Uint32, Uint32> deviceQueueFamilyIndexCountPairs = {};

    for (Uint64 i = 0; i < pImmediateContextCreateInfos.GetLength(); i++)
    {
        auto it = deviceQueueFamilyIndexCountPairs.Find(pImmediateContextCreateInfos[i].DeviceQueueFamilyIndex);
        if (it == deviceQueueFamilyIndexCountPairs.end())
            deviceQueueFamilyIndexCountPairs.Insert({pImmediateContextCreateInfos[i].DeviceQueueFamilyIndex, 1});
        else
            it->Second++;
    }

    auto CreateVkDevice = [&]() -> VkDevice {
        System::List<VkDeviceQueueCreateInfo> deviceQueueCreateInfos = {};
        System::List<System::List<Float32>>   deviceQueuePriorities  = {};

        Size index = 0;
        for (const auto& deviceQueueIndexCount : deviceQueueFamilyIndexCountPairs)
        {
            deviceQueuePriorities.Append(System::List<Float32>(deviceQueueIndexCount.Second, 1.0f));

            VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
            deviceQueueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            deviceQueueCreateInfo.pNext                   = nullptr;
            deviceQueueCreateInfo.flags                   = 0;
            deviceQueueCreateInfo.queueFamilyIndex        = deviceQueueIndexCount.First;
            deviceQueueCreateInfo.queueCount              = deviceQueueIndexCount.Second;
            deviceQueueCreateInfo.pQueuePriorities        = deviceQueuePriorities[index].GetData();

            deviceQueueCreateInfos.Append(deviceQueueCreateInfo);

            index++;
        }

        VkPhysicalDeviceCustomBorderColorFeaturesEXT customColorBorderFeature = {};
        customColorBorderFeature.sType                                        = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT;
        customColorBorderFeature.customBorderColorWithoutFormat               = VK_TRUE;
        customColorBorderFeature.customBorderColors                           = VK_TRUE;

        VkPhysicalDeviceVulkan12Features features = {};
        features.sType                            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features.pNext                            = &customColorBorderFeature;
        features.timelineSemaphore                = true;

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.depthClamp               = VK_TRUE;
        deviceFeatures.depthBiasClamp           = VK_TRUE;

        VkDeviceCreateInfo deviceCreateInfo      = {};
        deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext                   = &features;
        deviceCreateInfo.flags                   = 0;
        deviceCreateInfo.queueCreateInfoCount    = (Uint32)deviceQueueCreateInfos.GetLength();
        deviceCreateInfo.pQueueCreateInfos       = deviceQueueCreateInfos.GetData();
        deviceCreateInfo.enabledLayerCount       = (Uint32)VulkanGraphicsSystem::InstanceLayers.GetLength();
        deviceCreateInfo.ppEnabledLayerNames     = VulkanGraphicsSystem::InstanceLayers.GetData();
        deviceCreateInfo.enabledExtensionCount   = (Uint32)VulkanGraphicsDevice::DeviceExtensions.GetLength();
        deviceCreateInfo.ppEnabledExtensionNames = VulkanGraphicsDevice::DeviceExtensions.GetData();
        deviceCreateInfo.pEnabledFeatures        = &deviceFeatures;

        VkDevice logicalDevice = VK_NULL_HANDLE;

        auto result = vkCreateDevice(vulkanGraphicsSystem->GetVulkanPhysicalDevices()[adapterIndex].GetVkPhysicalDeviceHandle(), &deviceCreateInfo, nullptr, &logicalDevice);

        if (result != VK_SUCCESS)
            throw System::ExternalException("Failed to create vkDevice!");
        else
            return logicalDevice;
    };

    auto DestroyVkDevice = [](VkDevice device) {
        vkDestroyDevice(device, nullptr);
    };

    _vulkanLogicalDevice = VkPtr<VkDevice>(CreateVkDevice, std::move(DestroyVkDevice));

    auto CreateVmaAllocator = [&]() -> VmaAllocator {
        VmaAllocator vmaAllocator = VK_NULL_HANDLE;

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion       = vulkanGraphicsSystem->GetVulkanPhysicalDevices()[adapterIndex].GetVkPhysicalDeviceProperties().apiVersion;
        allocatorInfo.physicalDevice         = vulkanGraphicsSystem->GetVulkanPhysicalDevices()[adapterIndex].GetVkPhysicalDeviceHandle();
        allocatorInfo.device                 = GetVkDeviceHandle();
        allocatorInfo.instance               = vulkanGraphicsSystem->GetVkInstanceHandle();

        if (vmaCreateAllocator(&allocatorInfo, &vmaAllocator) != VK_SUCCESS)
            throw System::ExternalException("Failed to create vmaAllocator!");
        else
            return vmaAllocator;
    };

    auto DestroyVmaAllocator = [](VmaAllocator vmaAllocator) {
        vmaDestroyAllocator(vmaAllocator);
    };

    _vulkanMemoryAllocator = VkPtr<VmaAllocator>(CreateVmaAllocator, std::move(DestroyVmaAllocator));

    for (const auto& deviceQueueFamilyIndexCount : deviceQueueFamilyIndexCountPairs)
        _deviceQueueFamilies.Insert({deviceQueueFamilyIndexCount.First, VulkanDeviceQueueFamily(deviceQueueFamilyIndexCount.First, deviceQueueFamilyIndexCount.Second, *this)});
}

System::SharedPointer<ISwapChain> VulkanGraphicsDevice::CreateSwapChain(const SwapChainDescription& description)
{
    ValidateCreateSwapChain(description);

    return Axis::System::MakeShared<VulkanSwapChain>(description, *this);
}

System::SharedPointer<ITextureView> VulkanGraphicsDevice::CreateTextureView(const TextureViewDescription& description)
{
    ValidateCreateTextureView(description);

    return Axis::System::MakeShared<VulkanTextureView>(description, *this);
}

System::SharedPointer<IRenderPass> VulkanGraphicsDevice::CreateRenderPass(const RenderPassDescription& description)
{
    ValidateCreateRenderPass(description);

    return Axis::System::MakeShared<VulkanRenderPass>(description, *this);
}

System::SharedPointer<IFramebuffer> VulkanGraphicsDevice::CreateFramebuffer(const FramebufferDescription& description)
{
    ValidateCreateFramebuffer(description);

    return Axis::System::MakeShared<VulkanFramebuffer>(description, *this);
}

System::SharedPointer<IShaderModule> VulkanGraphicsDevice::CompileShaderModule(const ShaderModuleDescription&  description,
                                                                               const System::StringView<Char>& sourceCode)
{
    ValidateCompileShaderModule(description, sourceCode);

    return Axis::System::MakeShared<VulkanShaderModule>(description, sourceCode, *this);
}

System::SharedPointer<IResourceHeapLayout> VulkanGraphicsDevice::CreateResourceHeapLayout(const ResourceHeapLayoutDescription& description)
{
    ValidateCreateResourceHeapLayout(description);

    return Axis::System::MakeShared<VulkanResourceHeapLayout>(description, *this);
}

System::SharedPointer<IGraphicsPipeline> VulkanGraphicsDevice::CreateGraphicsPipeline(const GraphicsPipelineDescription& description)
{
    ValidateCreateGraphicsPipeline(description);

    return Axis::System::MakeShared<VulkanGraphicsPipeline>(description, *this);
}

System::SharedPointer<IBuffer> VulkanGraphicsDevice::CreateBuffer(const BufferDescription& description,
                                                                  const BufferInitialData* pInitialData)
{
    ValidateCreateBuffer(description, pInitialData);

    auto buffer = Axis::System::MakeShared<VulkanBuffer>(description, pInitialData, *this);

    if (pInitialData)
    {
        if (!Graphics::IsResourceUsageMappable(buffer->Description.Usage))
        {
            // Creates staging buffer
            BufferDescription stagingBufferDescription = {
                .BufferSize            = pInitialData->DataSize,
                .BufferBinding         = BufferBinding::TransferSource,
                .Usage                 = ResourceUsage::StagingSource,
                .DeviceQueueFamilyMask = (Size)System::Math::AssignBitToPosition(0, pInitialData->ImmediateContext->DeviceQueueFamilyIndex, true)};

            auto stagingBuffer = CreateBuffer(stagingBufferDescription, nullptr);

            // Maps the staging buffer
            auto mappedMemory = pInitialData->ImmediateContext->MapBuffer(stagingBuffer,
                                                                          MapAccess::Write,
                                                                          MapType::Overwrite);

            // Copies the initial data to the staging buffer
            std::memcpy(mappedMemory, pInitialData->Data, buffer->Description.BufferSize);

            // Unmaps the staging buffer
            pInitialData->ImmediateContext->UnmapBuffer(stagingBuffer);

            // Copies the staging buffer to the buffer
            pInitialData->ImmediateContext->CopyBuffer(stagingBuffer,
                                                       0,
                                                       buffer,
                                                       pInitialData->Offset,
                                                       pInitialData->DataSize);

            // Flushes the command buffer
            pInitialData->ImmediateContext->Flush();
        }
        else
        {
            // Simply maps the buffer here
            auto mappedMemory = pInitialData->ImmediateContext->MapBuffer(buffer,
                                                                          MapAccess::Write,
                                                                          MapType::Overwrite);

            // Copies the initial data to the buffer with offset
            std::memcpy(((Byte*)mappedMemory) + pInitialData->Offset, pInitialData->Data, buffer->Description.BufferSize);

            // Unmaps the buffer
            pInitialData->ImmediateContext->UnmapBuffer(buffer);
        }
    }

    return buffer;
}

System::SharedPointer<ITexture> VulkanGraphicsDevice::CreateTexture(const TextureDescription& description)
{
    ValidateCreateTexture(description);

    return Axis::System::MakeShared<VulkanTexture>(description, *this);
}

System::SharedPointer<IResourceHeap> VulkanGraphicsDevice::CreateResourceHeap(const ResourceHeapDescription& description)
{
    ValidateCreateResourceHeap(description);

    return Axis::System::MakeShared<VulkanResourceHeap>(description, *this);
}

System::SharedPointer<ISampler> VulkanGraphicsDevice::CreateSampler(const SamplerDescription& description)
{
    return Axis::System::MakeShared<VulkanSampler>(description, *this);
}

System::SharedPointer<IFence> VulkanGraphicsDevice::CreateFence(Uint64 initialValue)
{
    return Axis::System::MakeShared<VulkanFence>(initialValue, *this);
}

void VulkanGraphicsDevice::WaitDeviceIdle() const noexcept
{
    vkDeviceWaitIdle(_vulkanLogicalDevice);
}

System::List<System::SharedPointer<IDeviceContext>> VulkanGraphicsDevice::GetDeviceContexts()
{
    auto graphicsAdapters = GraphicsSystem->GetGraphicsAdapters();

    System::List<System::SharedPointer<IDeviceContext>> deviceContexts = {};

    for (const auto& vulkanDeviceQueueFamily : _deviceQueueFamilies)
    {
        for (Size i = 0; i < vulkanDeviceQueueFamily.Second.GetDeviceQueueCount(); i++)
        {
            auto deviceContext = Axis::System::MakeShared<VulkanDeviceContext>(vulkanDeviceQueueFamily.First,
                                                                               (Uint32)i,
                                                                               graphicsAdapters[GraphicsAdapterIndex].DeviceQueueFamilies[vulkanDeviceQueueFamily.First].QueueType,
                                                                               *this);

            deviceContexts.Append(deviceContext);

            _vulkanDeviceContexts.Append(deviceContext);
        }
    }

    return deviceContexts;
}

} // namespace Graphics

} // namespace Axis
