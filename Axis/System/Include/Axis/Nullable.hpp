/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_NULLABLE_HPP
#define AXIS_SYSTEM_NULLABLE_HPP
#pragma once

#include "Trait.hpp"
#include "Utility.hpp"

namespace Axis
{

namespace System
{

/// \brief The type of the object which can be either in null state or regular state.
template <RawType T>
class Nullable
{
public:
    /// \brief Default constructor
    ///
    /// The object is in null state.
    constexpr Nullable() noexcept = default;

    /// \brief Constructs the object in null state.
    constexpr Nullable(NullptrType) noexcept;

    /// \brief Constructs the object with the given value.
    ///
    /// \param value The value to be assigned to the object.
    constexpr Nullable(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(std::is_copy_constructible_v<T>);

    /// \brief Constructs the object with the given value.
    ///
    /// \param value The value to be moved to the object.
    constexpr Nullable(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::is_nothrow_move_constructible_v<T>);

    /// \brief Copy constructor
    ///
    /// \param other Instance to be copied.
    constexpr Nullable(const Nullable& other) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(std::is_copy_constructible_v<T>);

    /// \brief Constructs the object with the given value.
    ///
    /// \param other Instance to be moved.
    constexpr Nullable(Nullable&& other) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::is_nothrow_move_constructible_v<T>);

    /// \brief Destructs the object.
    constexpr ~Nullable() noexcept;

    /// \brief Assigns the given value to the object.
    ///
    /// \param value The value to be assigned to the object.
    constexpr Nullable& operator=(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>&& std::is_nothrow_copy_assignable_v<T>) requires(std::is_copy_constructible_v<T>&& std::is_copy_assignable_v<T>);

    /// \brief Moves the given value to the object.
    ///
    /// \param value The value to be assigned to the object.
    constexpr Nullable& operator=(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_assignable_v<T>) requires(std::is_move_constructible_v<T>&& std::is_move_assignable_v<T>);

    /// \brief Copy assignment operator
    ///
    /// \param other Instance to be copied.
    constexpr Nullable& operator=(const Nullable& other) noexcept(std::is_nothrow_copy_constructible_v<T>&& std::is_nothrow_copy_assignable_v<T>) requires(std::is_copy_constructible_v<T>&& std::is_copy_assignable_v<T>);

    /// \brief Move assignment operator
    ///
    /// \param other Instance to be moved.
    constexpr Nullable& operator=(Nullable&& other) noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_assignable_v<T>) requires(std::is_move_constructible_v<T>&& std::is_move_assignable_v<T>);

    /// \brief Turns this object into null state.
    constexpr Nullable& operator=(NullptrType) noexcept;

    /// \brief Array subscript operator.
    ///
    /// \pre The object is not in null state.
    AXIS_NODISCARD constexpr T* operator->();

    /// \brief Const array subscript operator.
    ///
    /// \pre The object is not in null state.
    AXIS_NODISCARD constexpr const T* operator->() const;

    /// \brief Dereference operator
    ///
    /// \pre The object is not in null state.
    AXIS_NODISCARD constexpr T& operator*();

    /// \brief Const dereference operator
    ///
    /// \pre The object is not in null state.
    AXIS_NODISCARD constexpr const T& operator*() const;

    /// \brief Implicit conversion: Checks if the object is in null state.
    ///
    /// \return `false` if the object is in null state, `true` otherwise.
    AXIS_NODISCARD constexpr operator Bool() const noexcept;

    /// \brief Checks if the object is in null or valid state.
    ///
    /// \return `true` if the object is in valid state, `false` otherwise.
    AXIS_NODISCARD constexpr Bool IsValid() const noexcept;

    /// \brief Constructs the object with the given arguments.
    ///
    /// If the object is not in null state, the previous value is replaced by calling the destructor
    /// and then calling the constructor with the given arguments.
    ///
    /// \param args Variadic template arguments to be passed to the constructor.
    template <class... Args>
    constexpr void EmplaceConstruct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) requires(std::is_constructible_v<T, Args...>);

private:
    Bool                                 _isValid = false; ///< The flag which indicates whether the object is in null state or not.
    StaticStorage<sizeof(T), alignof(T)> _storage = {};    ///< The storage for the object.
};

} // namespace System

} // namespace Axis

#include "../../Private/Axis/NullableImpl.inl"

#endif // AXIS_SYSTEM_NULLABLE_HPP