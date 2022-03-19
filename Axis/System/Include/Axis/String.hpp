/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_STRING_HPP
#define AXIS_SYSTEM_STRING_HPP
#pragma once

#include "../../Private/Axis/CoreContainer.inl"
#include "Allocator.hpp"

namespace Axis::System
{

namespace Concept
{

/// \brief Type of character with different size.
template <class T>
concept BasicChar = IsSame<T, Char> || IsSame<T, WChar> || IsSame<T, char8_t> || IsSame<T, char16_t> || IsSame<T, char32_t>;

} // namespace Concept

/// \brief Container contains the sequence of specified character continuously in the memory. The sequence is terminated
///        with null (0) at the end.
///
/// All the functions in this class are categorized as `strong exception guarantee`. Which means that
/// if an exception is thrown, the state of the object is rolled back to the state before the function.
///
/// \tparam T The target chatacter type to contain
/// \tparam Alloc Allocator type to use in the memory allocation
/// \tparam IteratorDebugging Specifies whether to enable iteartor debugging or not
template <Concept::BasicChar T, template <Concept::Pure> class Alloc = DefaultAllocator, Bool IteratorDebugging = Detail::CoreContainer::DefaultIteratorDebug>
class BasicString final : private ConditionalType<IteratorDebugging, Detail::CoreContainer::DebugIteratorContainer, Detail::CoreContainer::Empty>
{
public:
    using ThisType  = BasicString<T, Alloc, IteratorDebugging>; // Type to this class
    using AllocType = Alloc<T>;                                 // Type of the allocator

    // Checks if alloc type is useable
    static_assert(Concept::Allocator<AllocType>, "The given allocator type was not viable.");

    using AllocTraits      = AllocatorTraits<AllocType>;                              // Allocator traits
    using ValueType        = typename AllocTraits::ValueType;                         // Value type
    using SizeType         = typename AllocTraits::SizeType;                          // Size type
    using PointerType      = typename AllocTraits::template Pointer<ValueType>;       // pointer type
    using ConstPointerType = typename AllocTraits::template Pointer<const ValueType>; // const pointer type
    using DifferenceType   = typename PointerTraits<Pointer>::DifferenceType;         // Difference type

    // AllocatorTraits' value type must be the same as `T`
    static_assert(Concept::IsSame<ValueType, T>, "Allocator's value type mismatch");

private:
    using BaseType = ConditionalType<IteratorDebugging, Detail::CoreContainer::DebugIteratorContainer, Detail::CoreContainer::Empty>;

    // Holds dynamic memory allocation info
    struct DynamicMemoryAllocationInfo
    {
        PointerType Begin           = {};
        SizeType    AllocatedLength = {}; // The amount of the memory allocated (null-terminated is not included)
    };

    ///  Number of elements in the string that would apply the small string optimization.
    static constexpr Size SmallStringSize = sizeof(DynamicMemoryAllocationInfo) > sizeof(T) ? (Size)(sizeof(DynamicMemoryAllocationInfo) / sizeof(T)) - 1 : 0;

    struct Data
    {
        union
        {
            // Dynamic memory allocation info
            DynamicMemoryAllocationInfo MemAllocInfo;

            // Small string buffer
            T SmallStringBuffer[SmallStringSize + 1];
        };

        // The length of the string (null terminated isn't counted).
        SizeType StringLength = SizeType(0);

        // Identifies whether the current string uses small buffer or not.
        Bool IsSmallString = Bool(true);
    };

    // Checks if default constructor is nooexcept
    static constexpr Bool DefaultConstructorNoexcept = IsNothrowDefaultConstructible<AllocType>;

    // Checks if allocator copy constructor is noexcept
    static constexpr Bool AllocatorCopyConstructorNoexcept = IsNothrowCopyConstructible<AllocType>;

    // Checks if move constructor is noexcept
    static constexpr Bool MoveConstructorNoexcept = IsNothrowMoveConstructible<AllocType>;

    // Checks if move assignment is noexcept
    static constexpr Bool MoveAssignmentNoexcept = AllocTraits::Equality == AllocatorEquality::AlwaysEqual || AllocTraits::PropagateOnContainerMoveAssignment;

    // Contains both the data and the allocator
    CompressedPair<Data, AllocType> _dataAllocPair = {};

public:
    /// \brief Default constructor (empty string)
    BasicString() noexcept(DefaultConstructorNoexcept);

    /// \brief Destructor
    ~BasicString() noexcept;

    /// \brief Gets the length of the string. (null terminated isn't included)
    ///
    /// \return The length of the string
    inline SizeType GetLength() const noexcept;

private:
    // Gets the pointer to the current buffer
    PointerType GetCurrentPointer() noexcept;

    // Reserves the memory for the given string length
    template <Bool CopyContent, Bool RoundAllocation>
    PointerType ReserveMemoryFor(SizeType stringLength);

    // Tidies the container
    inline void Tidy() noexcept;
};

} // namespace Axis::System

#include "../../Private/Axis/StringImpl.inl"

#endif // AXIS_SYSTEM_STRING_HPP
