/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ALLOCATOR_HPP
#define AXIS_SYSTEM_ALLOCATOR_HPP
#pragma once

#include "Config.hpp"
#include "Memory.hpp"
#include "Trait.hpp"

namespace Axis
{

namespace System
{

namespace Detail
{

template <class T, class = void>
struct ElementTypeImpl
{
    using Type = TypeAtTemplatedType<T, 0>;
};

template <class T>
struct ElementTypeImpl<T, std::void_t<typename T::ElementType>>
{
    using Type = typename T::ElementType;
};

template <class T, class = void>
struct DifferenceTypeImpl
{
    using Type = PointerDifference;
};

template <class T>
struct DifferenceTypeImpl<T, std::void_t<typename T::DifferenceTypes>>
{
    using Type = typename T::DifferenceType;
};

template <class T, class U, class = void>
struct RebindTypeImpl
{
    using Type = ReplaceFirstParameterInTemplatedType<U, T>;
};

template <class T, class U>
struct RebindTypeImpl<T, U, std::void_t<typename T::template Rebind<U>>>
{
    using Type = typename T::template Rebind<U>;
};

} // namespace Detail

/// \brief Provides a mechanism for accessing pointer traits for a given type;
///        also provides a default value for the pointer traits, if the pointer does not
///        provide one.
template <RawConstableType Ptr>
class PointerTraits;

/// \brief Provides a mechanism for accessing pointer traits for a given type;
///        also provides a default value for the pointer traits, if the pointer does not
///        provide one.
template <RawConstableType Ptr>
class PointerTraits final
{
public:
    /// \brief This class can't be instantiated.
    PointerTraits() = delete;

    /// \brief The type of the pointer.
    using PointerType = Ptr;

    /// \brief The dereferenced type of the pointer. If it is not defined,
    ///        the first template parameter of `Ptr` is used, if it doesn't have one,
    ///        this specialization is invalid.
    using ElementType = typename Detail::ElementTypeImpl<Ptr>::Type;

    /// \brief The difference type of the pointer (the returned value of operator-). If it is not defined,
    ///        `ptrDiff` is used instead.
    using DifferenceType = typename Detail::DifferenceTypeImpl<Ptr>::Type;

    /// \brief Rebinds to the new pointer type of given element type. If it is not defined,
    ///        the first template parameter of `Ptr` is replaced with the given element type,
    ///        if it doesn't have one, this specialization is invalid.
    template <class OtherElementType>
    using Rebind = typename Detail::RebindTypeImpl<Ptr, OtherElementType>;

    /// \brief Retrieves the pointer to the given element.
    static ElementType* GetPointer(ElementType& object) noexcept { return Ptr::GetPointer(object); }
};

/// \brief Provides a mechanism for accessing pointer traits for a given type;
///        also provides a default value for the pointer traits, if the pointer does not
///        provide one.
template <RawConstableType T>
class PointerTraits<T*> final
{
public:
    /// \brief This class can't be instantiated.
    PointerTraits() = delete;

    /// \brief The type of the pointer.
    using PointerType = T*;

    /// \brief The dereferenced type of the pointer.
    using ElementType = T;

    /// \brief The difference type of the pointer (the returned value of operator-).
    using DifferenceType = PointerDifference;

    /// \brief Rebinds to the new pointer type of given element type.
    template <class OtherElementType>
    using Rebind = OtherElementType*;

    /// \brief Retrieves the pointer from the lvalue reference
    static ElementType* GetPointer(ElementType& object) noexcept { return Axis::System::AddressOf(object); }
};

namespace Detail
{

// Checks if the allocator defines the `ValueType` type name.
template <class T>
concept AllocatorDefineValueType = requires
{
    typename T::ValueType;
}
&&RawType<T>;

// Checks if the allocator defines the `Construct` function.
template <class AllocType, class... Args>
concept AllocatorDefineConstruct = requires(AllocType                       alloc,
                                            typename AllocType::PointerType ptr,
                                            Args&&... args)
{
    {
        alloc.Construct(ptr, Forward<Args>(args)...)
        } -> IsSame<void>;
};

// Checks if the allocator defines the `Destruct` function.
template <class AllocType>
concept AllocatorDefineDestruct = requires(AllocType                       alloc,
                                           typename AllocType::PointerType ptr)
{
    {
        alloc.Destruct(ptr)
        } -> IsSame<void>;
};

// Checks if the allocator defines the `SelectOnContainerCopyConstruction` function.
template <class AllocType>
concept AllocatorDefineSelectOnContainerCopyConstruction = requires(const AllocType alloc)
{
    {
        alloc.SelectOnContainerCopyConstruction()
        } -> IsConvertible<AllocType>;
};

// Checks if the allocator type defined `Select`
template <class Allocator, class = void>
struct AllocatorPointerTypeImpl
{
    using Type = typename Allocator::ValueType*;
};

template <class Allocator>
struct AllocatorPointerTypeImpl<Allocator, std::void_t<typename Allocator::PointerType>>
{
    using Type = typename Allocator::PointerType;
};

template <class Allocator, class = void>
struct AllocatorConstPointerTypeImpl
{
    using Type = typename PointerTraits<typename AllocatorPointerTypeImpl<Allocator>::Type>::template Rebind<const typename Allocator::ValueType>;
};

template <class Allocator>
struct AllocatorConstPointerTypeImpl<Allocator, std::void_t<typename Allocator::ConstPointerType>>
{
    using Type = typename Allocator::ConstPointerType;
};

template <class Allocator, class = void>
struct AllocatorDifferenceTypeImpl
{
    using Type = typename PointerTraits<typename AllocatorPointerTypeImpl<Allocator>::Type>::DifferenceType;
};

template <class Allocator>
struct AllocatorDifferenceTypeImpl<Allocator, std::void_t<typename Allocator::DifferenceType>>
{
    using Type = typename Allocator::DifferenceType;
};

template <class Allocator, class = void>
struct AllocatorSizeTypeImpl
{
    using Type = MakeUnsigned<typename AllocatorDifferenceTypeImpl<Allocator>::Type>;
};

template <class Allocator>
struct AllocatorSizeTypeImpl<Allocator, std::void_t<typename Allocator::DifferenceType>>
{
    using Type = typename Allocator::SizeType;
};

template <class Allocator, class = void>
struct AllocatorPropagateOnContainerCopyAssignmentImpl
{
    using Type = FalseType;
};

template <class Allocator>
struct AllocatorPropagateOnContainerCopyAssignmentImpl<Allocator, std::void_t<typename Allocator::PropagateOnContainerCopyAssignment>>
{
    using Type = typename Allocator::PropagateOnContainerCopyAssignment;
};

template <class Allocator, class = void>
struct AllocatorPropagateOnContainerMoveAssignmentImpl
{
    using Type = FalseType;
};

template <class Allocator>
struct AllocatorPropagateOnContainerMoveAssignmentImpl<Allocator, std::void_t<typename Allocator::PropagateOnContainerMoveAssignment>>
{
    using Type = typename Allocator::PropagateOnContainerMoveAssignment;
};

template <class Allocator, class = void>
struct AllocatorIsAlwaysEqualImpl
{
    using Type = ConditionalType<IsEmpty<Allocator>, TrueType, FalseType>;
};

template <class Allocator>
struct AllocatorIsAlwaysEqualImpl<Allocator, std::void_t<typename Allocator::IsAlwaysEqual>>
{
    using Type = typename Allocator::IsAlwaysEqual;
};

template <class Allocator, class = void>
struct AllocatorVoidPointerTypeImpl
{
    using Type = typename PointerTraits<typename AllocatorPointerTypeImpl<Allocator>::Type>::template Rebind<void>;
};

template <class Allocator>
struct AllocatorVoidPointerTypeImpl<Allocator, std::void_t<typename Allocator::VoidPointerType>>
{
    using Type = typename Allocator::VoidPointerType;
};

template <class Allocator, class = void>
struct AllocatorConstVoidPointerTypeImpl
{
    using Type = typename PointerTraits<typename AllocatorPointerTypeImpl<Allocator>::Type>::template Rebind<const void>;
};

template <class Allocator>
struct AllocatorConstVoidPointerTypeImpl<Allocator, std::void_t<typename Allocator::ConstVoidPointerType>>
{
    using Type = typename Allocator::ConstVoidPointerType;
};

template <class Allocator, class = void>
struct NoThrowSelectOnContainerCopyConstruction
{
    static constexpr bool Value = IsNoThrowCopyConstructible<Allocator>;
};

template <class Allocator>
struct NoThrowSelectOnContainerCopyConstruction<Allocator, std::void_t<decltype(noexcept(MakeValue<Allocator>().SelectOnContainerCopyConstruction()))>>
{
    static constexpr bool Value = noexcept(MakeValue<Allocator>().SelectOnContainerCopyConstruction());
};

// Checks if the allocator requests to `PropagateOnContainerCopyAssignment` or not
template <class T>
concept AllocatorPropagateOnContainerCopyAssignment = AllocatorPropagateOnContainerCopyAssignmentImpl<T>::Type::Value && IsNoThrowCopyAssignable<T>;

// Checks if the allocator requests to `PropagateOnContainerMoveAssignment` or not
template <class T>
concept AllocatorPropagateOnContainerMoveAssignment = AllocatorPropagateOnContainerMoveAssignmentImpl<T>::Type::Value &&(IsNoThrowCopyAssignable<T> || IsNoThrowMoveAssignable<T>);

// Checks if the allocator PropagateOnContainerCopyAssignment valid.
template <class T>
concept AllocatorPropagateOnContainerCopyAssignmentValid = requires
{
    {
        AllocatorPropagateOnContainerCopyAssignmentImpl<T>::Type::Value
        } -> IsConvertible<bool>;
};

// Checks if the allocator PropagateOnContainerMoveAssignment valid.
template <class T>
concept AllocatorPropagateOnContainerMoveAssignmentValid = requires
{
    {
        AllocatorPropagateOnContainerMoveAssignmentImpl<T>::Type::Value
        } -> IsConvertible<bool>;
};

// Checks if the allocator is always equal
template <class T>
concept AllocatorIsAlwaysEqualValid = requires
{
    {
        AllocatorIsAlwaysEqualImpl<T>::Type::Value
        } -> IsConvertible<bool>;
};

template <class Allocator>
inline constexpr bool GetNoThrowSelectOnContainerCopyConstruction()
{
    if constexpr (Detail::AllocatorDefineSelectOnContainerCopyConstruction<Allocator>)
        return noexcept(MakeValue<Allocator>().SelectOnContainerCopyConstruction());
    else
        IsNoThrowCopyConstructible<Allocator>;
}

// Checks if the allocator provides `GetMaxSize` method
template <class Allocator>
concept AllocatorHasGetMaxSize = requires(const Allocator alloc)
{
    {
        alloc.GetMaxSize()
        } -> IsConvertible<typename AllocatorSizeTypeImpl<Allocator>::Type>;
}
&&noexcept(::Axis::System::MakeValue<Allocator>().GetMaxSize());

}; // namespace Detail

/// \brief Provides a mechanism for accessing allocator functions for a given type;
///        also provides a default value / function for the allocator, if the allocator does not
///        provide one.
///
/// **Allocator** is a class that provides mechanisms for allocating and deallocating memory
/// for a given type in the container. The containers contain the allocator instances to allocate
/// and deallocate memory which is used by various parts and operations of the containers. The allocator
/// instances are being aware by the containers when the containers are being moved or copied around.
template <DefaultType T>
class AllocatorTraits final
{
public:
    // Checks if the allocator defines `ValueType` typename
    static_assert(Detail::AllocatorDefineValueType<T>, "Allocator doesn't define `ValueType` typename");

    /// \brief Target type of the allocator.
    using ValueType = typename T::ValueType;

    /// \brief The type of the pointer. If the allocator doesn't define one, `ValueType*` is used.
    using PointerType = typename Detail::AllocatorPointerTypeImpl<T>::Type;

    /// \brief The type of the const pointer. If the allocator doesn't define one,
    ///        `typename PointerTraits<Pointer>::template Rebind<const ValueType>>` is used.
    using ConstPointerType = typename Detail::AllocatorConstPointerTypeImpl<T>::Type;

    /// \brief The type of the difference of the pointers, if the allocator doesn't define one,
    ///        `typename PointerTraits<Pointer>::DifferenceType` is used.
    using DifferenceType = typename Detail::AllocatorDifferenceTypeImpl<T>::Type;

    /// \brief The type of the size of the value type. If the allocator doesn't define one,
    ///        `MakeUnsigned<DifferenceType>` is used.
    using SizeType = typename Detail::AllocatorSizeTypeImpl<T>::Type;

    /// \brief Retrieves the pointer type which reference void pointer type.
    using VoidPointerType = typename Detail::AllocatorVoidPointerTypeImpl<T>::Type;

    /// \brief Retrieves the pointer type which reference const void pointer type.
    using ConstVoidPointerType = typename Detail::AllocatorConstVoidPointerTypeImpl<T>::Type;

    // Checks if the allocator `ValueType` is valid.
    static_assert(RawType<ValueType>, "The `ValueType` of the allocator must not be reference, const or array qualified.");

    // Checks if the allocator `PointerType` is valid.
    static_assert(IsConvertible<ValueType&, decltype(*(MakeValue<PointerType>()))>, "The `PointerType` can't be dereference into `ValueType&`.");

    // Checks if the allocator `ConstPointerType` is valid.
    static_assert(IsConvertible<const ValueType&, decltype(*(MakeValue<ConstPointerType>()))>, "The `ConstPointerType` can't be dereference into `const ValueType&`.");

    // Checks if the allocator `DifferenceType` is valid.
    static_assert(IsSignedIntegral<DifferenceType>, "The `DifferenceType` of the allocator must be a signed integral type.");

    // Checks if the allocator `SizeType` is valid.
    static_assert(IsUnsignedIntegral<SizeType>, "The `SizeType` of the allocator must be an unsigned integral type.");

    // Checks if the allocator `PropagateOnContainerCopyAssignment` is valid.
    static_assert(Detail::AllocatorPropagateOnContainerCopyAssignmentValid<T>, "The allocator defines `PropagateOnContainerCopyAssignment` but it's not valid (should be `TrueType` or `FalseType`).");

    // Checks if the allocator `PropagateOnContainerMoveAssignment` is valid.
    static_assert(Detail::AllocatorPropagateOnContainerMoveAssignmentValid<T>, "The allocator defines `PropagateOnContainerMoveAssignment` but it's not valid (should be `TrueType` or `FalseType`).");

    // Checks if the allocator `PropagateOnContainerCopyAssignment` is valid.
    static_assert(Detail::AllocatorPropagateOnContainerCopyAssignmentImpl<T>::Type::Value ? IsNoThrowCopyAssignable<T> : true, "The allocator defines `PropagateOnContainerCopyAssignment` but it's not valid (the allocator should be nothrow copy assignable).");

    // Checks if the allocator `PropagateOnContainerMoveAssignment` is valid.
    static_assert(Detail::AllocatorPropagateOnContainerMoveAssignmentImpl<T>::Type::Value ? (IsNoThrowCopyAssignable<T> || IsNoThrowMoveAssignable<T>) : true, "The allocator defines `PropagateOnContainerMoveAssignment` but it's not valid (the allocator should be nothrow copy or move assignable).");

    // Checks if the allocator `IsAlwaysEqual` is valid.
    static_assert(Detail::AllocatorIsAlwaysEqualValid<T>, "The allocator defines `IsAlwaysEqual` but it's not valid (should be `TrueType` or `FalseType`).");

    /// \brief Checks if the allocator requests to propagate on container move assignment.
    static constexpr bool PropagateOnContainerCopyAssignment = Detail::AllocatorPropagateOnContainerCopyAssignmentImpl<T>::Type::Value;

    /// \brief Checks if the allocator requests to propagate on container move assignment.
    static constexpr bool PropagateOnContainerMoveAssignment = Detail::AllocatorPropagateOnContainerMoveAssignmentImpl<T>::Type::Value;

    /// \brief Checks if the allocator is always equal.
    static constexpr bool IsAlwaysEqual = Detail::AllocatorIsAlwaysEqualImpl<T>::Type::Value;

    // If IsAlwaysEqual isn't true, allocator should be noexcept equal comparable.
    static_assert(IsAlwaysEqual ? true : (noexcept(MakeValue<T>() == MakeValue<T>()) && noexcept(MakeValue<T>() != MakeValue<T>())), "The allocator must be noexcept equal comparable.");

    /// \brief Checks if calls to `Construct` with the given arguments throws or not.
    template <class... Args>
    static constexpr bool NoThrowConstructible = Detail::AllocatorDefineConstruct<T, Args...> ?
        noexcept(MakeValue<T>().Construct(MakeValue<PointerType>(), MakeValue<Args>()...)) :
        IsNothrowConstructible<ValueType, Args...>;

    /// \brief Checks if calls to `SelectOnContainerCopyConstruction` throws or not.
    static constexpr bool NoThrowSelectOnContainerCopyConstruction = Detail::NoThrowSelectOnContainerCopyConstruction<T>::Value;

    /// \brief Allcoates the memory for the given number of elements.
    ///
    /// \param[in] allocator The allocator to use.
    /// \param[in] elementCount The number of elements to allocate.
    ///
    /// \returns A pointer to the allocated memory.
    AXIS_NODISCARD static constexpr PointerType Allocate(T&       allocator,
                                                         SizeType elementCount);

    /// \brief Deallocates the memory from the given pointer.
    ///
    /// \param[in] allocator The allocator to use.
    /// \param[in] pointer The pointer to deallocate.
    /// \param[in] elementCount number of elements that were allocated for the given pointer.
    static constexpr void Deallocate(T&          allocator,
                                     PointerType pointer,
                                     SizeType    elementCount) noexcept;

    /// \brief Constructs / initializes the `ValueType` at the given pointer.
    ///
    /// \param[in] allocator The allocator to use.
    /// \param[in] pointer The pointer to construct / initialize.
    /// \param[in] args The arguments to pass to the constructor.
    template <class... Args>
    static constexpr void Construct(T&          allocator,
                                    PointerType pointer,
                                    Args&&... args) noexcept(NoThrowConstructible<Args...>);

    /// \brief Destructs / destroys the `ValueType` at the given pointer.
    static constexpr void Destruct(T&          allocator,
                                   PointerType pointer) noexcept;

    /// \brief The containers will use this function instead of the default copy constructor
    ///        to copy the allocator from the source container to the destination container.
    ///        If the allocator doesn't provide one, the default copy constructor is used.
    ///
    /// \param[in] allocator The allocator to copy.
    static constexpr T SelectOnContainerCopyConstruction(const T& allocator) noexcept(NoThrowSelectOnContainerCopyConstruction);

    /// \brief Gets the maximum number of elements that can be allocated.
    ///
    /// \param[in] allocator The allocator to use.
    ///
    /// \return The maximum number of elements that can be allocated.
    static constexpr SizeType GetMaxSize(const T& allocator) noexcept;
};

// clang-format off

/// \brief Constraints for the allocator traits.
template <class T>
concept AllocatorType = requires(T                                        allocator,
                                 const T                                  constAllocator,
                                 Size                                     number,
                                 typename AllocatorTraits<T>::PointerType ptr)
{
    typename AllocatorTraits<T>::PointerType;

    {
        allocator.Allocate(number)
        } -> IsConvertible<typename AllocatorTraits<T>::PointerType>;
    {
        allocator.Deallocate(ptr, number)
        } -> IsSame<void>;
    {
        constAllocator == constAllocator
        } -> IsConvertible<bool>;
    {
        constAllocator != constAllocator
        } -> IsConvertible<bool>;
}
&&noexcept(MakeValue<T>().Deallocate(MakeValue<typename AllocatorTraits<T>::PointerType>(), MakeValue<Size>()))
&&noexcept(MakeValue<T>() == MakeValue<T>())
&&noexcept(MakeValue<T>() != MakeValue<T>());

// clang-format on

/// \brief Default memory allocator using the standard library's `new` and `delete` functions.
template <RawType T, MemoryResourceType MemoryResource = DefaultMemoryResource>
class Allocator
{
public:
    /// \brief Gets the value type
    using ValueType = T;

    /// \brief Gets the type of pointer of `T` type.
    using PointerType = T*;

    /// \brief Gets the type of const pointer of `T` type.
    using ConstPointerType = const T*;

    /// \brief Gets the size type of `T` type.
    using SizeType = Size;

    /// \brief The difference type of the pointer.
    using DifferenceType = PointerDifference;

    /// \brief Allocates the memory for the specified type `T` and returns a pointer to it.
    ///
    /// \param[in] elementCount The number of elements to allocate.
    ///
    /// \return A pointer to the allocated memory.
    constexpr PointerType Allocate(SizeType elementCount);

    /// \brief Deallocates the memory
    ///
    /// \param[in] pointer The pointer to the memory to deallocate.
    constexpr void Deallocate(PointerType pointer,
                              SizeType    elementCount) noexcept;

    /// \brief Constructs an instance of the specified type `T` at the specified pointer.
    ///
    /// \param[in] pointer The pointer to the memory to construct.
    /// \param[in] args The arguments to pass to the constructor.
    template <class... Args>
    constexpr void Construct(PointerType pointer,
                             Args&&... args) noexcept(IsNothrowConstructible<T, Args...>);

    /// \brief Destructs an instance of the specified type `T` at the specified pointer.
    ///
    /// \param[in] pointer The pointer to the memory to destruct.
    constexpr void Destruct(PointerType pointer) noexcept;

    /// \brief Gets the maximum number of elements that can be allocated.
    ///
    /// \return The maximum number of elements that can be allocated.
    constexpr SizeType GetMaxSize() const noexcept;

    /// \brief Equality operator
    ///
    /// \param[in] rhs The right hand side operand.
    ///
    /// \return `true` if the allocators are equal, `false` otherwise.
    constexpr bool operator==(const Allocator& rhs) const noexcept;

    /// \brief Inequality operator
    ///
    /// \param[in] rhs The right hand side operand.
    ///
    /// \return `true` if the allocators are not equal, `false` otherwise.
    constexpr bool operator!=(const Allocator& rhs) const noexcept;
};

} // namespace System

} // namespace Axis

#include "../../Private/Axis/AllocatorImpl.inl"

#endif // AXIS_SYSTEM_ALLOCATOR_HPP
