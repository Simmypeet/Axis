/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_SMARTPOINTER_HPP
#define AXIS_SYSTEM_SMARTPOINTER_HPP
#pragma once

#include "../../Private/Axis/CompressedPair.inl"
#include "Memory.hpp"
#include "Trait.hpp"
#include <atomic>

namespace Axis::System
{

namespace Detail::SmartPointer
{
template <class ValueType, class Deleter>
struct SmartPointerPointerType;

} // namespace Detail::SmartPointer

namespace Concept
{

/// \brief Type which can be used with the smart pointer.
template <class T>
concept SmartPointerValue = !IsReference<T> && !IsBoundedArray<T> && !IsVolatile<T>;

// clang-format off

/// \brief Type which can be used to delete the smart pointer's resource.
template <class T, class ValueType>
concept SmartPointerDeleter =
    SmartPointerValue<ValueType> &&
    Pure<T> &&
    noexcept(MakeValue<T>()(MakeValue<typename Detail::SmartPointer::SmartPointerPointerType<ValueType, T>::Type>())) &&
    IsNothrowCopyConstructible<T> &&
    IsNothrowCopyAssignable<T>;

// clang-format on

} // namespace Concept

/// \brief Axis's default smart pointer deleter
template <Concept::SmartPointerValue T>
struct DefaultDeleter;

/// \brief A scope-based smart pointer; it will automatically delete the object when it goes out of scope.
///
/// A RAII wrapper class over a resource. The resource is managed by the deleter. when the object goes
/// out of scope, the deleter is invoked with the pointer to the resource for the clean up.
///
/// Note:
///   - The deleter is also moved when move constructs if they are noexcept, else deleter's copy constructor is called.
///   - The deleter is also moved when move assigns if they are noexcept, else deleter's copy assignment is called.
///   - Moving unique pointer always turn its value into nullptr.
///   - Default value of unique pointer is nullptr.
///   - When `Reset()` or destructor is called, the pointer to the resource is passed to the deleter, even if it is nullptr.
template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter = DefaultDeleter<T>>
class UniquePointer final
{
public:
    /// \brief Smart pointer internal pointer type.
    using PointerType = typename Detail::SmartPointer::SmartPointerPointerType<T, Deleter>::Type;

    /// \brief Target pointer value type
    using ValueType = RemoveAllExtents<T>;

    /// \brief Checks if the template parameter U's pointer can be converted to the template parameter T's pointer.
    template <Concept::SmartPointerValue From, Concept::SmartPointerValue To>
    static constexpr Bool ConvertibleFrom = Concept::IsConvertible<typename Detail::SmartPointer::SmartPointerPointerType<From, Deleter>::Type,
                                                                   typename Detail::SmartPointer::SmartPointerPointerType<To, Deleter>::Type> &&
        !IsUnboundedArray<From> && !IsUnboundedArray<To>;

    /// \brief Constructs a null pointer. Default constructor.
    UniquePointer() noexcept;

    /// \brief Constructs a null pointer
    UniquePointer(decltype(nullptr)) noexcept;

    /// \brief Constructs a pointer from the given pointer and deleter.
    ///
    /// \param[in] ptr The pointer to construct the smart pointer from
    /// \param[in] deleter The deleter to use when deleting the pointer
    explicit UniquePointer(PointerType    ptr,
                           const Deleter& deleter = Deleter()) noexcept;

    /// \brief Copy constructor is deleted.
    UniquePointer(const UniquePointer<T, Deleter>&) = delete;

    /// \brief Move constructor.
    UniquePointer(UniquePointer<T, Deleter>&&) noexcept;

    /// \brief Move constructor and casts to the given type.
    ///
    /// \note The deleter will be default constructed.
    template <Concept::SmartPointerValue U, Concept::SmartPointerDeleter<U> AnotherDeleter>
    explicit(false) UniquePointer(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U, T>&& IsNothrowConstructible<Deleter, const AnotherDeleter&>);

    /// \brief Destructor
    ~UniquePointer() noexcept;

    /// \brief Copy assignment is deleted.
    UniquePointer<T, Deleter>& operator=(const UniquePointer<T, Deleter>&) = delete;

    /// \brief Move assignment.
    UniquePointer<T, Deleter>& operator=(UniquePointer<T, Deleter>&&) noexcept;

    /// \brief Releases and deletes the current object's resource
    UniquePointer<T, Deleter>& operator=(decltype(nullptr)) noexcept;

    /// \brief Move assignment and casts to the given type.
    template <Concept::SmartPointerValue U, Concept::SmartPointerDeleter<U> AnotherDeleter>
    UniquePointer<T, Deleter>& operator=(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U, T>&& IsNothrowAssignable<Deleter, const AnotherDeleter&>);

    /// \brief Dereferences the pointer
    AXIS_NODISCARD AddLValueReference<ValueType> operator*() const noexcept requires(!Concept::IsSame<T, void> && !IsUnboundedArray<T>);

    /// \brief Arrow operator
    AXIS_NODISCARD PointerType operator->() const noexcept requires(!Concept::IsSame<T, void> && !IsUnboundedArray<T>);

    /// \brief Subscript operator
    ///
    /// \param[in] index The index to access
    ///
    /// \warning This function is only available for arrays. There aren't any bounds checks.
    AXIS_NODISCARD AddLValueReference<ValueType> operator[](Size index) const noexcept requires(IsUnboundedArray<T>);

    /// \brief Checks if the pointer is null.
    AXIS_NODISCARD Bool operator==(decltype(nullptr)) const noexcept;

    /// \brief Checks if the pointer is not null.
    AXIS_NODISCARD Bool operator!=(decltype(nullptr)) const noexcept;

    /// \brief Implicit conversion to bool
    AXIS_NODISCARD explicit(false) operator Bool() const noexcept;

    /// \brief Gets the pointer.
    AXIS_NODISCARD PointerType GetPointer() const noexcept;

    /// \brief Releases and deletes the current object's resource
    void Reset() noexcept;

private:
    CompressedPair<PointerType, Deleter> _pair = {};

    template <Concept::SmartPointerValue U, Concept::SmartPointerDeleter<U>>
    friend class UniquePointer;
};

template <Concept::SmartPointerValue T>
class WeakPointer;

template <Concept::SmartPointerValue T>
class SharedPointer;

/// \brief Atomic signed integer used in reference counting
using AtomicReferenceCount = std::atomic<Size>;

namespace Detail::SmartPointer
{

class ReferenceCounter // Base class for atomic reference counting in shared pointer
{
public:
    ReferenceCounter() noexcept = default;

    virtual void DeleteResource() noexcept    = 0; // Deletes the resource
    virtual void DeleteThisCounter() noexcept = 0; // Deletes this reference counter

    inline void AddStrongCount() noexcept // Adds a strong count
    {
        StrongCount += 1;
    }

    inline void AddWeakCount() noexcept // Adds a weak count
    {
        WeakCount += 1;
    }

    inline void DeleteStrongCount() noexcept // Decrements the strong reference count
    {
        if ((StrongCount -= 1) == 0)
        {
            DeleteResource();
            DeleteWeakCount();
        }
    }

    inline void DeleteWeakCount() noexcept // Decrements the weak reference count
    {
        if ((WeakCount -= 1) == 0)
        {
            DeleteThisCounter();
        }
    }

    AtomicReferenceCount StrongCount = 1; // strong reference count
    AtomicReferenceCount WeakCount   = 1; // weak reference count
};

// Tag to invoke copy constructor
struct CopyConstructTag
{};

// Tag to invoke move constructor
struct MoveConstructTag
{};

// Manipulates strong count: SharedPointer
constexpr auto StrongIncrementer = [](ReferenceCounter* referenceCounter) {
    if (referenceCounter != nullptr)
        referenceCounter->AddStrongCount();
};

// Manipulates strong count: SharedPointer
constexpr auto StrongDecrementer = [](ReferenceCounter* referenceCounter) {
    if (referenceCounter != nullptr)
        referenceCounter->DeleteStrongCount();
};

// Manipulates weak count: WeakPointer
constexpr auto WeakIncrementer = [](ReferenceCounter* referenceCounter) {
    if (referenceCounter != nullptr)
        referenceCounter->AddWeakCount();
};

// Manipulates weak count: WeakPointer
constexpr auto WeakDecrementer = [](ReferenceCounter* referenceCounter) {
    if (referenceCounter != nullptr)
        referenceCounter->DeleteWeakCount();
};

using StrongIncrementerType = decltype(StrongIncrementer);
using StrongDecrementerType = decltype(StrongDecrementer);
using WeakIncrementerType   = decltype(WeakIncrementer);
using WeakDecrementerType   = decltype(WeakDecrementer);

template <Concept::SmartPointerValue T, typename Incrementer, typename Decrementer>
class BaseSharedPointer // Base class for SharedPointer and WeakPointer
{
public:
    using PointerType = RemoveAllExtents<T>*; // Pointer type

    inline BaseSharedPointer() noexcept = default; // Default constructor (nullptr)

    inline ~BaseSharedPointer() noexcept { Decrementer{}(_referenceCounter); } // Decrements the count

    // Constructs from resource pointer and reference counter pointer
    BaseSharedPointer(PointerType       resourcePointer,
                      ReferenceCounter* referenceCounterPointer) noexcept :
        _resource(resourcePointer),
        _referenceCounter(referenceCounterPointer) {}

    // Copy constructor, increments the count
    template <Concept::SmartPointerValue U>
    BaseSharedPointer(CopyConstructTag                                      tag,
                      const BaseSharedPointer<U, Incrementer, Decrementer>& other) noexcept :
        _resource(other._resource),
        _referenceCounter(other._referenceCounter)
    {
        Incrementer{}(_referenceCounter);
    }

    // Move constructr, doesn't increments the count
    template <Concept::SmartPointerValue U>
    BaseSharedPointer(MoveConstructTag                                 tag,
                      BaseSharedPointer<U, Incrementer, Decrementer>&& other) noexcept :
        _resource(other._resource),
        _referenceCounter(other._referenceCounter)
    {
        other._resource         = nullptr;
        other._referenceCounter = nullptr;
    }

    // Copy assignment, decrements the current count and increments the new count
    template <Concept::SmartPointerValue U>
    void CopyAssign(const BaseSharedPointer<U, Incrementer, Decrementer>& other) noexcept
    {
        Decrementer{}(_referenceCounter);

        _resource         = other._resource;
        _referenceCounter = other._referenceCounter;

        Incrementer{}(_referenceCounter);
    }

    // Move assignment, decrements the current count
    template <Concept::SmartPointerValue U>
    void MoveAssign(BaseSharedPointer<U, Incrementer, Decrementer>&& other) noexcept
    {
        Decrementer{}(_referenceCounter);

        _resource         = other._resource;
        _referenceCounter = other._referenceCounter;

        other._resource         = nullptr;
        other._referenceCounter = nullptr;
    }

    void Reset() noexcept // Decrements the count, turns into nullptr
    {
        Decrementer{}(_referenceCounter);
        _resource         = nullptr;
        _referenceCounter = nullptr;
    }

    BaseSharedPointer(const BaseSharedPointer& other) = delete;
    BaseSharedPointer(BaseSharedPointer&& other)      = delete;

    BaseSharedPointer& operator=(const BaseSharedPointer&) = delete;
    BaseSharedPointer& operator=(BaseSharedPointer&&) = delete;

protected:
    PointerType       _resource         = nullptr; ///< Pointer to the resource
    ReferenceCounter* _referenceCounter = nullptr; ///< Pointer to the reference counter

    template <Concept::SmartPointerValue, typename, typename>
    friend class ::Axis::System::Detail::SmartPointer::BaseSharedPointer;
};

} // namespace Detail::SmartPointer

/// \brief Smart pointer for shared ownership of a resource. The resource is deleted when the last strong reference
///        is deleted. This class manipulates the weak reference count.
template <Concept::SmartPointerValue T>
class WeakPointer final : private Detail::SmartPointer::BaseSharedPointer<T, Detail::SmartPointer::WeakIncrementerType, Detail::SmartPointer::WeakDecrementerType>
{
private:
    // Smart pointer's base type
    using BaseType = Detail::SmartPointer::BaseSharedPointer<T, Detail::SmartPointer::WeakIncrementerType, Detail::SmartPointer::WeakDecrementerType>;

public:
    /// \brief The target value type of the pointer
    using ValueType = RemoveAllExtents<T>;

    /// \brief The internal pointer type that is managed by this smart pointer
    using PointerType = ValueType*;

    /// \brief Checks if the template parameter U's pointer can be converted to the template parameter T's pointer.
    template <Concept::SmartPointerValue From, Concept::SmartPointerValue To>
    static constexpr Bool ConvertibleFrom = Concept::IsConvertible<RemoveAllExtents<From>*, RemoveAllExtents<To>*> && !IsUnboundedArray<From> && !IsUnboundedArray<To>;

    /// \brief Default constructor
    WeakPointer() noexcept = default;

    /// \brief Nullptr constructor
    WeakPointer(decltype(nullptr)) noexcept;

    /// \brief Destructor
    ///
    /// Decrements the reference count
    ~WeakPointer() noexcept = default;

    /// \brief Copy constructor
    ///
    /// Copies the pointer value and increments the reference count
    ///
    /// \param[in] other The other instance to copy
    WeakPointer(const WeakPointer& other) noexcept;

    /// \brief Copy constructor and cast from another type
    ///
    /// Copies the pointer value and increments the reference count
    ///
    /// \param[in] other The other instance to copy
    template <Concept::SmartPointerValue U>
    WeakPointer(const WeakPointer<U>& other) noexcept requires(ConvertibleFrom<U, T>);

    /// \brief Move constructor
    ///
    /// Moves the pointer value and turns the other instance into a null pointer
    ///
    /// \param[in] other The other instance to move
    WeakPointer(WeakPointer&& other) noexcept;

    /// \brief Move constructor and cast from another type
    ///
    /// Moves the pointer value and turns the other instance into a null pointer
    ///
    /// \param[in] other The other instance to move
    template <Concept::SmartPointerValue U>
    WeakPointer(WeakPointer<U>&& other) noexcept requires(ConvertibleFrom<U, T>);

    /// \brief Copy assignment operator
    ///
    /// Decrements the current reference count, increments the new reference count and copies the pointer value
    ///
    /// \param[in] other The other instance to copy
    WeakPointer& operator=(const WeakPointer&) noexcept;

    /// \brief Copy assignment operator and cast from another type
    ///
    /// Decrements the current reference count, increments the new reference count and copies the pointer value
    ///
    /// \param[in] other The other instance to copy
    template <Concept::SmartPointerValue U>
    WeakPointer& operator=(const WeakPointer<U>&) noexcept requires(ConvertibleFrom<U, T>);

    /// \brief Move assignment operator
    ///
    /// Decrements the current reference count, moves the pointer value and turns the other instance into a null pointer
    ///
    /// \param[in] other The other instance to move
    WeakPointer& operator=(WeakPointer&&) noexcept;

    /// \brief Move assignment operator and cast from another type
    ///
    /// Decrements the current reference count, moves the pointer value and turns the other instance into a null pointer
    ///
    /// \param[in] other The other instance to move
    template <Concept::SmartPointerValue U>
    WeakPointer& operator=(WeakPointer<U>&&) noexcept requires(ConvertibleFrom<U, T>);

    /// \brief Turns the pointer into nullptr and decrements the reference count
    WeakPointer& operator=(decltype(nullptr)) noexcept;

    /// \brief Turns the pointer into nullptr and decrements the reference count
    void Reset() noexcept;

    /// \brief Checks if two pointers point to the same resource or not.
    ///
    /// \param[in] other The other pointer to check
    ///
    /// \return Returns true if the pointers point to the same resource, false otherwise
    template <Concept::SmartPointerValue U>
    AXIS_NODISCARD Bool operator==(const WeakPointer<U>& other) const noexcept requires(ConvertibleFrom<U, T> || ConvertibleFrom<T, U>);

    /// \brief Checks if two pointers point to the same resource or not.
    ///
    /// \param[in] other The other pointer to check
    ///
    /// \return Returns true if the pointers don't point to the same resource, false otherwise
    template <Concept::SmartPointerValue U>
    AXIS_NODISCARD Bool operator!=(const WeakPointer<U>& other) const noexcept requires(ConvertibleFrom<U, T> || ConvertibleFrom<T, U>);

    /// \brief Checks if the pointer is null or not
    ///
    /// \return Returns true if the pointer is null, false otherwise
    AXIS_NODISCARD Bool operator==(decltype(nullptr)) const noexcept;

    /// \brief Checks if the pointer is null or not
    ///
    /// \return Returns true if the pointer is not null, false otherwise
    AXIS_NODISCARD Bool operator!=(decltype(nullptr)) const noexcept;

    /// \brief Gets the strong reference count of the resource.
    ///
    /// \pre The pointer shouldn't be nullptr when call this method.
    ///
    /// \return The atomic reference counter representing the strong reference count.
    const AtomicReferenceCount& GetStrongReferenceCount() const noexcept;

    /// \brief Creates the shared pointer and increments the strong count out of the weak pointer,
    ///        if the object is still alive.
    ///
    /// \return Returns a valid shared pointer if the object is still alive, else return nullptr shared pointer.
    template <Concept::SmartPointerValue U = T>
    AXIS_NODISCARD SharedPointer<U> Generate() const noexcept requires(ConvertibleFrom<T, U>);

private:
    WeakPointer(PointerType                             pointer,
                Detail::SmartPointer::ReferenceCounter* referenceCounterPointer) noexcept;

    template <Concept::SmartPointerValue>
    friend class WeakPointer;

    template <Concept::SmartPointerValue>
    friend class SharedPointer;
};

/// \brief Smart pointer for shared ownership of a resource. The resource is deleted when the last strong reference
///        is deleted. This class manipulates the strong reference count.
template <Concept::SmartPointerValue T>
class SharedPointer final : private Detail::SmartPointer::BaseSharedPointer<T, Detail::SmartPointer::StrongIncrementerType, Detail::SmartPointer::StrongDecrementerType>
{
private:
    using BaseType = Detail::SmartPointer::BaseSharedPointer<T, decltype(Detail::SmartPointer::StrongIncrementer), decltype(Detail::SmartPointer::StrongDecrementer)>;

    template <Concept::SmartPointerValue U>
    using BaseTypeFromType = Detail::SmartPointer::BaseSharedPointer<U, decltype(Detail::SmartPointer::StrongIncrementer), decltype(Detail::SmartPointer::StrongDecrementer)>;

    template <Concept::SmartPointerValue From, Concept::SmartPointerValue To>
    static constexpr Bool ConvertibleFrom = Concept::IsConvertible<RemoveAllExtents<From>*, RemoveAllExtents<To>*> && !IsUnboundedArray<From> && !IsUnboundedArray<To>;

public:
    /// \brief The target value type of the pointer
    using ValueType = RemoveAllExtents<T>;

    /// \brief The internal pointer type that is managed by this smart pointer
    using PointerType = ValueType*;

    /// \brief Default constructor
    SharedPointer() noexcept = default;

    /// \brief Nullptr constructor
    SharedPointer(decltype(nullptr)) noexcept;

    /// \brief Destructor
    ///
    /// Decrements the reference count
    ~SharedPointer() noexcept = default;

    /// \brief Copy constructor
    ///
    /// Copies the pointer value and increments the reference count
    ///
    /// \param[in] other The other instance to copy
    SharedPointer(const SharedPointer& other) noexcept;

    /// \brief Copy constructor and cast from another type
    ///
    /// Copies the pointer value and increments the reference count
    ///
    /// \param[in] other The other instance to copy
    template <Concept::SmartPointerValue U>
    SharedPointer(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U, T>);

    /// \brief Move constructor
    ///
    /// Moves the pointer value and turns the other instance into a null pointer
    ///
    /// \param[in] other The other instance to move
    SharedPointer(SharedPointer&& other) noexcept;

    /// \brief Move constructor and cast from another type
    ///
    /// Moves the pointer value and turns the other instance into a null pointer
    ///
    /// \param[in] other The other instance to move
    template <Concept::SmartPointerValue U>
    SharedPointer(SharedPointer<U>&& other) noexcept requires(ConvertibleFrom<U, T>);

    /// \brief Copy assignment operator
    ///
    /// Decrements the current reference count, increments the new reference count and copies the pointer value
    ///
    /// \param[in] other The other instance to copy
    SharedPointer& operator=(const SharedPointer&) noexcept;

    /// \brief Copy assignment operator and cast from another type
    ///
    /// Decrements the current reference count, increments the new reference count and copies the pointer value
    ///
    /// \param[in] other The other instance to copy
    template <Concept::SmartPointerValue U>
    SharedPointer& operator=(const SharedPointer<U>&) noexcept requires(ConvertibleFrom<U, T>);

    /// \brief Move assignment operator
    ///
    /// Decrements the current reference count, moves the pointer value and turns the other instance into a null pointer
    ///
    /// \param[in] other The other instance to move
    SharedPointer& operator=(SharedPointer&&) noexcept;

    /// \brief Move assignment operator and cast from another type
    ///
    /// Decrements the current reference count, moves the pointer value and turns the other instance into a null pointer
    ///
    /// \param[in] other The other instance to move
    template <Concept::SmartPointerValue U>
    SharedPointer& operator=(SharedPointer<U>&&) noexcept requires(ConvertibleFrom<U, T>);

    /// \brief Turns the pointer into nullptr and decrements the reference count
    SharedPointer& operator=(decltype(nullptr)) noexcept;

    /// \brief Turns the pointer into nullptr and decrements the reference count
    void Reset() noexcept;

    /// \brief Checks if two pointers point to the same resource or not.
    ///
    /// \param[in] other The other pointer to check
    ///
    /// \return Returns true if the pointers point to the same resource, false otherwise
    template <Concept::SmartPointerValue U>
    AXIS_NODISCARD Bool operator==(const SharedPointer<U>& other) const noexcept requires(ConvertibleFrom<U, T> || ConvertibleFrom<T, U>);

    /// \brief Checks if two pointers point to the same resource or not.
    ///
    /// \param[in] other The other pointer to check
    ///
    /// \return Returns true if the pointers don't point to the same resource, false otherwise
    template <Concept::SmartPointerValue U>
    AXIS_NODISCARD Bool operator!=(const SharedPointer<U>& other) const noexcept requires(ConvertibleFrom<U, T> || ConvertibleFrom<T, U>);

    /// \brief Checks if the pointer is null or not
    ///
    /// \return Returns true if the pointer is null, false otherwise
    AXIS_NODISCARD Bool operator==(decltype(nullptr)) const noexcept;

    /// \brief Checks if the pointer is null or not
    ///
    /// \return Returns true if the pointer is not null, false otherwise
    AXIS_NODISCARD Bool operator!=(decltype(nullptr)) const noexcept;

    /// \brief Dereferences the pointer
    AXIS_NODISCARD AddLValueReference<ValueType> operator*() const noexcept requires(!Concept::IsSame<T, void> && !IsUnboundedArray<T>);

    /// \brief Arrow operator
    AXIS_NODISCARD PointerType operator->() const noexcept requires(!Concept::IsSame<T, void> && !IsUnboundedArray<T>);

    /// \brief Subscript operator
    ///
    /// \param[in] index The index to access
    ///
    /// \warning This function is only available for arrays. There aren't any bounds checks.
    AXIS_NODISCARD AddLValueReference<ValueType> operator[](Size index) const noexcept requires(IsUnboundedArray<T>);

    /// \brief Swaps the content between two shared pointers
    ///
    /// \param[in] other The other smart pointer to swap
    void Swap(SharedPointer<T>& other) noexcept;

    /// \brief Explicitly cast the shared pointer to the new type.
    template <Concept::SmartPointerValue U>
    AXIS_NODISCARD explicit operator SharedPointer<U>() const noexcept;

    /// \brief Gets weak pointer from the existing shared pointer.
    template <Concept::SmartPointerValue U>
    AXIS_NODISCARD explicit operator WeakPointer<U>() const noexcept requires(ConvertibleFrom<U, T>);

    /// \brief Gets the strong reference count of the resource.
    ///
    /// \pre The pointer shouldn't be nullptr when call this method.
    ///
    /// \return The atomic reference counter representing the strong reference count.
    AXIS_NODISCARD const AtomicReferenceCount& GetStrongReferenceCount() const noexcept;

    /// \brief Creates new object using the specified `MemoryResource` for memory allocations and deallocations.
    ///
    /// \tparam[in] MemRes MemoryResource used for memory allocations and deallocations
    /// \tparam[in] Args Variadic arguments to pass to the object's constructor
    ///
    /// \return SharedPointer to the newly created object
    template <Concept::MemoryResource MemRes, class... Args>
    AXIS_NODISCARD static SharedPointer<T> MakeShared(Args&&... args) requires(!IsUnboundedArray<T>);

    /// \brief Creates an array of new objects using the specified `MemoryResource` for memory allocation and deallocation.
    ///
    /// \tparam[in] MemRes MemoryResource used for memory allocations and deallocations
    ///
    /// \param[in] count The number of elements to create
    ///
    /// \return SharedPointer to the array of new objects
    template <Concept::MemoryResource MemRes>
    AXIS_NODISCARD static SharedPointer<T> MakeShared(Size count) requires(IsUnboundedArray<T>);

    /// \brief Creates an array of new objects using the specified `MemoryResource` for memory allocation and deallocation.
    ///
    /// \tparam[in] MemRes MemoryResource used for memory allocations and deallocations
    ///
    /// \param[in] count The number of elements to create
    /// \param[in] value The value used to initialize the elements
    ///
    /// \return SharedPointer to the array of new objects
    template <Concept::MemoryResource MemRes, class... Args>
    AXIS_NODISCARD static SharedPointer<T> MakeShared(Size count,
                                                      Args&&... args) requires(IsUnboundedArray<T>);

private:
    SharedPointer(PointerType                             resourcePointer,
                  Detail::SmartPointer::ReferenceCounter* referenceCounterPointer) noexcept;

    template <Concept::SmartPointerValue>
    friend class SharedPointer;

    template <Concept::SmartPointerValue>
    friend class WeakPointer;
};


} // namespace Axis::System

#include "../../Private/Axis/SmartPointerImpl.inl"

#endif // AXIS_SYSTEM_SMARTPOINTER_HPP
