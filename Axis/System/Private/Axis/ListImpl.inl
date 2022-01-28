/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ARRAYIMPL_INL
#define AXIS_SYSTEM_ARRAYIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/List.hpp"
#include "../../Include/Axis/Math.hpp"

namespace Axis
{

template <RawType T, AllocatorType Allocator>
template <Bool FreeMemory>
inline void List<T, Allocator>::ClearInternal(T*   buffer,
                                              Size length)
{
    if (buffer)
    {
        for (Size i = 0; i < length; i++)
            buffer[i].~T();

        if constexpr (FreeMemory)
            Allocator::Deallocate(buffer);
    }
}

template <RawType T, AllocatorType Allocator>
inline List<T, Allocator>::List(const List<T, Allocator>& other) requires(std::is_copy_constructible_v<T>)
{
    if (other._length > 0)
    {
        auto newMemory   = ConstructsNewList<true, true, true>(other._length, Math::RoundToNextPowerOfTwo(other._length), other._buffer);
        _allocatedLength = GetTuple<1>(newMemory);
        _length          = other._length;
        _buffer          = GetTuple<0>(newMemory);
    }
}

template <RawType T, AllocatorType Allocator>
inline List<T, Allocator>::List(List<T, Allocator>&& other) noexcept :
    _buffer(other._buffer),
    _allocatedLength(other._length),
    _length(other._length)
{
    // Turns other into an empty array.
    other._buffer          = nullptr;
    other._length          = 0;
    other._allocatedLength = 0;
}

template <RawType T, AllocatorType Allocator>
inline List<T, Allocator>::List(NullptrType) noexcept {}

template <RawType T, AllocatorType Allocator>
template <class... Args>
inline List<T, Allocator>::List(Size length, Args... args) requires(std::is_constructible_v<T, Args...>)
{
    if (length > 0)
    {
        auto newMemory   = ConstructsNewList<true, false, false>(length, Math::RoundToNextPowerOfTwo(length), nullptr, std::forward<Args>(args)...);
        _allocatedLength = GetTuple<1>(newMemory);
        _length          = length;
        _buffer          = GetTuple<0>(newMemory);
    }
}

template <RawType T, AllocatorType Allocator>
inline List<T, Allocator>::List(const std::initializer_list<T>& other) requires(std::is_copy_constructible_v<T>)
{
    if (other.size() > 0)
    {
        auto newMemory   = ConstructsNewList<true, true, true>(other.size(), Math::RoundToNextPowerOfTwo(other.size()), const_cast<T*>(std::addressof(*other.begin())));
        _allocatedLength = GetTuple<1>(newMemory);
        _length          = other.size();
        _buffer          = GetTuple<0>(newMemory);
    }
}

template <RawType T, AllocatorType Allocator>
inline List<T, Allocator>::~List() noexcept
{
    ClearInternal<true>(_buffer, _length);
}

template <RawType T, AllocatorType Allocator>
inline List<T, Allocator>& List<T, Allocator>::operator=(const List<T, Allocator>& other) requires(std::is_copy_constructible_v<T>)
{
    if (this == std::addressof(other))
        return *this;

    if constexpr (std::is_nothrow_copy_constructible_v<T>)
    {
        // Clears the current list
        if (other._length > _allocatedLength)
        {
            auto allocatedLength = Math::RoundToNextPowerOfTwo(other._length);

            ClearInternal<true>(_buffer, _length);

            _buffer          = (T*)Allocator::Allocate(allocatedLength, alignof(T));
            _allocatedLength = allocatedLength;
        }
        else
            ClearInternal<false>(_buffer, _length);

        _length = other._length;

        for (Size i = 0; i < _length; ++i)
            new (&_buffer[i]) T(other._buffer[i]);
    }
    else
    {
        // Clears the current list
        ClearInternal<true>(_buffer, _length);

        // Creates new buffer to provide strong exception guarantee
        auto newMemory = ConstructsNewList<false, true, true>(other._length, Math::RoundToNextPowerOfTwo(other._length), const_cast<T*>(other._buffer));

        _allocatedLength = GetTuple<1>(newMemory);
        _length          = other._length;
        _buffer          = GetTuple<0>(newMemory);
    }

    return *this;
}

template <RawType T, AllocatorType Allocator>
inline List<T, Allocator>& List<T, Allocator>::operator=(List<T, Allocator>&& other) noexcept
{
    if (this == std::addressof(other))
        return *this;

    ClearInternal<true>(_buffer, _length);

    _buffer          = other._buffer;
    _length          = other._length;
    _allocatedLength = other._allocatedLength;

    // Turns other into an empty array.
    other._buffer          = nullptr;
    other._length          = 0;
    other._allocatedLength = 0;

    return *this;
}

template <RawType T, AllocatorType Allocator>
inline Size List<T, Allocator>::GetLength() const noexcept
{
    return _length;
}

template <RawType T, AllocatorType Allocator>
inline void List<T, Allocator>::ReserveFor(Size length)
{
    if (length > _allocatedLength)
    {
        auto newMemory = ConstructsNewList<false, true, !std::is_nothrow_move_constructible_v<T>>(_length, Math::RoundToNextPowerOfTwo(length), _buffer);

        // Destructs the old array.
        ClearInternal<true>(_buffer, _length);

        _allocatedLength = GetTuple<1>(newMemory);
        _buffer          = GetTuple<0>(newMemory);
    }
}

template <RawType T, AllocatorType Allocator>
inline void List<T, Allocator>::Clear() noexcept
{
    ClearInternal<false>(_buffer, _length);

    _length = 0;
}

template <RawType T, AllocatorType Allocator>
inline void List<T, Allocator>::Reset() noexcept(std::is_nothrow_default_constructible_v<T>) requires(std::is_default_constructible_v<T>)
{
    if constexpr (std::is_nothrow_default_constructible_v<T>)
    {
        ClearInternal<false>(_buffer, _length);

        // Invokes default constructor using placement new.
        for (Size i = 0; i < _length; i++)
            new (_buffer + i) T();
    }
    else
    {
        // Allocates new memory to ensure strong exception safety.
        auto newMemory = ConstructsNewList<false, false, false>(_length, Math::RoundToNextPowerOfTwo(_length), nullptr);

        // Destructs the old array.
        ClearInternal<true>(_buffer, _length);

        _allocatedLength = GetTuple<1>(newMemory);
        _buffer          = GetTuple<0>(newMemory);
    }
}

template <RawType T, AllocatorType Allocator>
template <class... Args>
inline T* List<T, Allocator>::EmplaceBack(Args... args) requires(std::is_constructible_v<T, Args...> && (std::is_copy_constructible_v<T> || std::is_nothrow_move_constructible_v<T>))
{
    if (_length == _allocatedLength)
    {
        auto newMemory = ConstructsNewList<false, true, !std::is_nothrow_move_constructible_v<T>>(_length, Math::RoundToNextPowerOfTwo(_length + 1), _buffer);

        // Destructs the old array.
        ClearInternal<true>(_buffer, _length);

        _allocatedLength = GetTuple<1>(newMemory);
        _buffer          = GetTuple<0>(newMemory);
    }

    // Invokes new placement constructor
    new (_buffer + _length) T(std::forward<Args>(args)...);

    _length++;

    return _buffer + _length;
}

template <RawType T, AllocatorType Allocator>
inline T* List<T, Allocator>::Append(const T& element) requires(std::is_copy_constructible_v<T>)
{
    return EmplaceBack<const T&>(element);
}

template <RawType T, AllocatorType Allocator>
inline T* List<T, Allocator>::Append(T&& element) requires(std::is_move_constructible_v<T>)
{
    return EmplaceBack<T&&>(std::move(element));
}

template <RawType T, AllocatorType Allocator>
template <class... Args>
inline T* List<T, Allocator>::Emplace(Size index, Args... args) requires(std::is_constructible_v<T, Args...> && (std::is_copy_constructible_v<T> || std::is_nothrow_move_constructible_v<T>))
{
    if (index > _length)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    // Checks if the index is the end.
    if (index == _length) return EmplaceBack(std::forward<Args>(args)...);

    if (_length < _allocatedLength && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>)&&std::is_nothrow_constructible_v<T, Args...>)
    {
        // Moves the elements to the right of the index.
        for (Size i = _length; i > index; i--)
        {
            // Uses any constructor with noexcept.
            if constexpr (std::is_nothrow_move_constructible_v<T>)
                new (_buffer + i) T(std::move(_buffer[i - 1]));
            else
                new (_buffer + i) T(_buffer[i - 1]);

            // Destructs the old element.
            _buffer[i - 1].~T();
        }

        // Invokes placement new at the index.
        new (_buffer + index) T(std::forward<Args>(args)...);

        return _buffer + index;
    }
    else
    {
        auto newAllocatedLength = Math::RoundToNextPowerOfTwo(_length + 1);

        // Allocates new memory.
        auto newMemory = (T*)Allocator::Allocate(newAllocatedLength * sizeof(T), alignof(T));

        // Copies the elements to the new memory and creates a space for the new element.
        for (Size i = 0; i < index; i++)
        {
            // try to use move constructor if it is nothrow.
            if constexpr (std::is_nothrow_move_constructible_v<T>)
                new (newMemory + i) T(std::move(_buffer[i]));
            else
            {
                // if copy constructor isn't noexcept, wraps it with try-catch.

                if constexpr (std::is_nothrow_copy_constructible_v<T>)
                    new (newMemory + i) T(_buffer[i]);
                else
                {
                    try
                    {
                        new (newMemory + i) T(_buffer[i]);
                    }
                    catch (...)
                    {
                        // Destructs the new memory.
                        ClearInternal<true>(newMemory, i + 1);

                        // Re-throws the exception.
                        throw;
                    }
                }
            }
        }

        // Invokes new placement constructor at the index.
        new (newMemory + index) T(std::forward<Args>(args)...);

        // Continue copying the elements to the new memory.
        for (Size i = index; i < _length; i++)
        {
            // try to use move constructor if it is nothrow.
            if constexpr (std::is_nothrow_move_constructible_v<T>)
                new (newMemory + i + 1) T(std::move(_buffer[i]));
            else
            {
                // if copy constructor isn't noexcept, wraps it with try-catch.

                if constexpr (std::is_nothrow_copy_constructible_v<T>)
                    new (newMemory + i + 1) T(_buffer[i]);
                else
                {
                    try
                    {
                        new (newMemory + i + 1) T(_buffer[i]);
                    }
                    catch (...)
                    {
                        // Destructs the new memory.
                        ClearInternal<true>(newMemory, i + 2);

                        // Re-throws the exception.
                        throw;
                    }
                }
            }
        }

        // The old memory is destructed.
        ClearInternal<true>(_buffer, _length);

        // Assigns the new memory.
        _buffer          = newMemory;
        _allocatedLength = newAllocatedLength;

        // Increases the length.
        _length++;

        return _buffer + index;
    }
}

template <RawType T, AllocatorType Allocator>
inline void List<T, Allocator>::PopBack() noexcept
{
    // Checks if the array is empty, if so, does nothing.
    if (_length == 0)
        return;

    // Destructs the last element.
    _buffer[_length - 1].~T();

    // Decreases the length.
    _length--;
}

template <RawType T, AllocatorType Allocator>
inline void List<T, Allocator>::RemoveAt(Size index) requires(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>)
{
    // Checks if the index is valid.
    if (index >= _length)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    // No need to allocate new memory if
    if constexpr (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>)
    {
        // Destruct the element at the index.
        _buffer[index].~T();

        // Moves the elements to the left of the index.
        for (Size i = index; i < _length - 1; i++)
        {
            // Uses any constructor with noexcept.
            if constexpr (std::is_nothrow_move_constructible_v<T>)
                new (_buffer + i) T(std::move(_buffer[i + 1]));
            else
                new (_buffer + i) T(_buffer[i + 1]);

            // Destructs the old element.
            _buffer[i + 1].~T();
        }
    }
    else
    {
        // Creates new buffer to ensure strong exception safety.
        auto newBuffer = (T*)Allocator::Allocate((_length - 1) * sizeof(T), alignof(T));

        // Keeps track of allocated elements in case of exception.
        Size allocated = 0;

        // Copies the elements to the new buffer.
        try
        {
            for (Size i = 0; i < index; i++)
            {
                new (newBuffer + i) T(_buffer[i]);
                allocated++;
            }

            for (Size i = index + 1; i < _length; i++)
            {
                new (newBuffer + i - 1) T(_buffer[i]);
                allocated++;
            }
        }
        catch (...)
        {
            // Destructs the new memory.
            ClearInternal<true>(newBuffer, allocated);

            // Re-throws the exception.
            throw;
        }

        // The old memory is destructed.
        ClearInternal<true>(_buffer, _length);

        // Assigns the new memory.
        _buffer = newBuffer;
    }

    // Decreases the length.
    _length--;
}

template <RawType T, AllocatorType Allocator>
inline T* List<T, Allocator>::GetData() noexcept
{
    return _buffer;
}

template <RawType T, AllocatorType Allocator>
inline const T* List<T, Allocator>::GetData() const noexcept
{
    return _buffer;
}

template <RawType T, AllocatorType Allocator>
inline void List<T, Allocator>::Resize(Size length) requires(std::is_default_constructible_v<T>)
{
    if (length <= _allocatedLength && std::is_nothrow_default_constructible_v<T>)
    {
        for (Size i = 0; i < _length; i++)
            _buffer[i].~T();

        _length = length;

        for (Size i = 0; i < _length; i++)
            new (_buffer + i) T();
    }
    else
    {
        // Creates new buffer to ensure strong exception safety.
        auto newMemory = ConstructsNewList<true, false, false>(length,
                                                               Math::RoundToNextPowerOfTwo(length),
                                                               nullptr);

        // Destroys the old buffer.
        ClearInternal<true>(_buffer, _length);

        // Assigns the new buffer.
        _allocatedLength = GetTuple<1>(newMemory);
        _buffer          = GetTuple<0>(newMemory);
        _length          = length;
    }
}

template <RawType T, AllocatorType Allocator>
inline T& List<T, Allocator>::operator[](Size index)
{
    // Checks if the index is valid.
    if (index >= _length)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return _buffer[index];
}

template <RawType T, AllocatorType Allocator>
inline const T& List<T, Allocator>::operator[](Size index) const
{
    // Checks if the index is valid.
    if (index >= _length)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return _buffer[index];
}

template <RawType T, AllocatorType Allocator>
inline T* List<T, Allocator>::begin() noexcept { return _buffer; }

template <RawType T, AllocatorType Allocator>
inline const T* List<T, Allocator>::begin() const noexcept { return _buffer; }

template <RawType T, AllocatorType Allocator>
inline T* List<T, Allocator>::end() noexcept { return _buffer + _length; }

template <RawType T, AllocatorType Allocator>
inline const T* List<T, Allocator>::end() const noexcept { return _buffer + _length; }

template <RawType T, AllocatorType Allocator>
template <Bool CleanWhenThrow, Bool ListInitialize, Bool CopyConstructor, class... Args>
inline Tuple<T*, Size> List<T, Allocator>::ConstructsNewList(Size elementCount,
                                                             Size allocationSize,
                                                             T*   begin,
                                                             Args&&... args)
{
    if (elementCount == 0 && allocationSize == 0)
        return {nullptr, (Size)0};

    // Allocates memory for the array.
    T* array = (T*)Allocator::Allocate(sizeof(T) * allocationSize, alignof(T));

    // Initializes the array.
    // If T can be constructed with noexcept, then we don't need to check for exceptions.
    // Otherwise, we need to check for exceptions.
    constexpr Bool NoException = ListInitialize ? (CopyConstructor ? std::is_nothrow_copy_constructible_v<T> : std::is_nothrow_move_constructible_v<T>) : std::is_nothrow_constructible_v<T, Args...>;

    if constexpr (NoException)
    {
        for (Size i = 0; i < elementCount; i++)
        {
            if constexpr (ListInitialize)
            {
                if constexpr (CopyConstructor)
                    new (array + i) T(begin[i]);
                else
                    new (array + i) T(std::move(begin[i]));
            }
            else
                new (array + i) T(std::forward<Args>(args)...);
        }
    }
    else
    {
        // Keeps track of the number of constructed elements.
        Size constructedCount = 0;
        try
        {
            for (Size i = 0; i < elementCount; i++)
            {
                if constexpr (ListInitialize)
                {
                    if constexpr (CopyConstructor)
                        new (array + i) T(begin[i]);
                    else
                        new (array + i) T(std::move(begin[i]));
                }
                else
                    new (array + i) T(std::forward<Args>(args)...);
            }
        }
        catch (...)
        {
            // Destructs the already constructed elements.
            for (Size i = 0; i < constructedCount; i++)
                array[i].~T();

            ClearInternal<CleanWhenThrow>(array, constructedCount);

            throw;
        }
    }

    // Done constructing the array.
    return {array, (Size)allocationSize};
}

template <RawType T, AllocatorType Allocator>
inline List<T, Allocator>::operator Bool() const noexcept
{
    return _length > 0;
}

} // namespace Axis

#endif // AXIS_SYSTEM_ARRAYIMPL_INL