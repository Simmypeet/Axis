/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_MEMORY_HPP
#define AXIS_SYSTEM_MEMORY_HPP
#pragma once

#include "SystemExport.hpp"
#include "Utility.hpp"

namespace Axis::System
{

namespace Concept
{

/// \brief The type which handles memory allocation and deallocation
///
/// The term `MemoryResource` refers to the type that is capable of allocating and
/// deallocating memory.
///
/// The class should have these following static function:
///     - *PVoid Allocate(Size byteSize)*
///     - *void Deallocate(PVoid memoryPtr) noexcept*
///
/// \tparam T Type to check for the constraints
template <typename T>
concept MemoryResource = IsSame<decltype(T::Allocate(Size{})), PVoid> && noexcept(T::Deallocate(PVoid{})) && Pure<T>;

} // namespace Concept

/// \brief MemoryResource class which uses operator new and delete
///        for memory allocation and deallocation
struct AXIS_SYSTEM_API MemoryResource
{
public:
    /// \brief Allocates the memory with the given size and alignment
    ///
    /// \param[in] byteSize  The size of the memory to allocate
    ///
    /// \returns The pointer to the allocated memory
    AXIS_NODISCARD static PVoid Allocate(Size byteSize);

    /// \brief Deallocates the memory for the allocated pointer
    ///
    /// \param[in] allocatedMemory The allocated memory
    static void Deallocate(PVoid allocatedMemory) noexcept;
};

/// \brief Axis's default memory resource type
using DefaultMemoryResource = MemoryResource;

/// \brief Gets the maximum number of elements that can be allocated at once.
///
/// \tparam T Type to check for maximum array size
template <Concept::Pure T>
AXIS_NODISCARD constexpr Size GetMaxArraySize() noexcept;

/// \brief Creates a new instance of the specified type using the
///        specified memory resource on the heap. Uses \a `Delete` to delete
///        the instance.
///
/// \param[in] args The arguments to pass to the constructor.
///
/// \return A new instance of the specified type.
template <Concept::MemoryResource MemoryResource, Concept::Pure T, class... Args>
AXIS_NODISCARD T* MemoryNew(Args&&... args);

/// \brief Creates the array of new instances of the specified type using
///        the specified memory resource on the heap. Uses \a `DeleteArray` to
///        delete the array.
///
/// \param[in] elementCount Number of elements to allocate.
/// \param[in] args The arguments to pass to the constructor.
///
/// \return A new array of the specified type.
template <Concept::MemoryResource MemoryResource, Concept::Pure T, class... Args>
AXIS_NODISCARD T* MemoryNewArray(Size elementCount, Args&&... args);

/// \brief Deletes the instance and frees the memory, must use the
///        same memory resource type as the one used to allocate the instance.
///
/// \param[in] instance Pointer to the instance to delete.
template <Concept::MemoryResource MemoryResource, Concept::Pure T>
void MemoryDelete(const T* instance) noexcept;

/// \brief Deletes the array and frees the memory, must use the
///        same memory resource type as the one used to allocate the array.
///
/// \param[in] array Pointer to the array to delete.
template <Concept::MemoryResource MemoryResource, Concept::Pure T>
void MemoryDeleteArray(const T* array) noexcept;

/// \brief Creates a new instance of the specified type using the
///        default memory resource on the heap. Uses \a `Delete` to delete
///        the instance.
///
/// \param[in] args The arguments to pass to the constructor.
///
/// \return A new instance of the specified type.
template <Concept::Pure T, class... Args>
AXIS_NODISCARD inline T* New(Args&&... args) { return MemoryNew<MemoryResource, T, Args...>(Forward<Args>(args)...); }

/// \brief Creates the array of new instances of the specified type using
///        the default memory resource on the heap. Uses \a `DeleteArray` to
///        delete the array.
///
/// \param[in] elementCount Number of elements to allocate.
/// \param[in] args The arguments to pass to the constructor.
///
/// \return A new array of the specified type.
template <Concept::Pure T, class... Args>
AXIS_NODISCARD inline T* NewArray(Size elementCount, Args&&... args) { return MemoryNewArray<MemoryResource, T, Args...>(elementCount, Forward<Args>(args)...); }

/// \brief Deletes the instance and frees the memory
///
/// \param[in] instance Pointer to the instance to delete.
template <Concept::Pure T>
inline void Delete(const T* instance) noexcept { MemoryDelete<DefaultMemoryResource, T>(instance); }

/// \brief Deletes the array and frees the memory.
///
/// \param[in] array Pointer to the array to delete.
template <Concept::Pure T>
inline void DeleteArray(const T* array) noexcept { MemoryDeleteArray<DefaultMemoryResource, T>(array); }

} // namespace Axis::System

#include "../../Private/Axis/MemoryImpl.inl"

#endif // AXIS_SYSTEM_MEMORY_HPP
