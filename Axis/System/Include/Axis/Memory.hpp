/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_MEMORY_HPP
#define AXIS_SYSTEM_MEMORY_HPP
#pragma once

#include "SystemExport.hpp"
#include "Trait.hpp"
#include <concepts>

namespace Axis
{

namespace System
{

/// \brief Concept for checking if a type is capable being an allocator
///        for the memory management system.
///
/// The type should have public static function `Allocate` and `Deallocate`
template <class T>
concept AllocatorType = requires(T t)
{
    {
        // Allocates memory with the given size and alignment
        T::Allocate((Size)0, (Size)0)
        } -> SameAs<PVoid>;

    {
        // Deallocates the given memory pointer
        T::Deallocate((PVoid) nullptr)
        } -> SameAs<PVoid>;
};

/// \brief Allocator class which satisfies the concept `AllocatorType`.
///        It uses the standard `std::malloc` and `std::free` functions to
///        allocate and deallocate memory.
struct AXIS_SYSTEM_API MallocAllocator
{
    /// \brief Allocates memory with the given size and alignment
    ///
    /// \param[in] size The size of the memory to allocate
    /// \param[in] alignment The alignment of the memory to allocate
    AXIS_NODISCARD static PVoid Allocate(Size size, Size alignment);

    /// \brief Deallocates the given memory pointer
    ///
    /// \param[in] ptr The pointer to deallocate
    static void Deallocate(PVoid ptr) noexcept;
};

/// \brief Allocator class which satisfies the concept `AllocatorType`.
///        the allocator pools the memory when it is deallocated for the next
///        allocation.
struct AXIS_SYSTEM_API PoolAllocator
{
    /// \brief Allocates / retrieves memory with the given size and alignment
    ///
    /// \param[in] size The size of the memory to allocate
    /// \param[in] alignment The alignment of the memory to allocate
    AXIS_NODISCARD static PVoid Allocate(Size size, Size alignment);

    /// \brief Returns the given memory pointer back to the pool
    ///
    /// \param[in] ptr The pointer to deallocate
    static void Deallocate(PVoid ptr) noexcept;
};

/// \brief Axis's default container type memory allocator.
using DefaultAllocator = MallocAllocator;

/// \brief Creates a new instance of the specified type using the
///        specified allocator on the heap. Uses \a `Delete` to delete
///        the instance.
///
/// \tparam T Type of the instance to create.
/// \tparam Allocator Type of the allocator to use.
///
/// \return A new instance of the specified type.
template <AllocatorType Allocator, RawType T, class... Args>
AXIS_NODISCARD T* AllocatedNew(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) requires(std::is_constructible_v<T, Args...>);

/// \brief Creates the array of new instances of the specified type using
///        the specified allocator on the heap. Uses \a `DeleteArray` to
///        delete the array.
///
/// \tparam T Type of the instance to create.
/// \tparam AllocatorType Type of the allocator to use.
///
/// \param[in] elementCount Number of elements to allocate.
///
/// \return A new array of the specified type.
template <AllocatorType Allocator, RawType T>
AXIS_NODISCARD T* AllocatedNewArray(Size elementCount) noexcept(std::is_nothrow_default_constructible_v<T>) requires(std::is_default_constructible_v<T>);

/// \brief Deletes the instance and frees the memory, must use the
///        same allocator type as the one used to allocate the instance.
///
/// \tparam T Type of the instance to delete.
/// \tparam AllocatorType Type of the allocator to use.
///
/// \param[in] instance Pointer to the instance to delete.
template <AllocatorType Allocator, RawConstableType T>
void AllocatedDelete(T* instance) noexcept;

/// \brief Deletes the array and frees the memory, must use the
///        same allocator type as the one used to allocate the array.
///
/// \tparam T Type of the instance to delete.
/// \tparam AllocatorType Type of the allocator to use.
///
/// \param[in] array Pointer to the array to delete.
template <AllocatorType Allocator, RawConstableType T>
void AllocatedDeleteArray(T* array) noexcept;

/// \brief Creates a new instance of the specified type using the
///        default allocator on the heap. Uses \a `Delete` to delete
///        the instance.
///
/// \tparam T Type of the instance to create.
///
/// \return A new instance of the specified type.
template <RawType T, class... Args>
AXIS_NODISCARD inline T* New(Args&&... args) { return AllocatedNew<DefaultAllocator, T>(std::forward<Args>(args)...); }

/// \brief Creates the array of new instances of the specified type using
///        the default allocator on the heap. Uses \a `DeleteArray` to
///        delete the array.
///
/// \tparam T Type of the instance to create.
///
/// \param[in] elementCount Number of elements to allocate.
///
/// \return A new array of the specified type.
template <RawType T>
AXIS_NODISCARD inline T* NewArray(Size elementCount) { return AllocatedNewArray<DefaultAllocator, T>(elementCount); }

/// \brief Deletes the instance and frees the memory
///
/// \tparam T Type of the instance to delete.
///
/// \param[in] instance Pointer to the instance to delete.
template <RawConstableType T>
inline void Delete(T* instance) noexcept { AllocatedDelete<DefaultAllocator, T>(instance); }

/// \brief Deletes the array and frees the memory.
///
/// \tparam T Type of the instance to delete.
///
/// \param[in] array Pointer to the array to delete.
template <RawConstableType T>
inline void DeleteArray(T* array) noexcept { AllocatedDeleteArray<DefaultAllocator, T>(array); }

} // namespace System

} // namespace Axis

#include "../../Private/Axis/MemoryImpl.inl"

#endif // AXIS_SYSTEM_MEMORY_HPP
