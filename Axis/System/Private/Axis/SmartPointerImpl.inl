/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_SMARTPOINTERIMPL_INL
#define AXIS_SYSTEM_SMARTPOINTERIMPL_INL
#pragma once

#include "../../Include/Axis/Assert.hpp"
#include "../../Include/Axis/SmartPointer.hpp"

namespace Axis::System
{

namespace Detail::SmartPointer
{

template <typename ValueType, typename Deleter, typename = void>
struct SmartPointerPointerTypeImpl
{
    using Type = RemoveAllExtents<ValueType>*;
};

template <typename ValueType, typename Deleter>
struct SmartPointerPointerTypeImpl<ValueType,
                                   Deleter,
                                   VoidTypeSink<typename Deleter::PointerType>>
{
    using Type = typename Deleter::PointerType;
};

template <typename ValueType, typename Deleter>
struct SmartPointerPointerType
{
    using Type = typename SmartPointerPointerTypeImpl<ValueType, Deleter>::Type;
};

} // namespace Detail::SmartPointer

template <Concept::SmartPointerValue T>
struct DefaultDeleter
{
    DefaultDeleter() noexcept {}

    template <Concept::SmartPointerValue U>
    DefaultDeleter(const DefaultDeleter<U>& other) noexcept
    {}

    template <Concept::SmartPointerValue U>
    DefaultDeleter& operator=(const DefaultDeleter<U>& other) noexcept { return *this; }

    inline void operator()(RemoveAllExtents<T>* objectPointer) noexcept
    {
        if (objectPointer == nullptr)
            return;

        if constexpr (IsBoundedArray<T>)
            Axis::System::DeleteArray(objectPointer);
        else
            Axis::System::Delete(objectPointer);
    }
};

// ===> UniquePointer vvv

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer() noexcept :
    _pair()
{
    _pair.GetFirst() = nullptr;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer(decltype(nullptr)) noexcept :
    _pair()
{
    _pair.GetFirst() = nullptr;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer(PointerType    ptr,
                                                const Deleter& deleter) noexcept :
    _pair(ptr, deleter)
{}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline UniquePointer<T, Deleter>::UniquePointer(UniquePointer<T, Deleter>&& other) noexcept :
    _pair(PerfectForwardTag,
          Move(other._pair.GetFirst()),
          MoveConstructIfNoThrow(other._pair.GetSecond()))
{
    other._pair.GetFirst() = nullptr;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
template <Concept::SmartPointerValue U, Concept::SmartPointerDeleter<U> AnotherDeleter>
inline UniquePointer<T, Deleter>::UniquePointer(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U, T>&& IsNothrowConstructible<Deleter, const AnotherDeleter&>) :
    _pair(PerfectForwardTag,
          Move(other._pair.GetFirst()),
          static_cast<ConditionalType<IsNothrowConstructible<Deleter, AnotherDeleter&&>, AnotherDeleter&&, const AnotherDeleter&>>(other._pair.GetSecond()))
{
    other._pair.GetFirst() = nullptr;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline UniquePointer<T, Deleter>::~UniquePointer() noexcept
{
    _pair.GetSecond()(_pair.GetFirst());
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline UniquePointer<T, Deleter>& UniquePointer<T, Deleter>::operator=(UniquePointer<T, Deleter>&& other) noexcept
{
    if (this == AddressOf(other))
        return *this;

    _pair.GetSecond()(_pair.GetFirst());

    _pair.GetFirst()  = Move(other._pair.GetFirst());
    _pair.GetSecond() = MoveAssignIfNoThrow(other._pair.GetSecond());

    other._pair.GetFirst() = nullptr;

    return *this;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline UniquePointer<T, Deleter>& UniquePointer<T, Deleter>::operator=(decltype(nullptr)) noexcept
{
    Reset();
    return *this;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
template <Concept::SmartPointerValue U, Concept::SmartPointerDeleter<U> AnotherDeleter>
inline UniquePointer<T, Deleter>& UniquePointer<T, Deleter>::operator=(UniquePointer<U, AnotherDeleter>&& other) noexcept requires(ConvertibleFrom<U, T>&& IsNothrowAssignable<Deleter, const AnotherDeleter&>)
{
    _pair.GetSecond()(_pair.GetFirst());

    _pair.GetFirst()  = Move(other._pair.GetFirst());
    _pair.GetSecond() = static_cast<ConditionalType<IsNothrowAssignable<Deleter, AnotherDeleter&&>, AnotherDeleter&&, const AnotherDeleter&>>(other._pair.GetSecond());

    other._pair.GetFirst() = nullptr;

    return *this;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
AddLValueReference<typename UniquePointer<T, Deleter>::ValueType> UniquePointer<T, Deleter>::operator*() const noexcept requires(!Concept::IsSame<T, void> && !IsUnboundedArray<T>)
{
    return *_pair.GetFirst();
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
typename UniquePointer<T, Deleter>::PointerType UniquePointer<T, Deleter>::operator->() const noexcept requires(!Concept::IsSame<T, void> && !IsUnboundedArray<T>)
{
    return _pair.GetFirst();
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline AddLValueReference<typename UniquePointer<T, Deleter>::ValueType> UniquePointer<T, Deleter>::operator[](Size index) const noexcept requires(IsUnboundedArray<T>)
{
    return _pair.GetFirst()[index];
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline Bool UniquePointer<T, Deleter>::operator==(decltype(nullptr)) const noexcept
{
    return _pair.GetFirst() == nullptr;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline Bool UniquePointer<T, Deleter>::operator!=(decltype(nullptr)) const noexcept
{
    return _pair.GetFirst() != nullptr;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline UniquePointer<T, Deleter>::operator Bool() const noexcept
{
    return _pair.GetFirst() != nullptr;
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline typename UniquePointer<T, Deleter>::PointerType UniquePointer<T, Deleter>::GetPointer() const noexcept
{
    return _pair.GetFirst();
}

template <Concept::SmartPointerValue T, Concept::SmartPointerDeleter<T> Deleter>
inline void UniquePointer<T, Deleter>::Reset() noexcept
{
    _pair.GetSecond()(_pair.GetFirst());
}

template <Concept::SmartPointerValue T>
inline WeakPointer<T>::WeakPointer(decltype(nullptr)) noexcept {}

template <Concept::SmartPointerValue T>
inline WeakPointer<T>::WeakPointer(const WeakPointer<T>& other) noexcept :
    BaseType(Detail::SmartPointer::CopyConstructTag{}, other)
{}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline WeakPointer<T>::WeakPointer(const WeakPointer<U>& other) noexcept requires(ConvertibleFrom<U, T>) :
    BaseType(Detail::SmartPointer::CopyConstructTag{}, other) {}

template <Concept::SmartPointerValue T>
inline WeakPointer<T>::WeakPointer(WeakPointer<T>&& other) noexcept :
    BaseType(Detail::SmartPointer::MoveConstructTag{}, Move(other)) {}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline WeakPointer<T>::WeakPointer(WeakPointer<U>&& other) noexcept requires(ConvertibleFrom<U, T>) :
    BaseType(Detail::SmartPointer::MoveConstructTag{}, Move(other)) {}

template <Concept::SmartPointerValue T>
inline WeakPointer<T>& WeakPointer<T>::operator=(const WeakPointer<T>& other) noexcept
{
    if (this == AddressOf(other))
        return *this;

    BaseType::CopyAssign(other);

    return *this;
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline WeakPointer<T>& WeakPointer<T>::operator=(const WeakPointer<U>& other) noexcept requires(ConvertibleFrom<U, T>)
{
    BaseType::CopyAssign(other);

    return *this;
}

template <Concept::SmartPointerValue T>
inline WeakPointer<T>& WeakPointer<T>::operator=(WeakPointer<T>&& other) noexcept
{
    if (this == AddressOf(other))
        return *this;

    BaseType::MoveAssign(Move(other));

    return *this;
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline WeakPointer<T>& WeakPointer<T>::operator=(WeakPointer<U>&& other) noexcept requires(ConvertibleFrom<U, T>)
{
    BaseType::MoveAssign(Move(other));

    return *this;
}

template <Concept::SmartPointerValue T>
inline WeakPointer<T>& WeakPointer<T>::operator=(decltype(nullptr)) noexcept
{
    BaseType::Reset();

    return *this;
}

template <Concept::SmartPointerValue T>
inline void WeakPointer<T>::Reset() noexcept
{
    BaseType::Reset();
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline Bool WeakPointer<T>::operator==(const WeakPointer<U>& other) const noexcept requires(ConvertibleFrom<U, T> || ConvertibleFrom<T, U>)
{
    return BaseType::_resource == other._resource;
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline Bool WeakPointer<T>::operator!=(const WeakPointer<U>& other) const noexcept requires(ConvertibleFrom<U, T> || ConvertibleFrom<T, U>)
{
    return BaseType::_resource != other._resource;
}

template <Concept::SmartPointerValue T>
inline Bool WeakPointer<T>::operator==(decltype(nullptr)) const noexcept
{
    return BaseType::_resource == nullptr;
}

template <Concept::SmartPointerValue T>
inline Bool WeakPointer<T>::operator!=(decltype(nullptr)) const noexcept
{
    return BaseType::_resource != nullptr;
}

template <Concept::SmartPointerValue T>
inline const AtomicReferenceCount& WeakPointer<T>::GetStrongReferenceCount() const noexcept
{
    AXIS_VALIDATE(BaseType::_referenceCounter, "The pointer was nullptr");

    return BaseType::_referenceCounter->StrongCount;
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline SharedPointer<U> WeakPointer<T>::Generate() const noexcept requires(ConvertibleFrom<T, U>)
{
    constexpr auto IncrementIfNotZero = [](AtomicReferenceCount& atomicReferenceCount) -> Bool {
        auto count = atomicReferenceCount.load();

        while (count != 0)
        {
            if (atomicReferenceCount.compare_exchange_strong(count, count + 1, std::memory_order::relaxed))
                return true;
        }
        return false;
    };

    if (BaseType::_referenceCounter != nullptr)
    {
        const auto valid = IncrementIfNotZero(BaseType::_referenceCounter->StrongCount);

        if (valid)
            return SharedPointer<U>(BaseType::_resource, BaseType::_referenceCounter);
        else
            return nullptr;
    }
    else
        return nullptr;
}

template <Concept::SmartPointerValue T>
inline WeakPointer<T>::WeakPointer(PointerType                             pointer,
                                   Detail::SmartPointer::ReferenceCounter* referenceCounterPointer) noexcept :
    BaseType(pointer, referenceCounterPointer) {}

// WeakPointer ^^^
// SharedPointer vvvv

template <Concept::SmartPointerValue T>
inline SharedPointer<T>::SharedPointer(decltype(nullptr)) noexcept {}

template <Concept::SmartPointerValue T>
inline SharedPointer<T>::SharedPointer(const SharedPointer<T>& other) noexcept :
    BaseType(Detail::SmartPointer::CopyConstructTag{}, other)
{}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline SharedPointer<T>::SharedPointer(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U, T>) :
    BaseType(Detail::SmartPointer::CopyConstructTag{}, other) {}

template <Concept::SmartPointerValue T>
inline SharedPointer<T>::SharedPointer(SharedPointer<T>&& other) noexcept :
    BaseType(Detail::SmartPointer::MoveConstructTag{}, Move(other)) {}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline SharedPointer<T>::SharedPointer(SharedPointer<U>&& other) noexcept requires(ConvertibleFrom<U, T>) :
    BaseType(Detail::SmartPointer::MoveConstructTag{}, Move(other)) {}

template <Concept::SmartPointerValue T>
inline SharedPointer<T>& SharedPointer<T>::operator=(const SharedPointer<T>& other) noexcept
{
    if (this == AddressOf(other))
        return *this;

    BaseType::CopyAssign(other);

    return *this;
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline SharedPointer<T>& SharedPointer<T>::operator=(const SharedPointer<U>& other) noexcept requires(ConvertibleFrom<U, T>)
{
    BaseType::CopyAssign(other);

    return *this;
}

template <Concept::SmartPointerValue T>
inline SharedPointer<T>& SharedPointer<T>::operator=(SharedPointer<T>&& other) noexcept
{
    if (this == AddressOf(other))
        return *this;

    BaseType::MoveAssign(Move(other));

    return *this;
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline SharedPointer<T>& SharedPointer<T>::operator=(SharedPointer<U>&& other) noexcept requires(ConvertibleFrom<U, T>)
{
    BaseType::MoveAssign(Move(other));

    return *this;
}

template <Concept::SmartPointerValue T>
inline SharedPointer<T>& SharedPointer<T>::operator=(decltype(nullptr)) noexcept
{
    BaseType::Reset();

    return *this;
}

template <Concept::SmartPointerValue T>
inline void SharedPointer<T>::Reset() noexcept
{
    BaseType::Reset();
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline Bool SharedPointer<T>::operator==(const SharedPointer<U>& other) const noexcept requires(ConvertibleFrom<U, T> || ConvertibleFrom<T, U>)
{
    return BaseType::_resource == other._resource;
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline Bool SharedPointer<T>::operator!=(const SharedPointer<U>& other) const noexcept requires(ConvertibleFrom<U, T> || ConvertibleFrom<T, U>)
{
    return BaseType::_resource != other._resource;
}

template <Concept::SmartPointerValue T>
inline Bool SharedPointer<T>::operator==(decltype(nullptr)) const noexcept
{
    return BaseType::_resource == nullptr;
}

template <Concept::SmartPointerValue T>
inline Bool SharedPointer<T>::operator!=(decltype(nullptr)) const noexcept
{
    return BaseType::_resource != nullptr;
}

template <Concept::SmartPointerValue T>
inline AddLValueReference<typename SharedPointer<T>::ValueType> SharedPointer<T>::operator*() const noexcept requires(!Concept::IsSame<T, void> && !IsUnboundedArray<T>)
{
    return *BaseType::_resource;
}

template <Concept::SmartPointerValue T>
inline typename SharedPointer<T>::PointerType SharedPointer<T>::operator->() const noexcept requires(!Concept::IsSame<T, void> && !IsUnboundedArray<T>)
{
    return BaseType::_resource;
}

template <Concept::SmartPointerValue T>
inline AddLValueReference<typename SharedPointer<T>::ValueType> SharedPointer<T>::operator[](Size index) const noexcept requires(IsUnboundedArray<T>)
{
    return BaseType::_resource[index];
}

template <Concept::SmartPointerValue T>
inline void SharedPointer<T>::Swap(SharedPointer<T>& other) noexcept
{
    auto resourceTemp         = other._resource;
    auto referenceCounterTemp = other._referenceCounter;

    other._resource         = BaseType::_resource;
    other._referenceCounter = BaseType::_referenceCounter;

    BaseType::_resource         = resourceTemp;
    BaseType::_referenceCounter = referenceCounterTemp;
}

namespace Detail::SmartPointer
{

template <Concept::SmartPointerValue T, Concept::MemoryResource MemRes>
class ReferenceCounterMakeShared final : public Detail::SmartPointer::ReferenceCounter // reference counter when creates with `MakeShared` and `AllocatedMakeShared`
{
public:
    ReferenceCounterMakeShared(RemoveAllExtents<T>* objectPointer) noexcept :
        _objectPointer(objectPointer) {}

    inline void DeleteResource() noexcept override final
    {
        static_assert(IsNothrowDestructible<T>, "Couldn't destruct the object, because it wasn't nothrow destructible");
        _objectPointer->~T();
    }

    inline void DeleteThisCounter() noexcept override final
    {
        MemoryResource::Deallocate((PVoid)_objectPointer);
    }

private:
    T* _objectPointer = nullptr; // pointer to the original object
};

template <Concept::SmartPointerValue T, Concept::MemoryResource MemRes>
class ReferenceCounterMakeSharedArray final : public Detail::SmartPointer::ReferenceCounter // reference counter when creates with `MakeSharedArray` and `AllocatedMakeSharedArray`
{
public:
    using ValueType = RemoveAllExtents<T>;

    ReferenceCounterMakeSharedArray(ValueType* objectPointer,
                                    Size       elementCount) noexcept :
        _objectPointer(objectPointer),
        _elementCount(elementCount) {}

    inline void DeleteResource() noexcept override final
    {
        static_assert(IsNothrowDestructible<ValueType>, "Couldn't destruct the object, because it wasn't nothrow destructible.");

        for (Size i = 0; i < _elementCount; ++i)
            _objectPointer[_elementCount - 1 - i].~ValueType();
    }

    inline void DeleteThisCounter() noexcept override final
    {
        MemoryResource::Deallocate((PVoid)_objectPointer);
    }

private:
    ValueType* _objectPointer = nullptr; // pointer to the original object
    const Size _elementCount  = 0;       // number of elements
};

} // namespace Detail::SmartPointer

template <Concept::SmartPointerValue T>
template <Concept::MemoryResource MemRes, class... Args>
inline SharedPointer<T> SharedPointer<T>::MakeShared(Args&&... args) requires(!IsUnboundedArray<T>)
{
    static_assert(!IsVoid<T>, "Couldn't create shared pointer from void");
    static_assert(IsConstructible<RemoveAllExtents<T>, Args...>, "Couldn't construct the object with the given arguments");

    using NewGuardType = Detail::Memory::NewGuard<MemoryResource>;
    using TidyNewGuard = Detail::Memory::TidyGuard<NewGuardType>;

    using ReferenceCounterType = Detail::SmartPointer::ReferenceCounterMakeShared<T, MemRes>;

    struct MakeSharedStruct
    {
        T                    Object;
        ReferenceCounterType ReferenceCounter;
    };

    auto ptr = (MakeSharedStruct*)MemRes::Allocate(sizeof(MakeSharedStruct));

    NewGuardType guard(static_cast<PVoid>(ptr));
    TidyNewGuard tidyGuard(AddressOf(guard));

    new (AddressOf(ptr->ReferenceCounter)) ReferenceCounterType(AddressOf(ptr->Object));
    new (AddressOf(ptr->Object)) T(Forward<Args>(args)...);

    tidyGuard.Target = nullptr;

    return SharedPointer<T>(AddressOf(ptr->Object), AddressOf(ptr->ReferenceCounter));
}

template <Concept::SmartPointerValue T>
template <Concept::MemoryResource MemRes, class... Args>
inline SharedPointer<T> SharedPointer<T>::MakeShared(Size count,
                                                     Args&&... args) requires(IsUnboundedArray<T>)
{
    using ValueType            = RemoveAllExtents<T>;
    using ReferenceCounterType = Detail::SmartPointer::ReferenceCounterMakeSharedArray<T, MemRes>;

    static_assert(IsConstructible<ValueType, Args...>, "Couldn't construct the object with the given arguments");

    constexpr auto MaxElementCount = (std::numeric_limits<Size>::max() - sizeof(ReferenceCounterType)) / sizeof(ValueType);

    AXIS_VALIDATE(count <= MaxElementCount, "The requested amount of elements exceeded maximum array size.");
    AXIS_VALIDATE(count > 0, "element count was zero.");

    using NewArrayGuardType = Detail::Memory::NewArrayGuard<MemoryResource, ValueType>;
    using TidyNewArrayGuard = Detail::Memory::TidyGuard<NewArrayGuardType>;

    PVoid                 rawPtr           = (PVoid)MemoryResource::Allocate(sizeof(ReferenceCounterType) + sizeof(ValueType) * count);
    ValueType*            objectArray      = (ValueType*)rawPtr;
    ReferenceCounterType* referencePointer = (ReferenceCounterType*)((UintPtr)rawPtr + (sizeof(ValueType) * count));

    NewArrayGuardType arrayGuard(objectArray);
    TidyNewArrayGuard tidyGuard(AddressOf(arrayGuard));

    new (referencePointer) ReferenceCounterType(objectArray,
                                                count);

    for (Size i = 0; i < count; ++i)
    {
        new (objectArray + i) ValueType(Forward<Args>(args)...);
        ++arrayGuard.ConstructedCount;
    }

    tidyGuard.Target = nullptr;

    return SharedPointer<T>(objectArray,
                            referencePointer);
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline SharedPointer<T>::operator SharedPointer<U>() const noexcept
{
    Detail::SmartPointer::StrongIncrementer(BaseType::_referenceCounter);

    return SharedPointer<U>((U*)BaseType::_resource, BaseType::_referenceCounter);
}

template <Concept::SmartPointerValue T>
template <Concept::SmartPointerValue U>
inline SharedPointer<T>::operator WeakPointer<U>() const noexcept requires(ConvertibleFrom<U, T>)
{
    Detail::SmartPointer::WeakIncrementer(BaseType::_referenceCounter);

    return WeakPointer<U>(BaseType::_resource, BaseType::_referenceCounter);
}

template <Concept::SmartPointerValue T>
inline const AtomicReferenceCount& SharedPointer<T>::GetStrongReferenceCount() const noexcept
{
    AXIS_VALIDATE(BaseType::_referenceCounter, "The pointer was nullptr");

    return BaseType::_referenceCounter->StrongCount;
}

template <Concept::SmartPointerValue T>
inline SharedPointer<T>::SharedPointer(typename SharedPointer<T>::PointerType  resourcePointer,
                                       Detail::SmartPointer::ReferenceCounter* referenceCounterPointer) noexcept :
    BaseType(resourcePointer,
             referenceCounterPointer) {}

} // namespace Axis::System

#endif // AXIS_SYSTEM_SMARTPOINTERIMPL_INL
