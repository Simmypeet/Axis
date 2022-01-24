/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE.txt', which is part of this source code package.

/// \file List.hpp
///
/// \brief Contains the definition of `Axis::List` template container class.

#ifndef AXIS_SYSTEM_LIST_HPP
#define AXIS_SYSTEM_LIST_HPP
#pragma once

#include "Memory.hpp"
#include "Trait.hpp"
#include "Utility.hpp"

namespace Axis
{

/// \brief Template array container class which can contain multiple elements
///        in a single container.
///
/// The array is dynamic which, can be resized to any specified size.
///
/// All the functions in this class are categorized as `strong exception guarantee`. Which means that
/// if an exception is thrown, the state of the object is rolled back to the state before the function.
template <RawType T, AllocatorType Allocator = DefaultAllocator>
class List
{
public:
    /// \brief Default constructor.
    ///
    /// The list is initialized with a size of 0 and no memory is allocated.
    List() noexcept = default;

    /// \brief Copy constructor.
    ///
    /// \param other List to copy from.
    List(const List& other) requires(std::is_copy_constructible_v<T>);

    /// \brief Move constructor.
    ///
    /// \param other List to move from.
    List(List&& other) noexcept;

    /// \brief nullptr constructor.
    ///
    /// The list is initialized with a size of 0 and no memory is allocated.
    List(NullptrType) noexcept;

    /// \brief Initializer constructor
    ///
    /// Initializes the list with the specified length,
    /// and initializes the  elements with the specified value.
    ///
    /// \param[in] length The length of the list.
    /// \param[in] args Arguments to forward to the constructor of the object to insert.
    template <class... Args>
    List(Size length, Args... args) requires(std::is_constructible_v<T, Args...>);

    /// \brief Initializer list constructor.
    ///
    /// \param[in] init Initializer list to copy from.
    List(const std::initializer_list<T>& init) requires(std::is_copy_constructible_v<T>);

    /// \brief Destructor.
    ~List() noexcept;

    /// \brief Copy assignment operator.
    ///
    /// \param[in] other List to copy from.
    List& operator=(const List& other) requires(std::is_copy_constructible_v<T>);

    /// \brief Move assignment operator.
    ///
    /// \param[in] other List to move from.
    List& operator=(List&& other) noexcept;

    /// \brief Returns the length of the list.
    ///
    /// \return The length of the list.
    AXIS_NODISCARD Size GetLength() const noexcept;

    /// \brief Reserves memory for the list to the specified length.
    ///
    /// If the list's memory allocated length is already larger than the specified length,
    /// the list is not resized.
    ///
    /// \param[in] length The length of the elements to reserve.
    void ReserveFor(Size length);

    /// \brief Clears the list.
    ///
    /// - Invokes the destructor on all the elements in the list.
    /// - The list is then resized to 0. The memory is not deallocated.
    void Clear() noexcept;

    /// \brief Invokes the destructor followed by default constructor on all the elements in the list.
    void Reset() noexcept(std::is_nothrow_default_constructible_v<T>) requires(std::is_default_constructible_v<T>);

    /// \brief Constructs an element at the end of the list.
    ///
    /// - The list is expanded to accommodate the new element.
    /// - New memory might be allocated if necessary and might invalidate iterators.
    ///
    /// \param[in] args Arguments to forward to the constructor of the object to insert.
    ///
    /// \returns An iterator to the newly constructed element.
    template <class... Args>
    T* EmplaceBack(Args... args) requires(std::is_constructible_v<T, Args...> && (std::is_copy_constructible_v<T> || std::is_nothrow_move_constructible_v<T>));

    /// \brief Appends an element at the end of the list.
    ///
    /// - The list is expanded to accommodate the new element.
    /// - New memory might be allocated if necessary and might invalidate iterators.
    ///
    /// \param[in] element The element to append.
    ///
    /// \returns An iterator to the newly constructed element.
    T* Append(const T& element) requires(std::is_copy_constructible_v<T>);

    /// \brief Appends an element at the end of the list.
    ///
    /// - The list is expanded to accommodate the new element.
    /// - New memory might be allocated if necessary and might invalidate iterators.
    ///
    /// \param[in] element The element to append.
    ///
    /// \returns An iterator to the newly constructed element.
    T* Append(T&& element) requires(std::is_move_constructible_v<T>);

    /// \brief Constructs an element at the end of the list.
    ///
    /// An insertion pushes all the elements after the insertion point to the right.
    ///
    /// - The list is expanded to accommodate the new element.
    /// - New memory might be allocated if necessary and might invalidate iterators.
    ///
    /// \param[in] index The index of the element to insert.
    /// \param[in] args Arguments to forward to the constructor of the object to insert.
    ///
    /// \pre The index must be less than or equal ot the list's length, otherwise the program will terminate.
    ///
    /// \remark If the index is equal to the list's length, the function calls \a `EmplaceBack` instead.
    ///
    /// \return An iterator to the inserted element.
    template <class... Args>
    T* Emplace(Size index, Args... args) requires(std::is_constructible_v<T, Args...> && (std::is_copy_constructible_v<T> || std::is_nothrow_move_constructible_v<T>));

    /// \brief Removes the element at the end of the list.
    ///
    /// - The list memory is not deallocated. The element is only destroyed.
    /// - The list is shrunk to accommodate the removed element.
    void PopBack() noexcept;

    /// \brief Removes the element at the specified index.
    ///
    /// - The list memory is not deallocated. The element is only destroyed.
    /// - The list is shrunk to accommodate the removed element.
    ///
    /// \pre The index must be less than or equal ot the list's length, otherwise the program will terminate.
    ///
    /// \param[in] index The index of the element to remove.
    void RemoveAt(Size index) requires(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>);

    /// \brief Gets the pointer to the first element of the list.
    AXIS_NODISCARD T* GetData() noexcept;

    /// \brief Gets the pointer to the first element of the list.
    AXIS_NODISCARD const T* GetData() const noexcept;

    /// \brief Resizes the list with the specified length. The object is constructed via default constructor.
    void Resize(Size length) requires(std::is_default_constructible_v<T>);

    /// \brief Index operator
    ///
    /// \param[in] index The index of the element to access.
    ///
    /// \returns A reference to the element at the specified index.
    AXIS_NODISCARD T& operator[](Size index);

    /// \brief Constant index operator
    ///
    /// \param[in] index The index of the element to access.
    ///
    /// \returns A reference to the element at the specified index.
    AXIS_NODISCARD const T& operator[](Size index) const;

    /// \brief Iterator to the beginning of the list.
    ///
    /// \returns An iterator to the beginning of the list.
    AXIS_NODISCARD T* begin() noexcept;

    /// \brief Iterator to the beginning of the list.
    ///
    /// \returns An iterator to the beginning of the list.
    AXIS_NODISCARD const T* begin() const noexcept;

    /// \brief Iterator to the end of the list.
    ///
    /// \returns An iterator to the end of the list.
    AXIS_NODISCARD T* end() noexcept;

    /// \brief Iterator to the end of the list.
    ///
    /// \returns An iterator to the end of the list.
    AXIS_NODISCARD const T* end() const noexcept;

    /// \brief Implicit conversion: checks if the list is empty or not.
    ///
    /// \returns `false` if the list is empty, `true` otherwise.
    AXIS_NODISCARD operator Bool() const noexcept;

private:
    template <Bool CleanWhenThrow = true, Bool ListInitialize = false, Bool CopyConstructor = true, class... Args>
    Tuple<T*, Size>                        ConstructsNewList(Size elementCount,
                                                             Size allocationSize, // Specifies zero if not specified
                                                             T*   begin,
                                                             Args&&... args); // Returns a tuple containing the pointer to the list and the size of allocated element.
    template <Bool FreeMemory> static void ClearInternal(T*   buffer,
                                                         Size length);

    T*   _buffer          = nullptr; ///< Pointer to the list's buffer.
    Size _allocatedLength = 0;       ///< The length of the allocated buffer.
    Size _length          = 0;       ///< The length of the list.
};

} // namespace Axis

#include "../../Private/Axis/ListImpl.inl"

#endif // AXIS_SYSTEM_LIST_HPP