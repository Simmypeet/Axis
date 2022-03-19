/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ALLOCATORIMPL_INL
#define AXIS_SYSTEM_ALLOCATORIMPL_INL
#pragma once

#include "../../Include/Axis/Allocator.hpp"
#include "../../Include/Axis/Assert.hpp"
#include <limits>

namespace Axis::System
{

template <Concept::Pure T, Concept::MemoryResource MemoryResource>
inline typename Allocator<T, MemoryResource>::ValueTypePointer Allocator<T, MemoryResource>::Allocate(SizeType size)
{
    AXIS_VALIDATE(size <= MaxAllocationSize, "Allocation size exceeded the maximum allowed size.");

    return reinterpret_cast<ValueTypePointer>(MemoryResource::Allocate(size * sizeof(ValueType)));
}

template <Concept::Pure T, Concept::MemoryResource MemoryResource>
inline void Allocator<T, MemoryResource>::Deallocate(ValueTypePointer pointer,
                                                     SizeType         size) noexcept
{
    MemoryResource::Deallocate(reinterpret_cast<PVoid>(pointer));
}

template <Concept::Pure T, Concept::MemoryResource MemoryResource>
template <class... Args>
inline void Allocator<T, MemoryResource>::Construct(ValueTypePointer pointer,
                                                    Args&&... args) noexcept(IsNothrowConstructible<T, Args...>)
{
    static_assert(IsConstructible<T, Args...>, "Couldn't construct the element with the given arguments.");

    new (pointer) T(Forward<Args>(args)...);
}

template <Concept::Pure T, Concept::MemoryResource MemoryResource>
inline void Allocator<T, MemoryResource>::Destruct(ValueTypePointer pointer) noexcept
{
    static_assert(IsNothrowDestructible<T>, "The element couldn't be nothrow destructed.");

    pointer->~T();
}

namespace Detail::Allocator
{

template <class Alloc>
concept AllocatorDefiendEqualityOperators = ::Axis::System::Concept::Pure<Alloc> && noexcept(MakeValue<const Alloc&>() == MakeValue<const Alloc&>()) && noexcept(MakeValue<const Alloc&>() != MakeValue<const Alloc&>());

template <class Alloc, class... Args>
concept AllocatorDefinedConstruct = requires(Alloc                                                       allocator,
                                             typename Alloc::template Pointer<typename Alloc::ValueType> pointer,
                                             Args&&... args)
{
    allocator.Construct(pointer, Forward<Args>(args)...);
};

template <class Alloc, class... Args>
concept AllocatorConstructNoexcept = noexcept(MakeValue<Alloc>().Construct(MakeValue<typename Alloc::template Pointer<typename Alloc::ValueType>>(), MakeValue<Args>()...));

template <class Alloc>
concept AllocatorDestructNoexcept = noexcept(MakeValue<Alloc>().Destruct(MakeValue<typename Alloc::template Pointer<typename Alloc::ValueType>>()));

template <Concept::Allocator Alloc, typename = void>
struct AllocatorEqualityValueImpl
{
    static constexpr AllocatorEquality Value = (AllocatorDefiendEqualityOperators<Alloc> ?
                                                    AllocatorEquality::Compare :
                                                    (IsEmpty<Alloc> ? AllocatorEquality::AlwaysEqual :
                                                                      AllocatorEquality::AlwaysNotEqual));
};

template <Concept::Allocator Alloc>
struct AllocatorEqualityValueImpl<Alloc, VoidTypeSink<EnableIfType<Concept::IsConvertible<decltype(Alloc::Equality), AllocatorEquality>>>>
{
    static constexpr AllocatorEquality Value = Alloc::Equality;
};

template <Concept::Allocator Alloc>
struct AllocatorEqualityValue
{
    static constexpr AllocatorEquality Value = AllocatorEqualityValueImpl<Alloc>::Value;
};

template <Concept::Allocator Alloc, class... Args>
struct AllocatorConstructNoexceptValue
{
    static constexpr Bool Value = AllocatorDefinedConstruct<Alloc, Args...> ? AllocatorConstructNoexcept<Alloc, Args...> : IsNothrowConstructible<typename Alloc::ValueType, Args...>;
};

template <typename T, typename = void>
struct AllocatorPropagateOnContainerCopyAssignmentValueImpl
{
    static constexpr Bool Value = false;
};

template <typename T>
struct AllocatorPropagateOnContainerCopyAssignmentValueImpl<T, VoidTypeSink<EnableIfType<Concept::IsConvertible<decltype(T::PropagateOnContainerCopyAssignment), Bool>>>>
{
    static constexpr Bool Value = T::PropagateOnContainerCopyAssignment;
};

template <typename T, typename = void>
struct AllocatorPropagateOnContainerMoveAssignmentValueImpl
{
    static constexpr Bool Value = false;
};

template <typename T>
struct AllocatorPropagateOnContainerMoveAssignmentValueImpl<T, VoidTypeSink<EnableIfType<Concept::IsConvertible<decltype(T::PropagateOnContainerMoveAssignment), Bool>>>>
{
    static constexpr Bool Value = T::PropagateOnContainerMoveAssignment;
};


template <Concept::Allocator Alloc>
struct AllocatorPropagateOnContainerCopyAssignmentValue
{
    static constexpr Bool Value = AllocatorPropagateOnContainerCopyAssignmentValueImpl<Alloc>::Value;
};

template <Concept::Allocator Alloc>
struct AllocatorPropagateOnContainerMoveAssignmentValue
{
    static constexpr Bool Value = AllocatorPropagateOnContainerMoveAssignmentValueImpl<Alloc>::Value;
};

template <typename T, typename = void>
struct AllocatorSelectOnContainerCopyConstructorNoexceptValueImpl
{
    static constexpr Bool Value = IsNothrowCopyConstructible<T>;
};

template <typename T>
struct AllocatorSelectOnContainerCopyConstructorNoexceptValueImpl<T,
                                                                  VoidTypeSink<EnableIfType<Concept::IsSame<decltype(T::SelectOnContainerCopyConstructor(MakeValue<const T&>())), T>>>>
{
    static constexpr Bool Value = noexcept(T::SelectOnContainerCopyConstructor(MakeValue<const T&>()));
};

template <Concept::Allocator Alloc>
struct AllocatorSelectOnContainerCopyConstructorNoexceptValue
{
    static constexpr Bool Value = AllocatorSelectOnContainerCopyConstructorNoexceptValueImpl<Alloc>::Value;
};

} // namespace Detail::Allocator

template <Concept::Allocator Alloc>
inline typename AllocatorTraits<Alloc>::ValueTypePointer AllocatorTraits<Alloc>::Allocate(Alloc&   allocator,
                                                                                          SizeType size)
{
    return allocator.Allocate(size);
}

template <Concept::Allocator Alloc>
inline void AllocatorTraits<Alloc>::Deallocate(Alloc&           allocator,
                                               ValueTypePointer pointer,
                                               SizeType         size) noexcept
{
    allocator.Deallocate(pointer, size);
}

template <Concept::Allocator Alloc>
template <class... Args>
inline void AllocatorTraits<Alloc>::Construct(Alloc&           allocator,
                                              ValueTypePointer pointer,
                                              Args&&... args) noexcept(Detail::Allocator::AllocatorConstructNoexceptValue<Alloc, Args...>::Value)
{
    if constexpr (Detail::Allocator::AllocatorDefinedConstruct<Alloc, Args...>)
    {
        allocator.Construct(pointer, Forward<Args>(args)...);
    }
    else
    {
        new (pointer) ValueType(Forward<Args>(args)...);
    }
}

template <Concept::Allocator Alloc>
inline void AllocatorTraits<Alloc>::Destruct(Alloc& allocator, ValueTypePointer pointer) noexcept
{
    if constexpr (Detail::Allocator::AllocatorDestructNoexcept<Alloc>)
    {
        allocator.Destruct(pointer);
    }
    else
    {
        pointer->~ValueType();
    }
}

template <Concept::Allocator Alloc>
inline Bool AllocatorTraits<Alloc>::CompareEqual(const Alloc& lhs,
                                                 const Alloc& rhs) noexcept
{
    if constexpr (Equality != AllocatorEquality::Compare)
    {
        return Equality == AllocatorEquality::AlwaysEqual;
    }
    else
    {
        return lhs == rhs;
    }
}

template <Concept::Allocator Alloc>
inline Bool AllocatorTraits<Alloc>::CompareUneqaul(const Alloc& lhs,
                                                   const Alloc& rhs) noexcept
{
    if constexpr (Equality != AllocatorEquality::Compare)
    {
        return Equality == AllocatorEquality::AlwaysNotEqual;
    }
    else
    {
        return lhs != rhs;
    }
}

namespace Detail::Allocator
{

template <class Alloc>
concept DefinedSelectOnContainerCopyConstructor = requires(const Alloc& alloc)
{
    {
        Alloc::SelectOnContainerCopyConstructor(alloc)
        } -> Concept::IsConvertible<Alloc>;
};

} // namespace Detail::Allocator

template <Concept::Allocator Alloc>
inline Alloc AllocatorTraits<Alloc>::SelectOnContainerCopyConstructor(const Alloc& alloc) noexcept(Detail::Allocator::AllocatorSelectOnContainerCopyConstructorNoexceptValue<Alloc>::Value)
{
    if constexpr (Detail::Allocator::DefinedSelectOnContainerCopyConstructor<Alloc>)
    {
        return Alloc::SelectOnContainerCopyConstructor(alloc);
    }
    else
    {
        return alloc;
    }
}

} // namespace Axis::System

#endif // AXIS_SYSTEM_ALLOCATORIMPL_INL
