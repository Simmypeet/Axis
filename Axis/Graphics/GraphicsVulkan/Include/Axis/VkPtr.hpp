/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_GRAPHICSVULKAN_VKPTR_HPP
#define AXIS_GRAPHICSVULKAN_VKPTR_HPP
#pragma once

#include "../../../../System/Include/Axis/Function.hpp"
#include <vulkan/vulkan.h>

namespace Axis
{

/// \brief RAII wrapper over Vulkan resources (automatically destroys the resource)
///		   in the destructor
template <RawType T>
class VkPtr
{
public:
    /// \brief Constructs the Vulkan resource in the VkPtr
    ///
    /// In order to ensure strong exception (no resource leaks when the exceptions are thrown),
    /// we use RAII idiom to make sure that the resources are released when exceptions are thrown.
    ///
    /// \param[in] constructor Callable object to constructor the Vulkan resource.
    /// \param[in] deleter Callable object to release the resource.
    template <Callable<T> Constructor, Callable<void, T> Deleter>
    inline VkPtr(const Constructor& constructor,
                 Deleter&&          deleter) :
        _deleter(std::move(deleter)) // Constructs the deleter Axis::Function by move callable object in
                                     // if the exceptions are thrown here, no resource is leaked!
    {
        // Constructs the resource by using callable objects
        _resource = constructor();
    }

    /// \brief Default constructor
    VkPtr() noexcept = default;

    /// \brief Moves other resources and deleter to this
    ///        VkPtr
    inline VkPtr(VkPtr&& other) noexcept :
        _resource(other._resource),
        _deleter(std::move(other._deleter)) {}

    /// \brief Releases the resource
    inline ~VkPtr() noexcept
    {
        Release();
    }

    /// \brief Releases the resource here and turns into nullptr
    inline void Release() noexcept
    {
        if (_resource != T{} && _deleter != nullptr)
        {
            _deleter((T &&)(_resource));
        }

        _resource = {};
        _deleter  = nullptr;
    }

    /// \brief Moves the object into this
    inline VkPtr& operator=(VkPtr&& other) noexcept
    {
        if (this == std::addressof(other))
            return *this;

        Release();

        _resource = other._resource;
        _deleter  = std::move(other._deleter);

        other._resource = {};

        return *this;
    }

    /// \brief Copy assignment operator is deleted
    inline VkPtr& operator=(const VkPtr& other) = delete;

    /// \brief Implicit cast conversion
    inline operator T() const noexcept
    {
        return _resource;
    }

    /// \brief Implicit cast conversion to boolean
    inline operator Bool() const noexcept
    {
        return _resource != VK_NULL_HANDLE;
    }

private:
    T                       _resource = {};      ///< Actual resource
    Axis::Function<void(T)> _deleter  = nullptr; ///< Callable deleter
};

} // namespace Axis

#endif // AXIS_GRAPHICSVULKAN_VKPTR_HPP