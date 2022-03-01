/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_TRAIT_HPP
#define AXIS_SYSTEM_TRAIT_HPP
#pragma once

#include "Config.hpp"
#include <type_traits>
#include <utility>

namespace Axis
{

namespace System
{

namespace Detail
{

template <typename T>
struct TypeUnwrapper; // Retrives the type of the given templated type

template <template <typename...> class C, typename... Ts>
struct TypeUnwrapper<C<Ts...>> // Retrives the type of the given templated type
{
    static constexpr Size TypeCount = sizeof...(Ts);

    template <Size N>
    using Type = typename std::tuple_element<N, std::tuple<Ts...>>::type;
};

template <class NewFirst, class TemplatedType>
struct ReplaceFirstParameterInTemplatedTypeImpl; // Replaces the first parameter in the given templated type
// e.g. TemplatedType = std::tuple<int, float, double>; NewFirst = char; -> std::tuple<char, float, double>

template <class NewFirst, template <class, class...> class TemplatedType, class First, class... Rest>
struct ReplaceFirstParameterInTemplatedTypeImpl<NewFirst, TemplatedType<First, Rest...>>
{
    using Type = TemplatedType<NewFirst, Rest...>;
};

template <Bool Condition, class TrueType, class FalseType>
struct ConditionalImpl; // Conditional type

template <class TrueType, class FalseType>
struct ConditionalImpl<true, TrueType, FalseType>
{
    using Type = TrueType;
};

template <class TrueType, class FalseType>
struct ConditionalImpl<false, TrueType, FalseType>
{
    using Type = FalseType;
};

} // namespace Detail

/// \brief Checks if two types are the same
template <class T, class U>
concept IsSame = std::is_same_v<T, U>;

/// \brief The type that doesn't have reference, const or array qualifiers.
template <class T>
concept RawType = !std::is_reference_v<T> && !std::is_const_v<T> && !std::is_array_v<T> && !std::is_volatile_v<T>;

/// \brief The type that doesn't have reference, const or array qualifiers.
template <class T>
concept RawConstableType = !std::is_reference_v<T> && !std::is_const_v<T> && !std::is_array_v<T> && !std::is_volatile_v<T>;

/// \brief The type that doesn't have reference, const, pointer or array qualifiers.
template <class T>
concept DefaultType = !std::is_reference_v<T> && !std::is_const_v<T> && !std::is_array_v<T> && !std::is_pointer_v<T> && !std::is_volatile_v<T>;

/// \brief Checks if the type can be noexcept constructed with the following arguments.
template <class T, class... Args>
inline constexpr auto IsNothrowConstructible = std::is_nothrow_constructible_v<T, Args...>;

/// \brief Removes the reference out of the type.
template <class T>
using RemoveReference = std::remove_reference_t<T>;

/// \brief Adds the lvalue reference to the type
template <class T>
using AddLValueReference = std::add_lvalue_reference_t<T>;

/// \brief Adds the rvalue reference to the type
template <class T>
using AddRValueReference = std::add_rvalue_reference_t<T>;

/// \brief Checks if the type is an lvalue reference qualified.
template <class T>
inline constexpr auto IsLvalueReference = std::is_lvalue_reference_v<T>;

/// \brief Gets the type of the first parameter if the condition is true, otherwise the type of the second parameter.
template <Bool Conditional, class TrueResult, class FalseResult>
using ConditionalType = typename Detail::ConditionalImpl<Conditional, TrueResult, FalseResult>::Type;

/// \brief Turns signed numeric types into unsigned types.
template <class T>
using MakeUnsigned = std::make_unsigned_t<T>;

/// \brief Checks if a type is default constructible.
template <RawType T>
inline constexpr auto IsDefaultConstructible = std::is_default_constructible_v<T>;

/// \brief Checks if a type is nothrow default constructible.
template <RawType T>
inline constexpr auto IsNoThrowDefaultConstructible = std::is_nothrow_default_constructible_v<T>;

/// \brief Checks if a type is copy constructible.
template <RawType T>
inline constexpr auto IsCopyConstructible = std::is_copy_constructible_v<T>;

/// \brief Checks if a type is nothrow copy constructible.
template <RawType T>
inline constexpr auto IsNoThrowCopyConstructible = std::is_nothrow_copy_constructible_v<T>;

/// \brief Checks if a type is move constructible.
template <RawType T>
inline constexpr auto IsMoveConstructible = std::is_move_constructible_v<T>;

/// \brief Checks if a type is nothrow move constructible.
template <RawType T>
inline constexpr auto IsNoThrowMoveConstructible = std::is_nothrow_move_constructible_v<T>;

/// \brief Checks if a type is copy assignable.
template <RawType T>
inline constexpr auto IsCopyAssignable = std::is_copy_assignable_v<T>;

/// \brief Checks if a type is nothrow copy assignable.
template <RawType T>
inline constexpr auto IsNoThrowCopyAssignable = std::is_nothrow_copy_assignable_v<T>;

/// \brief Checks if a type is move assignable.
template <RawType T>
inline constexpr auto IsMoveAssignable = std::is_move_assignable_v<T>;

/// \brief Checks if a type is nothrow move assignable.
template <RawType T>
inline constexpr auto IsNoThrowMoveAssignable = std::is_nothrow_move_assignable_v<T>;

/// \brief Checks if a type is integral type.
template <class T>
inline constexpr auto IsIntegral = std::is_integral_v<T>;

/// \brief Checks if a type is signed integral type.
template <class T>
inline constexpr auto IsSignedIntegral = std::is_signed_v<T>;

/// \brief Checks if a type is unsigned integral type.
template <class T>
inline constexpr auto IsUnsignedIntegral = std::is_unsigned_v<T>;

/// \brief Retrieves the type at the given index from the templated type.
template <class T, Size N>
using TypeAtTemplatedType = typename Detail::TypeUnwrapper<T>::template Type<N>;

/// \brief Replaces the first parameter in the given templated type.
template <class NewFirst, class TemplatedType>
using ReplaceFirstParameterInTemplatedType = typename Detail::ReplaceFirstParameterInTemplatedTypeImpl<NewFirst, TemplatedType>::Type;

/// \brief Type that represents true in the Boolean context.
struct TrueType
{
    static constexpr Bool Value = true;
};

/// \brief Type that represents false in the Boolean context.
struct FalseType
{
    static constexpr Bool Value = false;
};

/// \brief Checks if a type is empty (has no members).
template <class T>
inline constexpr Bool IsEmpty = std::is_empty_v<T>;

/// \brief Checks if a type is convertible to the given type.
template <class T, class U>
concept IsConvertible = std::is_convertible_v<T, U>;

/// \brief Checks if a type is pointer
template <class T>
inline constexpr Bool IsPointer = std::is_pointer_v<T>;

/// \brief Checks if a type is arithmetic
template <class T>
inline constexpr Bool IsArithmetic = std::is_arithmetic_v<T>;

/// \brief Concept for checking if a type is a callable object.
template <class T, class ReturnType, class... Args>
concept Callable = requires(T t, Args&&... args)
{
    {
        t(std::forward<Args>(args)...)
        } -> IsSame<ReturnType>;
}
&&RawType<T>&& IsNoThrowMoveConstructible<T>&& IsNoThrowCopyConstructible<T>;

/// \brief Checks if a type is POD (Plain Old Data).
template <class T>
concept PodType = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_TRAIT_HPP