/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_SMARTPOINTERIMPL_INL
#define AXIS_SYSTEM_SMARTPOINTERIMPL_INL
#pragma once

#include "../../Include/Axis/SmartPointer.hpp"

namespace Axis
{

template <SmartPointerType T>
struct DefaultDeleter
{
    inline void operator()(std::remove_all_extents_t<T>* objectPointer) const noexcept
    {
        if constexpr (std::is_array_v<T>)
            Axis::DeleteArray<std::remove_all_extents_t<T>>(objectPointer);
        else
            Axis::Delete<std::remove_all_extents_t<T>>(objectPointer);
    }
};

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer(NullptrType) noexcept {}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer(PointerType ptr) noexcept :
    _objectPointer(ptr) {}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer(UniquePointer<T, Deleter>&& other) noexcept :
    _objectPointer(other._objectPointer)
{
    other._objectPointer = nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
template <SmartPointerType U, SmartPointerDeleterType<U> AnotherDeleter>
inline UniquePointer<T, Deleter>::UniquePointer(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U>) :
    _objectPointer(other._objectPointer)
{
    other._objectPointer = nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::~UniquePointer() noexcept
{
    if (_objectPointer != nullptr)
        Deleter{}(_objectPointer);
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>& UniquePointer<T, Deleter>::operator=(UniquePointer<T, Deleter>&& other) noexcept
{
    if (*this == std::addressof(other))
        return *this;

    if (_objectPointer != nullptr)
        Deleter{}(_objectPointer);

    _objectPointer = other._objectPointer;

    other._objectPointer = nullptr;

    return *this;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>& Axis::UniquePointer<T, Deleter>::operator=(NullptrType)
{
    Reset();
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
template <SmartPointerType U, SmartPointerDeleterType<U> AnotherDeleter>
inline UniquePointer<T, Deleter>& UniquePointer<T, Deleter>::operator=(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U>)
{
    if (*this == std::addressof(other))
        return *this;

    if (_objectPointer != nullptr)
        Deleter{}(_objectPointer);

    _objectPointer = other._objectPointer;

    other._objectPointer = nullptr;

    return *this;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
template <SmartPointerType U, typename>
inline std::add_lvalue_reference_t<std::remove_all_extents<T>> UniquePointer<T, Deleter>::operator*() const
{
    return *_objectPointer;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
template <SmartPointerType U, typename>
inline typename UniquePointer<T, Deleter>::PointerType Axis::UniquePointer<T, Deleter>::operator->() const
{
    return _objectPointer;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline std::add_lvalue_reference_t<std::remove_all_extents_t<T>> UniquePointer<T, Deleter>::operator[](Size index) const requires(std::is_array_v<T>)
{
    return _objectPointer[index];
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline Bool UniquePointer<T, Deleter>::operator==(NullptrType) const noexcept
{
    return _objectPointer == nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline Bool UniquePointer<T, Deleter>::operator!=(NullptrType) const noexcept
{
    return _objectPointer != nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::operator Bool() const noexcept
{
    return _objectPointer != nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline typename UniquePointer<T, Deleter>::PointerType UniquePointer<T, Deleter>::GetPointer() const noexcept
{
    return _objectPointer;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline void UniquePointer<T, Deleter>::Reset()
{
    Deleter{}(_objectPointer);

    _objectPointer = nullptr;
}

} // namespace Axis

#endif // AXIS_SYSTEM_SMARTPOINTERIMPL_INL