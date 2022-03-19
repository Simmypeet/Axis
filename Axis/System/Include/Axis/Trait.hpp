/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_TRAIT_HPP
#define AXIS_SYSTEM_TRAIT_HPP
#pragma once

#include "Config.hpp"
#include <type_traits>
#include <utility>

namespace Axis::System
{

namespace Concept
{

/// \brief Checks if two given types are the same.
///
/// \tparam T First type to check
/// \tparam U Second type to check
template <class T, class U>
concept IsSame = std::is_same_v<T, U>;

/// \brief Checks if the type isn't reference, const, array, or volatile qualified.
///
/// \tparam T Type to check
template <class T>
concept Pure = !std::is_reference_v<T> && !std::is_const_v<T> && !std::is_array_v<T> && !std::is_volatile_v<T>;

/// \brief Checks if the type isn't reference, array, or volatile qualified.
///
/// \tparam T Type to check
template <class T>
concept PureConstable = !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_volatile_v<T>;

/// \brief Checks if a type is convertible to the given type.
///
/// \tparam From The source type
/// \tparam To The destination type.
template <class From, class To>
concept IsConvertible = std::is_convertible_v<From, To>;

} // namespace Concept

/// \brief Gets the passed typename if the conditional is
///        true else the type is ill-formed.
template <Bool Conditional, typename T = void>
using EnableIfType = std::enable_if_t<Conditional, T>;

/// \brief Checks if the type can be noexcept constructed with the following arguments.
///
/// \tparam T Type to be constructed
/// \tparam Args Arguments to be passed to the constructor
template <Concept::Pure T, class... Args>
inline constexpr auto IsNothrowConstructible = std::is_nothrow_constructible_v<T, Args...>;

/// \brief Removes the reference out of the type.
///
/// \tparam T Type to remove the reference
template <class T>
using RemoveReference = std::remove_reference_t<T>;

/// \brief Adds the lvalue reference to the type.
///
/// \tparam T Type to add lvalue reference
template <class T>
using AddLValueReference = std::add_lvalue_reference_t<T>;

/// \brief Adds const qualification to the type
template <class T>
using AddConst = std::add_const_t<T>;

/// \brief Adds the rvalue reference to the type
///
/// \tparam T Type to add rvalue reference
template <class T>
using AddRValueReference = std::add_rvalue_reference_t<T>;

/// \brief Checks if the type is an lvalue reference qualified.
///
/// \tparam T Type to check
template <class T>
inline constexpr auto IsLvalueReference = std::is_lvalue_reference_v<T>;

/// \brief Gets the type of the first parameter if the condition is true, otherwise the type of the second parameter.
///
/// \tparam Conditional Boolean condition value
/// \tparam TrueResult Type to be referred when condition is true
/// \tparam FalseResult Type to be referred when condition is false
template <Bool Conditional, class TrueResult, class FalseResult>
using ConditionalType = std::conditional_t<Conditional, TrueResult, FalseResult>;

/// \brief Turns signed numeric types into unsigned types.
///
/// \tparam T Type to turn into unsigned integer type
template <Concept::Pure T>
using MakeUnsigned = std::make_unsigned_t<T>;

/// \brief Checks if a type is default constructible.
template <Concept::Pure T>
inline constexpr auto IsDefaultConstructible = std::is_default_constructible_v<T>;

/// \brief Checks if a type is nothrow default constructible.
template <Concept::Pure T>
inline constexpr auto IsNothrowDefaultConstructible = std::is_nothrow_default_constructible_v<T>;

/// \brief Checks if a type is copy constructible.
template <Concept::Pure T>
inline constexpr auto IsCopyConstructible = std::is_copy_constructible_v<T>;

/// \brief Checks if a type is nothrow copy constructible.
template <Concept::Pure T>
inline constexpr auto IsNothrowCopyConstructible = std::is_nothrow_copy_constructible_v<T>;

/// \brief Checks if a type is move constructible.
template <Concept::Pure T>
inline constexpr auto IsMoveConstructible = std::is_move_constructible_v<T>;

/// \brief Checks if a type is nothrow move constructible.
template <Concept::Pure T>
inline constexpr auto IsNothrowMoveConstructible = std::is_nothrow_move_constructible_v<T>;

/// \brief Checks if a type is copy assignable.
template <Concept::Pure T>
inline constexpr auto IsCopyAssignable = std::is_copy_assignable_v<T>;

/// \brief Checks if a type is nothrow copy assignable.
template <Concept::Pure T>
inline constexpr auto IsNothrowCopyAssignable = std::is_nothrow_copy_assignable_v<T>;

/// \brief Checks if a type is move assignable.
template <Concept::Pure T>
inline constexpr auto IsMoveAssignable = std::is_move_assignable_v<T>;

/// \brief Checks if a type is nothrow move assignable.
template <Concept::Pure T>
inline constexpr auto IsNothrowMoveAssignable = std::is_nothrow_move_assignable_v<T>;

/// \brief Checks if a type is integral type.
template <Concept::Pure T>
inline constexpr auto IsIntegral = std::is_integral_v<T>;

/// \brief Checks if a type is signed integral type.
template <Concept::Pure T>
inline constexpr auto IsSignedIntegral = std::is_signed_v<T>;

/// \brief Checks if a type is unsigned integral type.
template <Concept::Pure T>
inline constexpr auto IsUnsignedIntegral = std::is_unsigned_v<T>;

/// \brief Checks if a type is empty (has no members).
template <Concept::Pure T>
inline constexpr Bool IsEmpty = std::is_empty_v<T>;

/// \brief Checks if a type is pointer
template <Concept::Pure T>
inline constexpr Bool IsPointer = std::is_pointer_v<T>;

/// \brief Checks if a type is arithmetic
template <Concept::Pure T>
inline constexpr Bool IsArithmetic = std::is_arithmetic_v<T>;

/// \brief Checks if a type is constructible.
template <class T, class... Args>
inline constexpr Bool IsConstructible = std::is_constructible_v<T, Args...>;

/// \brief Checks if a type is nothrow destructible
template <class T>
inline constexpr Bool IsNothrowDestructible = std::is_nothrow_destructible_v<T>;

/// \brief Checks if the type is reference
template <class T>
inline constexpr Bool IsReference = std::is_reference_v<T>;

/// \brief Checks if the type is bounded array. (array with specified element count e.g., int[32])
template <class T>
inline constexpr Bool IsBoundedArray = std::is_bounded_array_v<T>;

/// \brief Checks if the type is unbounded array. (array with no element count specified e.g., int[])
template <class T>
inline constexpr Bool IsUnboundedArray = std::is_unbounded_array_v<T>;

/// \brief Checks if the type is volatile qualified.
template <class T>
inline constexpr Bool IsVolatile = std::is_volatile_v<T>;

/// \brief Removes const from the type
template <class T>
using RemoveConst = std::remove_const_t<T>;

/// \brief Removes all array specifiers from the type (bounded and unbounded array)
template <class T>
using RemoveAllExtents = std::remove_all_extents_t<T>;

/// \brief Checks if the type is void
template <class T>
inline constexpr Bool IsVoid = std::is_void_v<T>;

/// \brief Checks if the type can be assigned from another type
template <class To, class From>
inline constexpr Bool IsNothrowAssignable = std::is_nothrow_assignable_v<To, From>;

namespace Concept
{

/// \brief Concept for checking if a type is a callable object.
///
/// \tparam T Type to check for callable
/// \tparam ReturnType The return type of the function
/// \tparam Args Variadic arguments that are used to invoke the function
template <class T, class ReturnType, class... Args>
concept Callable = requires(T t, Args&&... args)
{
    {
        t(std::forward<Args>(args)...)
        } -> IsSame<ReturnType>;
}
&&Pure<T>&& IsNothrowCopyConstructible<T>&& IsNothrowCopyAssignable<T>;

} // namespace Concept

} // namespace Axis::System

#endif // AXIS_SYSTEM_TRAIT_HPP
