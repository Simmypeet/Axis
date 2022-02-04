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
#include <atomic>
#include <memory>

namespace Axis
{

/// \brief Type which can be used with the smart pointer.
template <class T>
concept SmartPointerType = std::is_same_v<T, void> ||(!std::is_reference_v<T> && !std::is_bounded_array_v<T>);

// clang-format off

/// \brief Type which can be used to delete the smart pointer's resource.
template <class T, class U>
concept SmartPointerDeleterType = Callable<std::remove_const_t<T>, void, std::decay_t<std::remove_all_extents_t<U>>*> && std::is_nothrow_default_constructible_v<std::remove_const_t<T>>;

// clang-format on

/// \brief Gets the pointer type of a type.
template <SmartPointerType T>
using Pointer = std::remove_all_extents_t<T>*;

template <SmartPointerType T>
struct DefaultDeleter;

/// \brief A scope-based smart pointer; it will automatically delete the object when it goes out of scope.
template <SmartPointerType T, SmartPointerDeleterType<T> Deleter = DefaultDeleter<T>>
class UniquePointer final
{
public:
    /// \brief Checks if the template parameter U's pointer can be converted to the template parameter T's pointer.
    template <SmartPointerType U>
    static constexpr Bool ConvertibleFrom = std::is_convertible_v<Pointer<U>, Pointer<T>>&& std::is_unbounded_array_v<T> == std::is_unbounded_array_v<U>;

    /// \brief Smart pointer internal pointer type.
    using PointerType = Pointer<T>;

    /// \brief Constructs a null pointer. Default constructor.
    UniquePointer() noexcept = default;

    /// \brief Constructs a null pointer
    UniquePointer(NullptrType) noexcept;

    /// \brief Constructs a pointer from the given pointer and deleter.
    ///
    /// \param[in] ptr The pointer to construct the smart pointer from
    /// \param[in] deleter The deleter to use when deleting the pointer
    explicit UniquePointer(PointerType ptr) noexcept;

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
    UniquePointer<T, Deleter>& operator=(NullptrType) noexcept;

    /// \brief Move assignment and casts to the given type.
    template <SmartPointerType U, SmartPointerDeleterType<U> AnotherDeleter>
    UniquePointer<T, Deleter>& operator=(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Dereferences the pointer
    template <SmartPointerType U = T, typename = std::enable_if_t<!std::disjunction_v<std::is_array<U>, std::is_void<U>>, Int32>>
    AXIS_NODISCARD std::add_lvalue_reference_t<std::remove_all_extents_t<T>> operator*() const noexcept;

    /// \brief Arrow operator
    template <SmartPointerType U = T, typename = std::enable_if_t<!std::disjunction_v<std::is_array<U>, std::is_void<U>>, Int32>>
    AXIS_NODISCARD PointerType operator->() const noexcept;

    /// \brief Subscript operator
    ///
    /// \param[in] index The index to access
    ///
    /// \warning This function is only available for arrays. There aren't any bounds checks.
    AXIS_NODISCARD std::add_lvalue_reference_t<std::remove_all_extents_t<T>> operator[](Size index) const requires(std::is_unbounded_array_v<T>);

    /// \brief Checks if the pointer is null.
    AXIS_NODISCARD Bool operator==(NullptrType) const noexcept;

    /// \brief Checks if the pointer is not null.
    AXIS_NODISCARD Bool operator!=(NullptrType) const noexcept;

    /// \brief Implicit conversion to bool
    AXIS_NODISCARD operator Bool() const noexcept;

    /// \brief Gets the pointer.
    AXIS_NODISCARD PointerType GetPointer() const noexcept;

    /// \brief Releases and deletes the current object's resource
    void Reset() noexcept;

private:
    PointerType _objectPointer = nullptr; ///< Pointer to the object.
    Deleter     _deleter       = {};      ///< The deleter function.

    template <SmartPointerType U, SmartPointerDeleterType<U>>
    friend class UniquePointer;
};

/// \brief A scope-based smart pointer; it will automatically delete the object when it goes out of scope.
template <SmartPointerType T, SmartPointerDeleterType<T> Deleter = DefaultDeleter<T>>
using Scope = UniquePointer<T, Deleter>;

/// \brief Atomic unsigned integer type used in reference counting.
using ReferenceCounter = std::atomic<Size>;

/// \brief A reference-counted smart pointer; it will automatically delete the object when it the
///        reference count reaches zero. This container manipulates the strong reference count.
template <SmartPointerType T>
class SharedPointer;

/// \brief A reference-counted smart pointer; it will automatically delete the object when it the
///        reference count reaches zero. This container manipulates the weak reference count.
template <SmartPointerType T>
class WeakPointer;

namespace Detail
{

/// Forward declaration of the internal reference counter.
class IReferenceCounter;

} // namespace Detail

/// \brief A reference-counted smart pointer; it will automatically delete the object when it the
///        reference count reaches zero. This container manipulates the strong reference count.
template <SmartPointerType T>
class SharedPointer final
{
public:
    /// \brief Checks if the template parameter U's pointer can be converted to the template parameter T's pointer.
    template <SmartPointerType U>
    static constexpr Bool ConvertibleFrom = std::is_convertible_v<Pointer<U>, Pointer<T>>&& std::is_unbounded_array_v<T> == std::is_unbounded_array_v<U>;

    /// \brief Smart pointer internal pointer type.
    using PointerType = Pointer<T>;

    /// \brief Constructs a null pointer. Default constructor.
    SharedPointer() noexcept = default;

    /// \brief Constructs a null pointer
    SharedPointer(NullptrType) noexcept;

    /// \brief Constructs a pointer from the given pointer.
    ///
    /// \param[in] ptr The pointer to construct the smart pointer from
    template <SmartPointerDeleterType<T> Deleter = DefaultDeleter<T>>
    explicit SharedPointer(PointerType ptr, Deleter deleter = {}) noexcept;

    /// \brief Copy constructor.
    SharedPointer(const SharedPointer<T>& other) noexcept;

    /// \brief Copy constructor and casts to the given type.
    template <SmartPointerType U>
    SharedPointer(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Move constructor.
    SharedPointer(SharedPointer<T>&& other) noexcept;

    /// \brief Move constructor and casts to the given type.
    template <SmartPointerType U>
    SharedPointer(SharedPointer<U>&& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Destructor
    ~SharedPointer() noexcept;

    /// \brief Copy assignment.
    SharedPointer<T>& operator=(const SharedPointer<T>&) noexcept;

    /// \brief Copy assignment and casts to the given type.
    template <SmartPointerType U>
    SharedPointer<T>& operator=(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Move assignment.
    SharedPointer<T>& operator=(SharedPointer<T>&&) noexcept;

    /// \brief Move assignment and casts to the given type.
    template <SmartPointerType U>
    SharedPointer<T>& operator=(SharedPointer<U>&& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Dereferences the pointer
    template <SmartPointerType U = T, typename = std::enable_if_t<!std::disjunction_v<std::is_array<U>, std::is_void<U>>, Int32>>
    AXIS_NODISCARD std::add_lvalue_reference_t<std::remove_all_extents_t<T>> operator*() const noexcept;

    /// \brief Arrow operator
    template <SmartPointerType U = T, typename = std::enable_if_t<!std::disjunction_v<std::is_array<U>, std::is_void<U>>, Int32>>
    AXIS_NODISCARD PointerType operator->() const noexcept;

    /// \brief Subscript operator
    ///
    /// \param[in] index The index to access
    ///
    /// \warning This function is only available for arrays. There aren't any bounds checks.
    AXIS_NODISCARD std::add_lvalue_reference_t<std::remove_all_extents_t<T>> operator[](Size index) const requires(std::is_unbounded_array_v<T>);

    /// \brief Checks if the pointer is null.
    AXIS_NODISCARD Bool operator==(NullptrType) const noexcept;

    /// \brief Checks if the pointer is not null.
    AXIS_NODISCARD Bool operator!=(NullptrType) const noexcept;

    /// \brief Implicit conversion to bool
    AXIS_NODISCARD operator Bool() const noexcept;

    /// \brief Gets the pointer.
    AXIS_NODISCARD PointerType GetPointer() const noexcept;

    /// \brief Releases and deletes the current object's resource
    void Reset() noexcept;

    /// \brief Equality operator
    ///
    /// \param[in] other The other pointer to compare with
    ///
    /// \return True if the pointers are equal, false otherwise
    AXIS_NODISCARD Bool operator==(const SharedPointer<T>& other) const noexcept;

    /// \brief Inequality operator
    ///
    /// \param[in] other The other pointer to compare with
    ///
    /// \return True if the pointers are not equal, false otherwise
    AXIS_NODISCARD Bool operator!=(const SharedPointer<T>& other) const noexcept;

    /// \brief Gets current reference counter strong count.
    AXIS_NODISCARD const ReferenceCounter& GetStrongCount() const noexcept;

    /// \brief Gets current reference counter strong count.
    AXIS_NODISCARD const ReferenceCounter& GetWeakCount() const noexcept;

    /// \brief Nullptr assignment
    SharedPointer<T>& operator=(NullptrType) noexcept;

    /// \brief Implicit conversion operator
    template <SmartPointerType U>
    AXIS_NODISCARD explicit operator SharedPointer<U>() const noexcept requires(!std::is_unbounded_array_v<T> && !std::is_unbounded_array_v<U>);

    /// \brief Implicit conversion operator
    template <SmartPointerType U>
    AXIS_NODISCARD explicit operator U*() const noexcept requires(!std::is_unbounded_array_v<T> && !std::is_unbounded_array_v<U>);


private:
    PointerType                _objectPointer    = nullptr; ///< Pointer to the object.
    Detail::IReferenceCounter* _referenceCounter = nullptr; ///< Pointer to the reference counter.

    template <SmartPointerType U, SmartPointerDeleterType<U>>
    friend class UniquePointer;

    template <SmartPointerType U>
    friend class SharedPointer;

    template <SmartPointerType U>
    friend class WeakPointer;

    friend class ISharedFromThis;

    template <SmartPointerType U, AllocatorType Allocator, class... Args, typename>
    friend SharedPointer<U> AllocatedMakeShared(Args&&...) requires(std::is_constructible_v<U, Args...>); // friend function to allow the AllocatedMakeShared function to access the private members

    template <SmartPointerType U, AllocatorType Allocator, typename>
    friend SharedPointer<U> AllocatedMakeShared(Size) requires(std::is_default_constructible_v<std::remove_all_extents_t<U>>); // friend function to allow the AllocatedMakeShared function to access the private members
};

/// \brief A reference-counted smart pointer; it will automatically delete the object when it the
///        reference count reaches zero. This container manipulates the weak reference count.
template <SmartPointerType T>
class WeakPointer final
{
public:
    /// \brief Checks if the template parameter U's pointer can be converted to the template parameter T's pointer.
    template <SmartPointerType U>
    static constexpr Bool ConvertibleFrom = std::is_convertible_v<Pointer<U>, Pointer<T>>&& std::is_unbounded_array_v<T> == std::is_unbounded_array_v<U>;

    /// \brief Smart pointer internal pointer type.
    using PointerType = Pointer<T>;

    /// \brief Constructs a null pointer. Default constructor.
    WeakPointer() noexcept = default;

    /// \brief Constructs a null pointer
    WeakPointer(NullptrType) noexcept;

    /// \brief Copy constructor.
    WeakPointer(const WeakPointer<T>& other) noexcept;

    /// \brief Copy constructor and casts to the given type.
    template <SmartPointerType U>
    WeakPointer(const WeakPointer<U>& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Constructs weak pointer from `Axis::SharedPointer`
    ///
    /// \param[in] other `Axis::SharedPointer` to construct from
    template <SmartPointerType U>
    WeakPointer(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Move constructor.
    WeakPointer(WeakPointer<T>&& other) noexcept;

    /// \brief Move constructor and casts to the given type.
    ///
    /// \note The deleter will be default constructed.
    template <SmartPointerType U>
    WeakPointer(WeakPointer<U>&& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Destructor
    ~WeakPointer() noexcept;

    /// \brief Copy assignment.
    WeakPointer<T>& operator=(const WeakPointer<T>&) noexcept;

    /// \brief Copy assignment and casts to the given type.
    template <SmartPointerType U>
    WeakPointer<T>& operator=(const WeakPointer<U>& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Copy assignment and casts to the given type.
    template <SmartPointerType U>
    WeakPointer<T>& operator=(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Move assignment.
    WeakPointer<T>& operator=(WeakPointer<T>&&) noexcept;

    /// \brief Move assignment and casts to the given type.
    template <SmartPointerType U>
    WeakPointer<T>& operator=(WeakPointer<U>&& other) noexcept requires(ConvertibleFrom<U>);

    /// \brief Checks if the pointer is null.
    AXIS_NODISCARD Bool operator==(NullptrType) const noexcept;

    /// \brief Checks if the pointer is not null.
    AXIS_NODISCARD Bool operator!=(NullptrType) const noexcept;

    /// \brief Gets current reference counter strong count.
    AXIS_NODISCARD const ReferenceCounter& GetStrongCount() const noexcept;

    /// \brief Gets current reference counter strong count.
    AXIS_NODISCARD const ReferenceCounter& GetWeakCount() const noexcept;

    /// \brief Implicit conversion to bool
    AXIS_NODISCARD operator Bool() const noexcept;

    /// \brief Gets the pointer.
    AXIS_NODISCARD PointerType GetPointer() const noexcept;

    /// \brief Releases and deletes the current object's resource
    void Reset() noexcept;

    /// \brief Equality operator
    ///
    /// \param[in] other The other pointer to compare with
    ///
    /// \return True if the pointers are equal, false otherwise
    AXIS_NODISCARD Bool operator==(const WeakPointer<T>& other) const noexcept;

    /// \brief Inequality operator
    ///
    /// \param[in] other The other pointer to compare with
    ///
    /// \return True if the pointers are not equal, false otherwise
    AXIS_NODISCARD Bool operator!=(const WeakPointer<T>& other) const noexcept;

    /// \brief Nullptr assignment
    WeakPointer<T>& operator=(NullptrType) noexcept;

    /// \brief Creates `Axis::SharedPointer` from weak pointer.
    ///
    /// \return Returns a valid `Axis::SharedPointer` if the object hasn't been destructed yet.
    SharedPointer<T> Generate() const noexcept;

private:
    PointerType                _objectPointer    = nullptr; ///< Pointer to the object.
    Detail::IReferenceCounter* _referenceCounter = nullptr; ///< Pointer to the reference counter.

    friend class ISharedFromThis;

    template <SmartPointerType U, SmartPointerDeleterType<U>>
    friend class UniquePointer;

    template <SmartPointerType U>
    friend class SharedPointer;

    template <SmartPointerType U>
    friend class WeakPointer;
};

/// \brief Constructs a new instance of shared pointer of the specific type.
///        this is the preferred way to create shared object as it's more efficient.
template <SmartPointerType T, class... Args, typename = std::enable_if_t<!std::is_unbounded_array_v<T>, Int32>>
AXIS_NODISCARD SharedPointer<T> MakeShared(Args&&... args) requires(std::is_constructible_v<T, Args...>);

/// \brief Constructs a new instance of shared pointer of the specific type.
///        this is the preferred way to create shared object as it's more efficient.
template <SmartPointerType T, typename = std::enable_if_t<std::is_unbounded_array_v<T>, Int32>>
AXIS_NODISCARD SharedPointer<T> MakeShared(Size elementCount) requires(std::is_default_constructible_v<std::remove_all_extents_t<T>>);

/// \brief Constructs a new instance of shared pointer of the specific type.
///        this is the preferred way to create shared object as it's more efficient.
template <SmartPointerType T, AllocatorType Allocator, class... Args, typename = std::enable_if_t<!std::is_unbounded_array_v<T>, Int32>>
AXIS_NODISCARD SharedPointer<T> AllocatedMakeShared(Args&&... args) requires(std::is_constructible_v<T, Args...>);

/// \brief Constructs a new instance of shared pointer of the specific type.
///        this is the preferred way to create shared object as it's more efficient.
template <SmartPointerType T, AllocatorType Allocator, typename = std::enable_if_t<std::is_unbounded_array_v<T>, Int32>>
AXIS_NODISCARD SharedPointer<T> AllocatedMakeShared(Size elementCount) requires(std::is_default_constructible_v<std::remove_all_extents_t<T>>);

/// \brief Inherits this class to be able to generate \a `Axis::SharedPointer`
///        via the reference to the object itself when the object is created via SharedPointer.
class AXIS_SYSTEM_API ISharedFromThis
{
public:
    /// \brief Default constructor
    ISharedFromThis() noexcept;

    /// \brief Pure virtual destructor
    virtual ~ISharedFromThis() noexcept;

    /// This class can't be moved and copied
    ISharedFromThis(const ISharedFromThis&) = delete;
    ISharedFromThis(ISharedFromThis&&)      = delete;
    ISharedFromThis& operator=(const ISharedFromThis&) = delete;
    ISharedFromThis& operator=(ISharedFromThis&&) = delete;

    /// \brief Generates the \a `Axis::SharedPointer` and increments
    ///        the strong reference count by one.
    ///
    /// \warning If the object which is passed to the function is still under construction (constructor),
    ///          the function will return `nullptr`.
    ///
    /// \warning This function will return nullptr \a `Axis::ReferencePointer`
    ///          if the function is called in the constructor or the object
    ///          is not created by SharedPointer
    template <SmartPointerType T>
    AXIS_NODISCARD static SharedPointer<T> CreateSharedPointerFromThis(T& object) noexcept requires(!std::is_array_v<T> && std::is_base_of_v<ISharedFromThis, T> && std::is_convertible_v<T*, ISharedFromThis*>);

    /// \brief Generates the \a Axis::WeakReferencePointer and increments
    ///        the weak reference count by 1.
    ///
    /// \warning If the object which is passed to the function is still under construction (constructor),
    ///          the function will return nullptr.
    ///
    /// \warning This function will return nullptr \a Axis::WeakReferencePointer
    ///          if the function is called in the constructor or the object
    ///          is not created by SharedPointer
    template <SmartPointerType T>
    AXIS_NODISCARD static WeakPointer<T> CreateWeakPointerFromThis(T& object) noexcept requires(!std::is_array_v<T> && std::is_base_of_v<ISharedFromThis, T> && std::is_convertible_v<T*, ISharedFromThis*>);

private:
    /////////////////////////////////////////////////////////////////
    /// Private members
    ///
    /////////////////////////////////////////////////////////////////
    PVoid _objectPtr = nullptr;        /// Raw pointer to the object. The pointer is stored as it
                                       /// was created with Axis::CreateRef, Axis::CreateRaw.
    PVoid _referenceCounter = nullptr; /// Pointer to the reference counter object.

    template <SmartPointerType T>
    friend class SharedPointer; // Friend class

    template <SmartPointerType U, AllocatorType Allocator, class... Args, typename>
    friend SharedPointer<U> AllocatedMakeShared(Args&&...) requires(std::is_constructible_v<U, Args...>); // friend function to allow the AllocatedMakeShared function to access the private members

    template <SmartPointerType U, AllocatorType Allocator, typename>
    friend SharedPointer<U> AllocatedMakeShared(Size) requires(std::is_default_constructible_v<std::remove_all_extents_t<U>>); // friend function to allow the AllocatedMakeShared function to access the private members
};

} // namespace Axis

#include "../../Private/Axis/SmartPointerImpl.inl"

#endif // AXIS_SYSTEM_SMARTPOINTER_HPP
