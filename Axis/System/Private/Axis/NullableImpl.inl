/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE.txt', which is part of this source code package.

#ifndef AXIS_SYSTEM_NULLABLEIMPL_INL
#define AXIS_SYSTEM_NULLABLEIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/Nullable.hpp"

namespace Axis
{

template <RawType T>
inline constexpr Nullable<T>::Nullable(NullptrType) noexcept {}

template <RawType T>
inline constexpr Nullable<T>::Nullable(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(std::is_copy_constructible_v<T>) :
    _isValid(true)
{
    T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

    new (ptr) T(value);
}

template <RawType T>
inline constexpr Nullable<T>::Nullable(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::is_nothrow_move_constructible_v<T>) :
    _isValid(true)
{
    T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

    new (ptr) T(std::move(value));
}

template <RawType T>
inline constexpr Nullable<T>::Nullable(const Nullable<T>& other) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(std::is_copy_constructible_v<T>) :
    _isValid(other._isValid)
{
    if (other._isValid)
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        new (ptr) T(*other);
    }
}

template <RawType T>
inline constexpr Nullable<T>::Nullable(Nullable<T>&& other) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::is_nothrow_move_constructible_v<T>) :
    _isValid(other._isValid)
{
    if (other._isValid)
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        new (ptr) T(std::move(*other));
    }
}

template <RawType T>
inline constexpr Nullable<T>::~Nullable<T>() noexcept
{
    if (_isValid)
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        ptr->~T();
    }
}

template <RawType T>
inline constexpr Nullable<T>& Nullable<T>::operator=(const T& other) noexcept(std::is_nothrow_copy_constructible_v<T>&& std::is_nothrow_copy_assignable_v<T>) requires(std::is_copy_constructible_v<T>&& std::is_copy_assignable_v<T>)
{
    if (this == std::addressof(other))
        return *this;

    if (_isValid)
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        *ptr = *other;
    }
    else
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        new (ptr) T(*other);

        _isValid = true;
    }

    return *this;
}

template <RawType T>
inline constexpr Nullable<T>& Nullable<T>::operator=(T&& other) noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_assignable_v<T>) requires(std::is_move_constructible_v<T>&& std::is_move_assignable_v<T>)
{
    if (this == std::addressof(other))
        return *this;

    if (_isValid)
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        *ptr = std::move(*other);
    }
    else
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        new (ptr) T(std::move(*other));

        _isValid = true;
    }

    return *this;
}

template <RawType T>
inline constexpr Nullable<T>& Nullable<T>::operator=(const Nullable<T>& other) noexcept(std::is_nothrow_copy_constructible_v<T>&& std::is_nothrow_copy_assignable_v<T>) requires(std::is_copy_constructible_v<T>&& std::is_copy_assignable_v<T>)
{
    if (this == std::addressof(other))
        return *this;

    if (other._isValid)
    {
        if (_isValid)
        {
            T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

            *ptr = *other;
        }
        else
        {
            T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

            new (ptr) T(*other);

            _isValid = true;
        }
    }
    else
    {
        if (_isValid)
        {
            T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

            ptr->~T();
        }

        _isValid = false;
    }

    return *this;
}

template <RawType T>
inline constexpr Nullable<T>& Nullable<T>::operator=(Nullable<T>&& other) noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_assignable_v<T>) requires(std::is_move_constructible_v<T>&& std::is_move_assignable_v<T>)
{
    if (this == &other)
        return *this;

    if (other._isValid)
    {
        if (_isValid)
        {
            T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

            *ptr = std::move(*other);
        }
        else
        {
            T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

            new (ptr) T(std::move(*other));

            _isValid = true;
        }
    }
    else
    {
        if (_isValid)
        {
            T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

            ptr->~T();
        }

        _isValid = false;
    }

    return *this;
}

template <RawType T>
inline constexpr Nullable<T>& Nullable<T>::operator=(NullptrType) noexcept
{
    if (_isValid)
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        ptr->~T();
    }

    _isValid = false;

    return *this;
}

template <RawType T>
inline constexpr T* Nullable<T>::operator->()
{
    if (!_isValid)
        throw InvalidOperationException("Attempted to access a null value.");

    return reinterpret_cast<T*>(_storage.GetStoragePtr());
}

template <RawType T>
inline constexpr const T* Nullable<T>::operator->() const
{
    if (!_isValid)
        throw InvalidOperationException("Attempted to access a null value.");

    return reinterpret_cast<const T*>(_storage.GetStoragePtr());
}

template <RawType T>
inline constexpr T& Nullable<T>::operator*()
{
    if (!_isValid)
        throw InvalidOperationException("Attempted to access a null value.");

    return *reinterpret_cast<T*>(_storage.GetStoragePtr());
}

template <RawType T>
inline constexpr const T& Nullable<T>::operator*() const
{
    if (!_isValid)
        throw InvalidOperationException("Attempted to access a null value.");

    return *reinterpret_cast<const T*>(_storage.GetStoragePtr());
}

template <RawType T>
inline constexpr Nullable<T>::operator Bool() const noexcept
{
    return _isValid;
}

template <RawType T>
inline constexpr Bool Nullable<T>::IsValid() const noexcept
{
    return _isValid;
}

template <RawType T>
template <class... Args>
inline constexpr void Nullable<T>::EmplaceConstruct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) requires(std::is_constructible_v<T, Args...>)
{
    if (_isValid)
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        ptr->~T();

        new (ptr) T(std::forward<Args>(args)...);
    }
    else
    {
        T* ptr = reinterpret_cast<T*>(_storage.GetStoragePtr());

        new (ptr) T(std::forward<Args>(args)...);

        _isValid = true;
    }
}

} // namespace Axis

#endif // AXIS_SYSTEM_NULLABLEIMPL_INL