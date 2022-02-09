/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanResourceHeapLayout.hpp>
#include <Axis/VulkanUtility.hpp>
#include <vulkan/vulkan.hpp>

namespace Axis
{

namespace Graphics
{

VulkanResourceHeapLayout::VulkanResourceHeapLayout(const ResourceHeapLayoutDescription& description,
                                                   VulkanGraphicsDevice&                vulkanGraphicsDevice) :
    IResourceHeapLayout(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkDescriptorSetLayout = [&]() -> VkDescriptorSetLayout {
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;

        System::List<VkDescriptorSetLayoutBinding> setLayoutBindings;
        setLayoutBindings.ReserveFor(description.ResourceBindings.GetLength());

        for (const auto& resourceBinding : description.ResourceBindings)
        {
            VkDescriptorSetLayoutBinding setLayoutBinding = {};
            setLayoutBinding.binding                      = resourceBinding.BindingIndex;
            setLayoutBinding.descriptorType               = VulkanUtility::GetVkDescriptorTypeFromResourceBinding(resourceBinding.Binding);
            setLayoutBinding.descriptorCount              = resourceBinding.ArraySize;
            setLayoutBinding.stageFlags                   = VulkanUtility::GetVkShaderStageFlagsFromShaderStageFlags(resourceBinding.StageFlags);
            setLayoutBinding.pImmutableSamplers           = nullptr;

            setLayoutBindings.Append(std::move(setLayoutBinding));
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount                    = (Uint32)setLayoutBindings.GetLength();
        descriptorSetLayoutCreateInfo.pBindings                       = setLayoutBindings.GetLength() == 0 ? nullptr : setLayoutBindings.GetData();

        auto vkResult = vkCreateDescriptorSetLayout(vulkanGraphicsDevice.GetVkDeviceHandle(), &descriptorSetLayoutCreateInfo, nullptr, &vkDescriptorSetLayout);

        if (vkResult != VK_SUCCESS)
            throw System::ExternalException("Failed to create VkDescriptorSetLayout!");

        return vkDescriptorSetLayout;
    };

    auto DestroyVkDescriptorSetLayout = [this](VkDescriptorSetLayout vkDescriptorSetLayout) {
        vkDestroyDescriptorSetLayout(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkDescriptorSetLayout, nullptr);
    };

    _vulkanDescriptorSetLayout = VkPtr<VkDescriptorSetLayout>(CreateVkDescriptorSetLayout, std::move(DestroyVkDescriptorSetLayout));
}

} // namespace Graphics

} // namespace Axis
