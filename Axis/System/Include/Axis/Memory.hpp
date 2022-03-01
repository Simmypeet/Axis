/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_MEMORY_HPP
#define AXIS_SYSTEM_MEMORY_HPP
#pragma once

#include "Config.hpp"
#include "Trait.hpp"
#include "Utility.hpp"
#include "SystemExport.hpp"

namespace Axis
{

namespace System
{

/// \brief The type which handles memory allocation and deallocation
///
/// Constraints:
/// - have public static member function with give signature:
///   - `Axis::PVoid Allocate(Axis::Size byteSize, Axis::Size alignment) noexcept(/*optional*/)`
///   - `void Deallocate(Axis::PVoid allocatedMemory) noexcept(true)`
template <typename T>
concept MemoryResourceType = requires(PVoid allocatedMemory,
                                      Size  byteSize,
                                      Size  alignment)
{
    {
        // Allocates the memory with the given size and alignment
        T::Allocate(byteSize,
                    alignment)
        } -> IsSame<PVoid>;
    {
        // Deallocates the memory for the allocated pointer (must be noexcept!)
        T::Deallocate(allocatedMemory)
        } -> IsSame<void>;
}
&&noexcept(T::Deallocate(MakeValue<PVoid>()));

/// \brief MemoryResource class which uses operator new and delete
///        for memory allocation and deallocation
struct AXIS_SYSTEM_API MemoryResource
{
public:
    /// \brief Can't be instantiated
    MemoryResource() = delete;

    /// \brief Allocates the memory with the given size and alignment
    ///
    /// \param[in] byteSize  The size of the memory to allocate
    /// \param[in] alignment The alignment of the memory to allocate
    ///
    /// \returns The pointer to the allocated memory
    AXIS_NODISCARD static PVoid Allocate(Size byteSize,
                                         Size alignment);

    /// \brief Deallocates the memory for the allocated pointer
    ///
    /// \param[in] allocatedMemory The allocated memory
    static void Deallocate(PVoid allocatedMemory) noexcept;
};

/// \brief MemoryResource class which uses operator new and delete
///        for memory allocation and deallocation, but pools the memory
///        when deallocates the memory.
struct AXIS_SYSTEM_API PoolMemoryResource
{
public:
    /// \brief Can't be instantiated
    PoolMemoryResource() = delete;

    /// \brief Allocates the memory with the given size and alignment
    ///
    /// \param[in] byteSize  The size of the memory to allocate
    /// \param[in] alignment The alignment of the memory to allocate
    ///
    /// \returns The pointer to the allocated memory
    AXIS_NODISCARD static PVoid Allocate(Size byteSize,
                                         Size alignment);

    /// \brief Deallocates the memory for the allocated pointer
    ///
    /// \param[in] allocatedMemory The allocated memory
    static void Deallocate(PVoid allocatedMemory) noexcept;
};

/// \brief Axis's default memory resource type
using DefaultMemoryResource = PoolMemoryResource;

/// \brief Creates a new instance of the specified type using the
///        specified memory resource on the heap. Uses \a `Delete` to delete
///        the instance.
///
/// \param[in] args The arguments to pass to the constructor.
///
/// \return A new instance of the specified type.
template <MemoryResourceType MemoryResource, RawType T, class... Args>
AXIS_NODISCARD T* MemoryNew(Args&&... args) noexcept(IsNothrowConstructible<T, Args...>);

/// \brief Creates the array of new instances of the specified type using
///        the specified memory resource on the heap. Uses \a `DeleteArray` to
///        delete the array.
///
/// \param[in] elementCount Number of elements to allocate.
/// \param[in] args         The arguments to pass to the constructor.
///
/// \return A new array of the specified type.
template <MemoryResourceType MemoryResource, RawType T, class... Args>
AXIS_NODISCARD T* MemoryNewArray(Size elementCount, Args&&... args) noexcept(IsNothrowConstructible<T, Args...>);

/// \brief Deletes the instance and frees the memory, must use the
///        same memory resource type as the one used to allocate the instance.
///
/// \param[in] instance Pointer to the instance to delete.
template <MemoryResourceType MemoryResource, RawConstableType T>
void MemoryDelete(T* instance) noexcept;

/// \brief Deletes the array and frees the memory, must use the
///        same memory resource type as the one used to allocate the array.
///
/// \param[in] array Pointer to the array to delete.
template <MemoryResourceType MemoryResource, RawConstableType T>
void MemoryDeleteArray(T* array) noexcept;

/// \brief Creates a new instance of the specified type using the
///        default memory resource on the heap. Uses \a `Delete` to delete
///        the instance.
///
/// \param[in] args The arguments to pass to the constructor.
///
/// \return A new instance of the specified type.
template <RawType T, class... Args>
AXIS_NODISCARD inline T* New(Args&&... args) noexcept(noexcept(MemoryNew<DefaultMemoryResource, T, Args...>(Forward<Args>(args)...)));

/// \brief Creates the array of new instances of the specified type using
///        the default memory resource on the heap. Uses \a `DeleteArray` to
///        delete the array.
///
/// \param[in] elementCount Number of elements to allocate.
/// \param[in] args         The arguments to pass to the constructor.
///
/// \return A new array of the specified type.
template <RawType T, class... Args>
AXIS_NODISCARD inline T* NewArray(Size elementCount, Args&&... args) noexcept(noexcept(MemoryNewArray<DefaultMemoryResource, T, Args...>(elementCount, Forward<Args>(args)...)));

/// \brief Deletes the instance and frees the memory
///
/// \param[in] instance Pointer to the instance to delete.
template <RawConstableType T>
inline void Delete(T* instance) noexcept;

/// \brief Deletes the array and frees the memory.
///
/// \param[in] array Pointer to the array to delete.
template <RawConstableType T>
inline void DeleteArray(T* array) noexcept;

} // namespace System

} // namespace Axis

#include "../../Private/Axis/MemoryImpl.inl"

#endif // AXIS_SYSTEM_MEMORY_HPP
