/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_SMARTPOINTERIMPL_INL
#define AXIS_SYSTEM_SMARTPOINTERIMPL_INL
#pragma once

#include "../../Include/Axis/Assert.hpp"
#include "../../Include/Axis/SmartPointer.hpp"

namespace Axis
{

namespace System
{

template <SmartPointerType T>
struct DefaultDeleter
{
    inline void operator()(std::remove_all_extents_t<T>* objectPointer) const noexcept
    {
        if constexpr (std::is_unbounded_array_v<T>)
            Axis::System::DeleteArray<std::remove_all_extents_t<T>>(objectPointer);
        else
            Axis::System::Delete<std::remove_all_extents_t<T>>(objectPointer);
    }
};

// ===> UniquePointer vvv

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer() noexcept :
    _pair()
{
    _pair.GetFirst() = nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer(NullptrType) noexcept :
    _pair()
{
    _pair.GetFirst() = nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer(PointerType    ptr,
                                                const Deleter& deleter) noexcept :
    _pair(ptr, deleter) {}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer(UniquePointer<T, Deleter>&& other) noexcept :
    _pair(PerfectForwardTag, MoveConstructIfNoThrow(other._pair.GetFirst()), MoveConstructIfNoThrow(other._pair.GetSecond()))
{
    other._pair.GetFirst() = nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
template <SmartPointerType U, SmartPointerDeleterType<U> AnotherDeleter>
inline UniquePointer<T, Deleter>::UniquePointer(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U>) :
    _pair(PerfectForwardTag, MoveConstructIfNoThrow(other._pair.GetFirst()), MoveConstructIfNoThrow(other._pair.GetSecond()))
{
    other._pair.GetFirst() = nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::~UniquePointer() noexcept
{
    if (_pair.GetFirst() != nullptr)
        _pair.GetSecond()(_pair.GetFirst());
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>& UniquePointer<T, Deleter>::operator=(UniquePointer<T, Deleter>&& other) noexcept
{
    if (this == AddressOf(other))
        return *this;

    if (_pair.GetFirst() != nullptr)
        _pair.GetSecond()(_pair.GetFirst());

    _pair.GetFirst()  = MoveAssignIfNoThrow(other._pair.GetFirst());
    _pair.GetSecond() = MoveAssignIfNoThrow(other._pair.GetSecond());

    other._pair.GetFirst() = nullptr;

    return *this;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>& UniquePointer<T, Deleter>::operator=(NullptrType) noexcept
{
    Reset();
    return *this;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
template <SmartPointerType U, SmartPointerDeleterType<U> AnotherDeleter>
inline UniquePointer<T, Deleter>& UniquePointer<T, Deleter>::operator=(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U>)
{
    if (this == AddressOf(other))
        return *this;

    if (_pair.GetFirst() != nullptr)
        _pair.GetSecond()(_pair.GetFirst());

    _pair.GetFirst()  = MoveAssignIfNoThrow(_pair.GetFirst());
    _pair.GetSecond() = MoveAssignIfNoThrow(_pair.GetSecond());

    other._pair.GetFirst() = nullptr;

    return *this;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
template <SmartPointerType U, typename>
inline std::add_lvalue_reference_t<std::remove_all_extents_t<T>> UniquePointer<T, Deleter>::operator*() const noexcept
{
    return *(_pair.GetFirst());
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
template <SmartPointerType U, typename>
inline typename UniquePointer<T, Deleter>::PointerType UniquePointer<T, Deleter>::operator->() const noexcept
{
    return _pair.GetFirst();
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline std::add_lvalue_reference_t<std::remove_all_extents_t<T>> UniquePointer<T, Deleter>::operator[](Size index) const requires(std::is_unbounded_array_v<T>)
{
    return _pair.GetFirst()[index];
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline Bool UniquePointer<T, Deleter>::operator==(NullptrType) const noexcept
{
    return _pair.GetFirst() == nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline Bool UniquePointer<T, Deleter>::operator!=(NullptrType) const noexcept
{
    return _pair.GetFirst() != nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline UniquePointer<T, Deleter>::operator Bool() const noexcept
{
    return _pair.GetFirst() != nullptr;
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline typename UniquePointer<T, Deleter>::PointerType UniquePointer<T, Deleter>::GetPointer() const noexcept
{
    return _pair.GetFirst();
}

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
inline void UniquePointer<T, Deleter>::Reset() noexcept
{
    if (_pair.GetFirst() != nullptr)
        _pair.GetSecond()(_pair.GetFirst());
}

// ===> SharedPointer vvv

namespace Detail
{

namespace SmartPointer
{

class IReferenceCounter // base class for all reference counter
{
public:
    // Default constructor
    IReferenceCounter() noexcept = default;

    // Deletes the managed object
    virtual void DeleteObject() noexcept = 0;

    // Deletes the current reference counter
    virtual void DeleteThisCounter() noexcept = 0;

    // Gets the strong reference counter
    inline const ReferenceCounter& GetStrongCount() const noexcept { return _strongCount; }

    // Gets the weak reference counter
    inline const ReferenceCounter& GetWeakCount() const noexcept { return _weakCount; }

    // Increments the strong reference count
    inline void AddStrongCount() noexcept { _strongCount += 1; }

    // Increments the strong reference count
    inline void AddWeakCount() noexcept { _weakCount += 1; }

    // Decrements the strong reference count
    inline const ReferenceCounter& DeleteStrongCount() noexcept
    {
        if ((_strongCount -= 1) == 0)
        {
            DeleteObject();
            DeleteWeakCount();
        }
        return _strongCount;
    }

    // Decrements the weak reference count
    inline const ReferenceCounter& DeleteWeakCount() noexcept
    {
        if ((_weakCount -= 1) == 0)
        {
            DeleteThisCounter();
        }
        return _weakCount;
    }

private:
    ReferenceCounter _strongCount = 1; ///< strong reference count (always starts with one)
    ReferenceCounter _weakCount   = 1; ///< weak reference count
};

template <SmartPointerType T, SmartPointerDeleterType<T> Deleter>
class ReferenceCounterConstructor final : public IReferenceCounter // reference counter when creates the shared pointer via constructor
{
public:
    ReferenceCounterConstructor(std::remove_all_extents_t<T>* objectPointer,
                                Deleter&&                     deleter) noexcept :
        _pair(PerfectForwardTag, objectPointer, Move(deleter)) {}

    ReferenceCounterConstructor(std::remove_all_extents_t<T>* objectPointer,
                                const Deleter&                deleter) noexcept :
        _pair(PerfectForwardTag, objectPointer, deleter) {}

    // Using Deleter to delete the object
    inline void DeleteObject() noexcept override final { _pair.GetSecond()(_pair.GetFirst()); }

    // Using Axis::Delete to delete this reference counter
    inline void DeleteThisCounter() noexcept override final { Axis::System::Delete(this); }

private:
    CompressedPair<std::remove_all_extents_t<T>*, Deleter> _pair;
};

struct MakeSharedHeaderArray final
{
    Size  ElementCount;   // Number of instances
    PVoid OriginalMemory; // Pointer to the memory which is allocated in the first place
};

template <SmartPointerType T, MemoryResourceType MemRes>
class ReferenceCounterMakeShared final : public IReferenceCounter // reference counter when creates with `MakeShared` and `AllocatedMakeShared`
{
public:
    ReferenceCounterMakeShared(std::remove_all_extents_t<T>* objectPointer) noexcept :
        _objectPointer(objectPointer) {}

    // Using Deleter to delete the object
    inline void DeleteObject() noexcept override final
    {
        if constexpr (std::is_unbounded_array_v<T>)
        {
            MakeSharedHeaderArray* header = (MakeSharedHeaderArray*)(((PVoid)_objectPointer)) - 1;
            using Type                    = std::remove_all_extents_t<T>;

            for (Size i = 0; i < header->ElementCount; i++)
                _objectPointer[i].~Type();
        }
        else
        {
            _objectPointer->~T();
        }
    }

    // Using Axis::Delete to delete this reference counter
    inline void DeleteThisCounter() noexcept override final
    {
        if constexpr (std::is_unbounded_array_v<T>)
        {
            MakeSharedHeaderArray* header = (MakeSharedHeaderArray*)(((PVoid)_objectPointer)) - 1;

            MemRes::Deallocate(header->OriginalMemory);
        }
        else
        {
            MemRes::Deallocate(_objectPointer);
        }
    }

private:
    std::remove_all_extents_t<T>* _objectPointer = nullptr; // pointer to the original object
};

} // namespace SmartPointer

} // namespace Detail

template <SmartPointerType T>
inline SharedPointer<T>::SharedPointer(NullptrType) noexcept {}

template <SmartPointerType T>
template <SmartPointerDeleterType<T> Deleter>
inline SharedPointer<T>::SharedPointer(PointerType ptr,
                                       Deleter     deleter) noexcept :
    _objectPointer(ptr),
    _referenceCounter(Axis::System::New<Detail::SmartPointer::ReferenceCounterConstructor<T, DefaultDeleter<T>>>(ptr, std::move(deleter)))
{
    if constexpr (!std::is_array_v<T> && std::is_base_of_v<ISharedFromThis, T> && std::is_convertible_v<T*, ISharedFromThis*>)
    {
        ISharedFromThis* referenceFromThis   = static_cast<ISharedFromThis*>(ptr);
        referenceFromThis->_referenceCounter = (PVoid)_referenceCounter;
        referenceFromThis->_objectPtr        = (PVoid)_objectPointer;
    }
}

template <SmartPointerType T>
inline SharedPointer<T>::SharedPointer(const SharedPointer<T>& other) noexcept :
    _objectPointer(other._objectPointer),
    _referenceCounter(other._referenceCounter)
{
    if (_referenceCounter)
        _referenceCounter->AddStrongCount();
}

template <SmartPointerType T>
template <SmartPointerType U>
inline SharedPointer<T>::SharedPointer(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U>) :
    _objectPointer(other._objectPointer),
    _referenceCounter(other._referenceCounter)
{
    if (_referenceCounter)
        _referenceCounter->AddStrongCount();
}


template <SmartPointerType T>
inline SharedPointer<T>::SharedPointer(SharedPointer<T>&& other) noexcept :
    _objectPointer(other._objectPointer),
    _referenceCounter(other._referenceCounter)
{
    other._objectPointer    = nullptr;
    other._referenceCounter = nullptr;
}

template <SmartPointerType T>
template <SmartPointerType U>
inline SharedPointer<T>::SharedPointer(SharedPointer<U>&& other) noexcept requires(ConvertibleFrom<U>) :
    _objectPointer(other._objectPointer),
    _referenceCounter(other._referenceCounter)
{
    other._objectPointer    = nullptr;
    other._referenceCounter = nullptr;
}

template <SmartPointerType T>
inline SharedPointer<T>::~SharedPointer() noexcept
{
    if (_referenceCounter != nullptr && _objectPointer != nullptr)
        _referenceCounter->DeleteStrongCount();
}

template <SmartPointerType T>
inline SharedPointer<T>& SharedPointer<T>::operator=(const SharedPointer<T>& other) noexcept
{
    if (_objectPointer != other._objectPointer)
    {
        if (_referenceCounter != nullptr && _objectPointer != nullptr)
            _referenceCounter->DeleteStrongCount();

        _objectPointer    = other._objectPointer;
        _referenceCounter = other._referenceCounter;

        if (_referenceCounter)
            _referenceCounter->AddStrongCount();
    }
    return *this;
}

template <SmartPointerType T>
template <SmartPointerType U>
inline SharedPointer<T>& SharedPointer<T>::operator=(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U>)
{
    if (_objectPointer != other._objectPointer)
    {
        if (_referenceCounter != nullptr && _objectPointer != nullptr)
            _referenceCounter->DeleteStrongCount();

        _objectPointer    = other._objectPointer;
        _referenceCounter = other._referenceCounter;

        if (_referenceCounter)
            _referenceCounter->AddStrongCount();
    }

    return *this;
}

template <SmartPointerType T>
inline SharedPointer<T>& SharedPointer<T>::operator=(SharedPointer<T>&& other) noexcept
{
    if (_objectPointer != other._objectPointer)
    {
        if (_referenceCounter != nullptr && _objectPointer != nullptr)
            _referenceCounter->DeleteStrongCount();

        _objectPointer    = other._objectPointer;
        _referenceCounter = other._referenceCounter;

        other._objectPointer    = nullptr;
        other._referenceCounter = nullptr;
    }

    return *this;
}

template <SmartPointerType T>
template <SmartPointerType U>
inline SharedPointer<T>& SharedPointer<T>::operator=(SharedPointer<U>&& other) noexcept requires(ConvertibleFrom<U>)
{
    if (_objectPointer != other._objectPointer)
    {
        if (_referenceCounter != nullptr && _objectPointer != nullptr)
            _referenceCounter->DeleteStrongCount();

        _objectPointer    = other._objectPointer;
        _referenceCounter = other._referenceCounter;

        other._objectPointer    = nullptr;
        other._referenceCounter = nullptr;
    }

    return *this;
}

template <SmartPointerType T>
template <SmartPointerType U, typename>
inline std::add_lvalue_reference_t<std::remove_all_extents_t<T>> SharedPointer<T>::operator*() const noexcept
{
    return *_objectPointer;
}

template <SmartPointerType T>
template <SmartPointerType U, typename>
inline typename SharedPointer<T>::PointerType SharedPointer<T>::operator->() const noexcept
{
    return _objectPointer;
}

template <SmartPointerType T>
inline std::add_lvalue_reference_t<std::remove_all_extents_t<T>> SharedPointer<T>::operator[](Size index) const requires(std::is_unbounded_array_v<T>)
{
    return _objectPointer[index];
}

template <SmartPointerType T>
inline Bool SharedPointer<T>::operator==(NullptrType) const noexcept
{
    return _objectPointer == nullptr;
}

template <SmartPointerType T>
inline Bool SharedPointer<T>::operator!=(NullptrType) const noexcept
{
    return _objectPointer != nullptr;
}

template <SmartPointerType T>
inline SharedPointer<T>::operator Bool() const noexcept
{
    return _objectPointer != nullptr;
}

template <SmartPointerType T>
inline typename SharedPointer<T>::PointerType SharedPointer<T>::GetPointer() const noexcept
{
    return _objectPointer;
}

template <SmartPointerType T>
inline void SharedPointer<T>::Reset() noexcept
{
    if (_objectPointer != nullptr)
    {
        _referenceCounter->DeleteStrongCount();
        _objectPointer    = nullptr;
        _referenceCounter = nullptr;
    }
}

template <SmartPointerType T>
inline Bool SharedPointer<T>::operator==(const SharedPointer<T>& other) const noexcept
{
    return _objectPointer == other._objectPointer;
}

template <SmartPointerType T>
inline Bool SharedPointer<T>::operator!=(const SharedPointer<T>& other) const noexcept
{
    return _objectPointer != other._objectPointer;
}

template <SmartPointerType T>
inline const ReferenceCounter& SharedPointer<T>::GetStrongCount() const noexcept
{
    return _referenceCounter->GetStrongCount();
}

template <SmartPointerType T>
inline const ReferenceCounter& SharedPointer<T>::GetWeakCount() const noexcept
{
    return _referenceCounter->GetWeakCount();
}

template <SmartPointerType T>
inline SharedPointer<T>& SharedPointer<T>::operator=(NullptrType) noexcept
{
    Reset();
    return *this;
}

template <SmartPointerType T>
template <SmartPointerType U>
inline SharedPointer<T>::operator SharedPointer<U>() const noexcept requires(!std::is_unbounded_array_v<T> && !std::is_unbounded_array_v<U>)
{
    SharedPointer<U> castedPointer = {};

    castedPointer._objectPointer    = (U*)_objectPointer;
    castedPointer._referenceCounter = _referenceCounter;

    _referenceCounter->AddStrongCount();

    return castedPointer;
}

template <SmartPointerType T>
template <SmartPointerType U>
inline SharedPointer<T>::operator U*() const noexcept requires(!std::is_unbounded_array_v<T> && !std::is_unbounded_array_v<U>)
{
    return (U*)_objectPointer;
}

// ===> WeakPointer vvv

template <SmartPointerType T>
inline WeakPointer<T>::WeakPointer(NullptrType) noexcept {}

template <SmartPointerType T>
inline WeakPointer<T>::WeakPointer(const WeakPointer<T>& other) noexcept :
    _objectPointer(other._objectPointer),
    _referenceCounter(other._referenceCounter)
{
    if (_referenceCounter)
        _referenceCounter->AddWeakCount();
}

template <SmartPointerType T>
inline WeakPointer<T>::WeakPointer(WeakPointer<T>&& other) noexcept :
    _objectPointer(other._objectPointer),
    _referenceCounter(other._referenceCounter)
{
    other._objectPointer    = nullptr;
    other._referenceCounter = nullptr;
}

template <SmartPointerType T>
template <SmartPointerType U>
inline WeakPointer<T>::WeakPointer(const WeakPointer<U>& other) noexcept requires(ConvertibleFrom<U>) :
    _objectPointer(other._objectPointer),
    _referenceCounter(other._referenceCounter)
{
    if (_referenceCounter)
        _referenceCounter->AddWeakCount();
}

template <SmartPointerType T>
template <SmartPointerType U>
inline WeakPointer<T>::WeakPointer(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U>) :
    _objectPointer(other._objectPointer),
    _referenceCounter(other._referenceCounter)
{
    if (_referenceCounter)
        _referenceCounter->AddWeakCount();
}

template <SmartPointerType T>
template <SmartPointerType U>
inline WeakPointer<T>::WeakPointer(WeakPointer<U>&& other) noexcept requires(ConvertibleFrom<U>) :
    _objectPointer(other._objectPointer),
    _referenceCounter(other._referenceCounter)
{
    other._objectPointer    = nullptr;
    other._referenceCounter = nullptr;
}

template <SmartPointerType T>
inline WeakPointer<T>::~WeakPointer() noexcept
{
    if (_referenceCounter != nullptr && _objectPointer != nullptr)
        _referenceCounter->DeleteWeakCount();
}

template <SmartPointerType T>
inline WeakPointer<T>& WeakPointer<T>::operator=(const WeakPointer<T>& other) noexcept
{
    if (_objectPointer != other._objectPointer)
    {
        if (_referenceCounter != nullptr && _objectPointer != nullptr)
            _referenceCounter->DeleteWeakCount();

        _objectPointer    = other._objectPointer;
        _referenceCounter = other._referenceCounter;

        if (_referenceCounter)
            _referenceCounter->AddWeakCount();
    }

    return *this;
}

template <SmartPointerType T>
inline WeakPointer<T>& WeakPointer<T>::operator=(WeakPointer<T>&& other) noexcept
{
    if (_objectPointer != other._objectPointer)
    {
        if (_referenceCounter != nullptr && _objectPointer != nullptr)
            _referenceCounter->DeleteWeakCount();

        _objectPointer    = other._objectPointer;
        _referenceCounter = other._referenceCounter;

        other._objectPointer    = nullptr;
        other._referenceCounter = nullptr;
    }

    return *this;
}

template <SmartPointerType T>
template <SmartPointerType U>
inline WeakPointer<T>& WeakPointer<T>::operator=(const WeakPointer<U>& other) noexcept requires(ConvertibleFrom<U>)
{
    if (_objectPointer != other._objectPointer)
    {
        if (_referenceCounter != nullptr && _objectPointer != nullptr)
            _referenceCounter->DeleteWeakCount();

        _objectPointer    = other._objectPointer;
        _referenceCounter = other._referenceCounter;

        if (_referenceCounter)
            _referenceCounter->AddWeakCount();
    }

    return *this;
}

template <SmartPointerType T>
template <SmartPointerType U>
inline WeakPointer<T>& WeakPointer<T>::operator=(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U>)
{
    if (_objectPointer != other._objectPointer)
    {
        if (_referenceCounter != nullptr && _objectPointer != nullptr)
            _referenceCounter->DeleteWeakCount();

        _objectPointer    = other._objectPointer;
        _referenceCounter = other._referenceCounter;

        if (_referenceCounter)
            _referenceCounter->AddWeakCount();
    }

    return *this;
}

template <SmartPointerType T>
template <SmartPointerType U>
inline WeakPointer<T>& WeakPointer<T>::operator=(WeakPointer<U>&& other) noexcept requires(ConvertibleFrom<U>)
{
    if (_objectPointer != other._objectPointer)
    {
        if (_referenceCounter != nullptr && _objectPointer != nullptr)
            _referenceCounter->DeleteWeakCount();

        _objectPointer    = other._objectPointer;
        _referenceCounter = other._referenceCounter;

        other._objectPointer    = nullptr;
        other._referenceCounter = nullptr;
    }

    return *this;
}


template <SmartPointerType T>
inline Bool WeakPointer<T>::operator==(NullptrType) const noexcept
{
    return _objectPointer == nullptr;
}

template <SmartPointerType T>
inline Bool WeakPointer<T>::operator!=(NullptrType) const noexcept
{
    return _objectPointer != nullptr;
}

template <SmartPointerType T>
inline const ReferenceCounter& WeakPointer<T>::GetStrongCount() const noexcept
{
    return _referenceCounter->GetStrongCount();
}

template <SmartPointerType T>
inline const ReferenceCounter& WeakPointer<T>::GetWeakCount() const noexcept
{
    return _referenceCounter->GetWeakCount();
}

template <SmartPointerType T>
inline WeakPointer<T>::operator Bool() const noexcept
{
    return _objectPointer != nullptr;
}

template <SmartPointerType T>
inline typename WeakPointer<T>::PointerType WeakPointer<T>::GetPointer() const noexcept
{
    return _objectPointer;
}

template <SmartPointerType T>
inline void WeakPointer<T>::Reset() noexcept
{
    if (_objectPointer != nullptr)
    {
        _referenceCounter->DeleteWeakCount();
        _objectPointer    = nullptr;
        _referenceCounter = nullptr;
    }
}

template <SmartPointerType T>
inline Bool WeakPointer<T>::operator==(const WeakPointer<T>& other) const noexcept
{
    return _objectPointer == other._objectPointer;
}

template <SmartPointerType T>
inline Bool WeakPointer<T>::operator!=(const WeakPointer<T>& other) const noexcept
{
    return _objectPointer != other._objectPointer;
}

template <SmartPointerType T>
inline WeakPointer<T>& WeakPointer<T>::operator=(NullptrType) noexcept
{
    Reset();
    return *this;
}

template <SmartPointerType T>
SharedPointer<T> WeakPointer<T>::Generate() const noexcept
{
    if (_objectPointer && _referenceCounter)
    {
        if (_referenceCounter->GetStrongCount() > 0)
        {
            SharedPointer<T> sharedPointer;
            sharedPointer._objectPointer    = _objectPointer;
            sharedPointer._referenceCounter = _referenceCounter;
            sharedPointer._referenceCounter->AddStrongCount();
            return sharedPointer;
        }
        else
            return nullptr;
    }
    else
        return nullptr;
}

// ===> MakeShared vvv

template <SmartPointerType T, class... Args, typename>
inline SharedPointer<T> MakeShared(Args&&... args) requires(std::is_constructible_v<T, Args...>)
{
    return AllocatedMakeShared<T, DefaultMemoryResource, Args...>(std::forward<Args>(args)...);
}

template <SmartPointerType T, typename>
inline SharedPointer<T> MakeShared(Size elementCount) requires(std::is_default_constructible_v<std::remove_all_extents_t<T>>)
{
    return AllocatedMakeShared<T, DefaultMemoryResource>(elementCount);
}

template <SmartPointerType T, MemoryResourceType MemRes, class... Args, typename>
inline SharedPointer<T> AllocatedMakeShared(Args&&... args) requires(std::is_constructible_v<T, Args...>)
{
    // Allocates the memory to accommodate the object and reference counter in the same allocation
    PVoid memory = MemRes::Allocate(sizeof(T) + sizeof(Detail::SmartPointer::ReferenceCounterMakeShared<T, MemRes>), alignof(T));

    // Constructs the object
    if constexpr (IsNothrowConstructible<T, Args...>)
    {
        new ((T*)memory) T(Forward<Args>(args)...);
    }
    else
    {
        try
        {
            new ((T*)memory) T(Forward<Args>(args)...);
        }
        catch (...)
        {
            MemRes::Deallocate(memory);
            throw;
        }
    }

    // Constructs the reference counter
    auto referenceCounterPointer = (Detail::SmartPointer::ReferenceCounterMakeShared<T, MemRes>*)((PVoid)(((T*)memory) + 1));

    new (referenceCounterPointer) Detail::SmartPointer::ReferenceCounterMakeShared<T, MemRes>((T*)memory);

    SharedPointer<T> sharedPointer;

    sharedPointer._objectPointer    = (T*)memory;
    sharedPointer._referenceCounter = referenceCounterPointer;

    if constexpr (!std::is_array_v<T> && std::is_base_of_v<ISharedFromThis, T> && std::is_convertible_v<T*, ISharedFromThis*>)
    {
        T*               objectPointer       = (T*)memory;
        ISharedFromThis* referenceFromThis   = objectPointer;
        referenceFromThis->_objectPtr        = (PVoid)objectPointer;
        referenceFromThis->_referenceCounter = (PVoid)referenceCounterPointer;
    }

    return sharedPointer;
}

template <SmartPointerType T, MemoryResourceType MemRes, typename>
inline SharedPointer<T> AllocatedMakeShared(Size elementCount) requires(std::is_default_constructible_v<std::remove_all_extents_t<T>>)
{
    constexpr auto ElementSize  = sizeof(std::remove_all_extents_t<T>);
    constexpr auto ElementAlign = alignof(std::remove_all_extents_t<T>);
    using PointerType           = std::remove_all_extents_t<T>*;
    using Type                  = std::remove_all_extents_t<T>;

    // Calculates the padding size.
    Int64 offset = ElementAlign - 1 + sizeof(Detail::SmartPointer::MakeSharedHeaderArray);

    // Size of memory to allocate for the array
    auto memorySize = (elementCount * ElementSize) + offset + sizeof(Detail::SmartPointer::ReferenceCounterMakeShared<T, MemRes>);

    // Malloc'ed memory
    PVoid originalMemory = MemRes::Allocate(memorySize, 1);

    // Calculates the aligned memory address.
    PVoid* alignedMemory = (PVoid*)(((Size)(originalMemory) + offset) & ~(alignof(T) - 1)); // Aligned block

    // Stores the size of array in the first bytes of the memory block.
    Detail::SmartPointer::MakeSharedHeaderArray* header = ((Detail::SmartPointer::MakeSharedHeaderArray*)alignedMemory) - 1;

    // Stores the original memory address before the aligned memory address.
    header->ElementCount   = elementCount;
    header->OriginalMemory = originalMemory;

    PointerType objectArray = (PointerType)alignedMemory;

    if constexpr (::std::is_nothrow_default_constructible_v<T>)
    {
        for (Size i = 0; i < elementCount; ++i)
        {
            // Placement new construct
            new (objectArray + i) std::remove_all_extents_t<T>();
        }
    }
    else
    {
        Size constructedElementCount = 0;

        try
        {
            for (Size i = 0; i < elementCount; ++i)
            {
                // Placement new construct
                new (objectArray + i) std::remove_all_extents_t<T>();

                // Increments the constructed element count
                ++constructedElementCount;
            }
        }
        catch (...)
        {
            // Destructs the already constructed elements
            for (Size i = 0; i < constructedElementCount; ++i)
                objectArray[i].~Type();

            // Frees the memory
            MemRes::Deallocate(originalMemory);

            // Rethrows the exception
            throw;
        }
    }

    // Constructs the reference counter
    auto referenceCounterPointer = (Detail::SmartPointer::ReferenceCounterMakeShared<T, MemRes>*)((PVoid)(objectArray + elementCount));

    new (referenceCounterPointer) Detail::SmartPointer::ReferenceCounterMakeShared<T, MemRes>((PointerType)objectArray);

    SharedPointer<T> sharedPointer;
    sharedPointer._objectPointer    = objectArray;
    sharedPointer._referenceCounter = referenceCounterPointer;


    return sharedPointer;
}

template <SmartPointerType T>
SharedPointer<T> ISharedFromThis::CreateSharedPointerFromThis(T& object) noexcept requires(!std::is_array_v<T> && std::is_base_of_v<ISharedFromThis, T> && std::is_convertible_v<T*, ISharedFromThis*>)
{
    ISharedFromThis* sharedFromThis = std::addressof(object);

    SharedPointer<T> sharedPointer  = {};
    sharedPointer._objectPointer    = (T*)sharedFromThis->_objectPtr;
    sharedPointer._referenceCounter = (Detail::SmartPointer::IReferenceCounter*)sharedFromThis->_referenceCounter;

    if (sharedPointer._referenceCounter)
        sharedPointer._referenceCounter->AddStrongCount();

    return sharedPointer;
}

template <SmartPointerType T>
WeakPointer<T> ISharedFromThis::CreateWeakPointerFromThis(T& object) noexcept requires(!std::is_array_v<T> && std::is_base_of_v<ISharedFromThis, T> && std::is_convertible_v<T*, ISharedFromThis*>)
{
    ISharedFromThis* sharedFromThis = std::addressof(object);

    WeakPointer<T> weakPointer    = {};
    weakPointer._objectPointer    = (T*)sharedFromThis->_objectPtr;
    weakPointer._referenceCounter = (Detail::SmartPointer::IReferenceCounter*)sharedFromThis->_referenceCounter;

    if (weakPointer._referenceCounter)
        weakPointer._referenceCounter->AddWeakCount();

    return weakPointer;
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_SMARTPOINTERIMPL_INL
