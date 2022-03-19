/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ALLOCATOR_HPP
#define AXIS_SYSTEM_ALLOCATOR_HPP
#pragma once

#include "Memory.hpp"
#include "Trait.hpp"
#include <limits>

namespace Axis::System
{

/// \brief Indicates how the allocator equality should be evaluated.
enum class AllocatorEquality
{
    /// \brief Two allocators of the same type are always equal.
    AlwaysEqual,

    /// \brief Two allocators of the same type are never equal.
    AlwaysNotEqual,

    /// \brief Allocator equality is determined by (==) and (!=) operators.
    Compare,
};

/// \brief The type handles memory allocation and deallocation for the given value type
///        using the provided memory resource type.
///
/// \tparam T The target value type
/// \tparam MemoryResource `MemoryResource` type used for memory allocation and deallocation
template <Concept::Pure T, Concept::MemoryResource MemoryResource>
class Allocator
{
public:
    /// \brief The pointer type that is used by the allocator.
    template <Concept::PureConstable U>
    using Pointer = U*;

    /// \brief The type of unsigned integer used to represent the count of elements.
    using SizeType = Size;

    /// \brief The type of the object to allocate.
    using ValueType = T;

    /// \brief The pointer type that points to the `ValueType`
    using ValueTypePointer = Pointer<T>;

    /// \brief Allocates memory for the given size and value type.
    ///
    /// The allocated memory is not initialized to any value; make sure to
    /// initialize the memory to a valid value by using the `Construct` function.
    ///
    /// \param[in] size The number of elements to allocate memory for.
    ///
    /// \return The pointer to the allocated memory.
    AXIS_NODISCARD ValueTypePointer Allocate(SizeType size);

    /// \brief Deallocates the memory for the given pointer.
    ///
    /// If the memory is already initialized to a valid value, the destructor
    /// won't be called here; make sure to call the `Destruct` function before
    /// deallocating the memory.
    ///
    /// \param[in] memory The pointer to the allocated memory.
    /// \param[in] size The number of elements that were allocated.
    void Deallocate(ValueTypePointer pointer,
                    SizeType         size) noexcept;

    /// \brief Constructs the element at the given pointer with the given arguments.
    ///
    /// \tparam Args The types of the arguments to pass to the constructor.
    ///
    /// \param[in] pointer The pointer to uninitialized memory to construct the element at.
    /// \param[in] args The arguments to pass to the constructor.
    template <class... Args>
    void Construct(ValueTypePointer pointer,
                   Args&&... args) noexcept(IsNothrowConstructible<T, Args...>);

    /// \brief Destructs the element at the given pointer.
    ///
    /// \param[in] pointer The pointer to the element to destruct.
    void Destruct(ValueTypePointer pointer) noexcept;

    /// \brief Don't propagate the allocator when the container is copy assigned.
    static constexpr Bool PropagateOnContainerCopyAssignment = false;

    /// \brief Don't propagate the allocator when the container is move assigned.
    static constexpr Bool PropagateOnContainerMoveAssignment = false;

    /// \brief This allocator is always compare equal to another allocator of the same type.
    static constexpr AllocatorEquality Equality = AllocatorEquality::AlwaysEqual;

    /// \brief The maximum allowed element count that can be allocated at once.
    static constexpr Size MaxAllocationSize = std::numeric_limits<Size>::max() / sizeof(ValueType);

    /// \brief Equality comparison operator
    ///
    /// \param[in] other The other allocator to compare with.
    ///
    /// \return Always returns true.
    constexpr Bool operator==(const Allocator&) const noexcept { return true; }

    /// \brief Inequality comparison operator
    ///
    /// \param[in] other The other allocator to compare with.
    ///
    /// \return Always returns false.
    constexpr Bool operator!=(const Allocator&) const noexcept { return false; }
};

/// \brief Axis's default allocator type
template <Concept::Pure T>
using DefaultAllocator = Allocator<T, DefaultMemoryResource>;

namespace Concept
{

// clang-format off

/// \brief Allocator type that is used for memory allocation and deallocation for
///        the given value type. The allocator will be used in containers to provide
///        all sorts of memory allocation and deallocation functionality.
///
/// To satisfy the concept, the following requirements must be satisfied:
///     - Has a `PointerType` template that is a pointer type that is used by the allocator.
///     - Has a `SizeType` defined that is an unsigned integer type used to represent the count of elements.
///     - Has a `ValueType` defined that is the type of the object to allocate.
///     - Has a `PointerType<T> Allocate(SizeType size)` member function that allocates memory for the given size and value type.
///     - Has a `void Deallocate(Pointer<T> pointer, SizeType size)` member function that deallocates the memory for the given pointer.
///     - Has a `MaxAllocationSize` constexpr static member that is the maximum allowed element count that can be allocated at once.
///
/// \tparam T The allocator type
template <class T>
concept Allocator = requires(T                                                       allocator,
                             typename T::SizeType                                    size,
                             typename T::ValueType                                   value,
                             typename T::template Pointer<typename T::ValueType> pointer)
{
    {
        allocator.Allocate(size)
        } -> IsConvertible<typename T::template Pointer<typename T::ValueType>>;
    {
        T::MaxAllocationSize
        } -> IsConvertible<typename T::SizeType>;
}
&& Pure<T>
&& Pure<typename T::SizeType>
&& Pure<typename T::ValueType>
&& Pure<typename T::template Pointer<typename T::ValueType>>
&& Pure<typename T::template Pointer<const typename T::ValueType>>
&& noexcept(MakeValue<T&>().Deallocate(MakeValue<typename T::template Pointer<typename T::ValueType>>(), MakeValue<typename T::SizeType>()))
&& IsUnsignedIntegral<typename T::SizeType>;

// clang-format on

} // namespace Concept

namespace Detail::Allocator
{

template <Concept::Allocator>
struct AllocatorEqualityValue; // Gets `AllocatorEquality` out of Alloc

template <Concept::Allocator, class...>
struct AllocatorConstructNoexceptValue; // Checks whether call to `Construct` with this following arguments throws or not

template <Concept::Allocator>
struct AllocatorPropagateOnContainerCopyAssignmentValue; // Gets the `PropagateOnContainerCopyAssignment` value

template <Concept::Allocator>
struct AllocatorPropagateOnContainerMoveAssignmentValue; // Gets the `PropagateOnContainerMoveAssignment` value

template <Concept::Allocator>
struct AllocatorSelectOnContainerCopyConstructorNoexceptValue; // Checks whether call to `SelectOnContainerCopyConstructor` noexcept or not

} // namespace Detail::Allocator

/// \brief Provides the uniform way to access the allocator's functionalities
///        and traits. It also provides the default traits and functions in case that allocator
///        missing them.
///
/// \tparam Alloc Allocator to be used
template <Concept::Allocator Alloc>
class AllocatorTraits
{
public:
    /// \brief The pointer type that is used by the allocator.
    template <Concept::PureConstable U>
    using Pointer = typename Alloc::template Pointer<U>;

    /// \brief The type of the object to allocate
    using ValueType = typename Alloc::ValueType;

    /// \brief The type of unsigned integer used to represent the count of elements.
    using SizeType = typename Alloc::SizeType;

    /// \brief The pointer type that points to the `ValueType`
    using ValueTypePointer = Pointer<ValueType>;

    // Checks if the templated pointer type is viable
    static_assert(Concept::PointerTemplate<Pointer, ValueType>, "The given pointer type was unusable with the given value type.");

    /// \brief Allocates memory for the given size and value type.
    ///
    /// The allocated memory is not initialized to any value; make sure to
    /// initialize the memory to a valid value by using the `Construct` function.
    ///
    /// \param[in] allocator The allocator to be used.
    /// \param[in] size The number of elements to allocate memory for.
    ///
    /// \return The pointer to the allocated memory.
    AXIS_NODISCARD static ValueTypePointer Allocate(Alloc&   allocator,
                                                    SizeType size);

    /// \brief Deallocates the memory for the given pointer.
    ///
    /// If the memory is already initialized to a valid value, the destructor
    /// won't be called here; make sure to call the `Destruct` function before
    /// deallocating the memory.
    ///
    /// \param[in] allocator The allocator to be used.
    /// \param[in] memory The pointer to the allocated memory.
    /// \param[in] size The number of elements that were allocated.
    static void Deallocate(Alloc&           allocator,
                           ValueTypePointer pointer,
                           SizeType         size) noexcept;

    /// \brief Constructs the element at the given pointer with the given arguments.
    ///
    /// \tparam Args The types of the arguments to pass to the constructor.
    ///
    /// \param[in] allocator The allocator to be used.
    /// \param[in] pointer The pointer to uninitialized memory to construct the element at.
    /// \param[in] args The arguments to pass to the constructor.
    template <class... Args>
    static void Construct(Alloc&           allocator,
                          ValueTypePointer pointer,
                          Args&&... args) noexcept(Detail::Allocator::AllocatorConstructNoexceptValue<Alloc, Args...>::Value);

    /// \brief Destructs the element at the given pointer.
    ///
    /// \param[in] allocator The allocator to be used.
    /// \param[in] pointer The pointer to the element to destruct.
    static void Destruct(Alloc&           allocator,
                         ValueTypePointer pointer) noexcept;

    /// \brief The maximum allowed element count that can be allocated at once.
    static constexpr SizeType MaxAllocationSize = Alloc::MaxAllocationSize;

    /// \brief Inidicates how the allocator equality should be evaluated.
    static constexpr AllocatorEquality Equality = Detail::Allocator::AllocatorEqualityValue<Alloc>::Value;

    /// \brief Inidicates whether the allocator should be propagated when the container copy assigns.
    static constexpr Bool PropagateOnContainerCopyAssignment = Detail::Allocator::AllocatorPropagateOnContainerCopyAssignmentValue<Alloc>::Value;

    /// \brief Inidicates whether the allocator should be propagated when the container move assigns.
    static constexpr Bool PropagateOnContainerMoveAssignment = Detail::Allocator::AllocatorPropagateOnContainerMoveAssignmentValue<Alloc>::Value;

    /// \brief Compares if two allocators are equal.
    ///
    /// \param[in] lhs First allocator
    /// \param[in] rhs Second allocator
    ///
    /// \return Returns true if two allocators are equal; it means that the resources that are occupied by both
    ///         allocators are interchangable.
    AXIS_NODISCARD static Bool CompareEqual(const Alloc& lhs,
                                            const Alloc& rhs) noexcept;

    /// \brief Compares if two allocators are not equal.
    ///
    /// \param[in] lhs First allocator
    /// \param[in] rhs Second allocator
    ///
    /// \return Returns true if two allocators are not equal.
    AXIS_NODISCARD static Bool CompareUneqaul(const Alloc& lhs,
                                              const Alloc& rhs) noexcept;

    /// \brief Gets the copy of the allocator.
    ///
    /// This function is used instead to gets the copy of the allocator when
    /// the container is copy constructed, as opposed to allocator's copy constructor.
    ///
    /// \param[in] alloc Allocator to get the copy of from.
    ///
    /// \return The copy of the allocator
    AXIS_NODISCARD static Alloc SelectOnContainerCopyConstructor(const Alloc& alloc) noexcept(Detail::Allocator::AllocatorSelectOnContainerCopyConstructorNoexceptValue<Alloc>::Value);
};

} // namespace Axis::System

#include "../../Private/Axis/AllocatorImpl.inl"

#endif // AXIS_SYSTEM_ALLOCATOR_HPP
