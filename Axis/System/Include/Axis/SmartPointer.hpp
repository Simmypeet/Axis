/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file SmartPointer.hpp
///
/// \brief Contains the definition of various smart pointer classes.

#ifndef AXIS_SYSTEM_SMARTPOINTER_HPP
#define AXIS_SYSTEM_SMARTPOINTER_HPP

#include "Memory.hpp"
#include "Trait.hpp"

namespace Axis
{

/// \brief Type which can be used with the smart pointer.
template <class T>
concept SmartPointerType = std::is_same_v<T, void> || !std::is_reference_v<T> && std::is_nothrow_destructible_v<std::decay_t<std::remove_all_extents_t<T>>>;

// clang-format off

/// \brief Type which can be used to delete the smart pointer's resource.
template <class T, class U>
concept SmartPointerDeleterType = Callable<std::remove_const_t<T>, void, std::decay_t<std::remove_all_extents_t<U>>*> && std::is_nothrow_default_constructible_v<std::remove_const_t<T>>;

// clang-format on

template <SmartPointerType T>
struct DefaultDeleter;

/// \brief A scope-based smart pointer; it will automatically delete the object when it goes out of scope.
template <SmartPointerType T, SmartPointerDeleterType<T> Deleter = DefaultDeleter<T>>
class UniquePointer final
{
public:
    /// \brief Checks if the template parameter U's pointer can be converted to the template parameter T's pointer.
    template <SmartPointerType U>
    static constexpr Bool ConvertibleFrom = std::is_convertible_v<std::remove_all_extents_t<U>*, std::remove_all_extents_t<T>*> && !std::is_array_v<T> && !std::is_array_v<U>;

    /// \brief Smart pointer internal pointer type.
    using PointerType = std::remove_all_extents_t<T>*;

    /// \brief Constructs a null pointer. Default constructor.
    UniquePointer() noexcept = default;

    /// \brief Constructs a null pointer
    UniquePointer(NullptrType) noexcept;

    /// \brief Constructs a pointer from the given pointer and deleter.
    ///
    /// \param[in] ptr The pointer to construct the smart pointer from
    /// \param[in] deleter The deleter to use when deleting the pointer
    UniquePointer(PointerType ptr) noexcept;

    /// \brief Copy constructor is deleted.
    UniquePointer(const UniquePointer<T, Deleter>&) = delete;

    /// \brief Move constructor.
    UniquePointer(UniquePointer<T, Deleter>&&) noexcept;

    /// \brief Move constructor and casts to the given type.
    ///
    /// \note The deleter will be default constructed.
    template <SmartPointerType U, SmartPointerDeleterType<U> AnotherDeleter>
    UniquePointer(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Destructor
    ~UniquePointer() noexcept;

    /// \brief Copy assignment is deleted.
    UniquePointer<T, Deleter>& operator=(const UniquePointer<T, Deleter>&) = delete;

    /// \brief Move assignment.
    UniquePointer<T, Deleter>& operator=(UniquePointer<T, Deleter>&&) noexcept;

    /// \brief Releases and deletes the current object's resource
    UniquePointer<T, Deleter>& operator=(NullptrType);

    /// \brief Move assignment and casts to the given type.
    template <SmartPointerType U, SmartPointerDeleterType<U> AnotherDeleter>
    UniquePointer<T, Deleter>& operator=(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Dereferences the pointer
    template <SmartPointerType U = T, typename = std::enable_if_t<!std::disjunction_v<std::is_array<U>, std::is_void<U>>, Int32>>
    AXIS_NODISCARD std::add_lvalue_reference_t<std::remove_all_extents<T>> operator*() const;

    /// \brief Arrow operator
    template <SmartPointerType U = T, typename = std::enable_if_t<!std::disjunction_v<std::is_array<U>, std::is_void<U>>, Int32>>
    AXIS_NODISCARD PointerType operator->() const;

    /// \brief Subscript operator
    ///
    /// \param[in] index The index to access
    ///
    /// \warning This function is only available for arrays. There aren't any bounds checks.
    AXIS_NODISCARD std::add_lvalue_reference_t<std::remove_all_extents_t<T>> operator[](Size index) const requires(std::is_array_v<T>);

    /// \brief Checks if the pointer is null.
    AXIS_NODISCARD Bool operator==(NullptrType) const noexcept;

    /// \brief Checks if the pointer is not null.
    AXIS_NODISCARD Bool operator!=(NullptrType) const noexcept;

    /// \brief Implicit conversion to bool
    AXIS_NODISCARD operator Bool() const noexcept;

    /// \brief Gets the pointer.
    AXIS_NODISCARD PointerType GetPointer() const noexcept;

    /// \brief Releases and deletes the current object's resource
    void Reset();

private:
    PointerType _objectPointer = nullptr; ///< Pointer to the object.
    Deleter     _deleter       = {};      ///< The deleter function.

    template <SmartPointerType U, SmartPointerDeleterType<U>>
    friend class UniquePointer;
};

/// \brief A scope-based smart pointer; it will automatically delete the object when it goes out of scope.
template <SmartPointerType T, SmartPointerDeleterType<T> Deleter = DefaultDeleter<T>>
using Scope = UniquePointer<T, Deleter>;

} // namespace Axis

#include "../../Private/Axis/SmartPointerImpl.inl"

#endif // AXIS_SYSTEM_SMARTPOINTER_HPP