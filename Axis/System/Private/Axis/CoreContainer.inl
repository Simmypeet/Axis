/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_CORECONTAINER_INL
#define AXIS_SYSTEM_CORECONTAINER_INL
#pragma once

#include "../../Include/Axis/Allocator.hpp"
#include "../../Include/Axis/Assert.hpp"
#include "../../Include/Axis/Memory.hpp"
#include "../../Include/Axis/SmartPointer.hpp"
#include "CompressedPair.inl"

namespace Axis
{

namespace System
{

namespace Detail
{

namespace CoreContainer
{

#if defined(AXIS_ENABLE_ITERATOR_DEBUG)
static_assert(IsSame<decltype(AXIS_ENABLE_ITERATOR_DEBUG), bool>, "AXIS_ENABLE_ITERATOR_DEBUG must be a boolean");
constexpr bool DefaultIteratorDebug = AXIS_ENABLE_ITERATOR_DEBUG;
#else
#    ifdef AXIS_DEBUG
constexpr bool DefaultIteratorDebug = true;
#    else
constexpr bool DefaultIteratorDebug = false;
#    endif
#endif

template <class T>
struct TidyGuard // Calls function `Tidy()` on the target on destruction
{
    // Target to call `Tidy()` on (if not null)
    T* Target = nullptr;

    // Calls `Tidy()` on the target
    ~TidyGuard() noexcept
    {
        if (Target) Target->Tidy();
    }
};

/// \note This class is not intended to be used directly
template <RawType IteratorType, RawType VoidPointerType>
struct IteratorTrackerNode
{
    using ThisType        = IteratorTrackerNode<IteratorType, VoidPointerType>;
    using NodePointerType = typename PointerTraits<VoidPointerType>::template Rebind<ThisType>;

    IteratorType*   IteratorPointer = {}; // Pointer to the iterator instance
    NodePointerType Next            = {}; // Pointer to the next node
};

/// \note This class is not intended to be used directly
template <RawType IteratorType, RawType VoidPointerType>
struct IteratorTrackingBase
{
    using NodeType        = IteratorTrackerNode<IteratorType, VoidPointerType>;                 // Iterator tracker node type
    using NodePointerType = typename PointerTratis<VoidPointerType>::template Rebind<NodeType>; // NodeType's pointer type

    static_assert(IsSame<NodePointerType, typename NodeType::NodePointerType>); // Ensure's that NodePointerType is the same as NodeType::NodePointerType

    NodePointerType Head = {}; // Head of the singly linked list

    template <AllocatorType Alloc>
    inline void ClearAll(Alloc& alloc) // Allocator
    {
        using AllocTraits = AllocatorTraits<Alloc>;

        auto current = Head;

        while (current)
        {
            auto next = current->Next;

            AllocTraits::Destroy(alloc, current);
            AllocTraits::Deallocate(alloc, current, 1);

            current = next;
        }

        Head = {};
    }
};

class DebugIteratorContainer; // Container for the debugging iterators
class BaseDebugIterator;

struct ContainerTracker
{
    ContainerTracker(DebugIteratorContainer* container) noexcept :
        DebugContainer(container) {}

    DebugIteratorContainer* DebugContainer = nullptr; // Pointer to the container for the debugging iterators
};

class DebugIteratorContainer // Container for debugging iterators
{
protected:
    DebugIteratorContainer() noexcept
    {
        try
        {
            // Try to allocate the memory for the tracker
            _debuggingTracker = MakeShared<ContainerTracker>(this);
        }
        catch (...)
        {}
    }

    ~DebugIteratorContainer() noexcept
    {
        if (_debuggingTracker != nullptr)
            // Destroy the tracker
            _debuggingTracker->DebugContainer = nullptr;
    }

    DebugIteratorContainer(const DebugIteratorContainer&) = delete;
    DebugIteratorContainer(DebugIteratorContainer&&)      = delete;
    DebugIteratorContainer& operator=(const DebugIteratorContainer&) = delete;
    DebugIteratorContainer& operator=(DebugIteratorContainer&&) = delete;

    // Gets the tracker
    inline const SharedPointer<ContainerTracker>& GetTracker() const noexcept
    {
        return _debuggingTracker;
    }

    inline void AssignIterator(BaseDebugIterator& iterator) const noexcept;

    /// Stores shared pointer to the `ContainerTracker` instance
    ///
    /// All the iterators in this container will share the same `ContainerTracker` instance
    /// when they are created. This is to ensure that the iterators are all valid.
    ///
    /// When the container is destroyed (destructor is called), the `ContainerTracker`'s `DebugContainer`
    /// pointer is set to null to indicate that the container is no longer valid, and so the iterators.
    ///
    /// The iterator which holds this shared pointer will be able to detect if the iterator is safe to dereference.
    SharedPointer<ContainerTracker> _debuggingTracker = nullptr;

    friend class BaseDebugIterator;
};

struct Empty
{
};

class BaseDebugIterator // Base class for all debugging iterators
{
protected:
    void AssignContainer(const DebugIteratorContainer& container) noexcept // Assigns the container to the iterator
    {
        _debuggingTracker = container.GetTracker();
    }

    void BasicValidate() const noexcept // basic validation: container association and container lifetime validity
    {
        if (_skipValidation)
            return;

        AXIS_VALIDATE(_debuggingTracker != nullptr, "The iterator was not associated with a container");

        AXIS_VALIDATE(_debuggingTracker->DebugContainer != nullptr, "The container for the iterator was destroyed");
    }

    SharedPointer<ContainerTracker> _debuggingTracker = nullptr; // Pointer to the container for the debugging iterators
    bool                            _skipValidation   = false;   // In cases where the _debuggingTracker can't be allocated (or whatever reasons), this flag is used to skip validation

    friend class DebugIteratorContainer;
};

inline void DebugIteratorContainer::AssignIterator(BaseDebugIterator& iterator) const noexcept
{
    iterator._debuggingTracker = GetTracker();

    // Failed to allocate the tracker, so skip validation
    if (iterator._debuggingTracker == nullptr)
        iterator._skipValidation = true;
}

template <class T>
inline constexpr T RoundToNextPowerOfTwo(T num) noexcept
{
    if (num == 0)
        return 1;

    num--;

    for (Size i = 1; i < sizeof(T) * CHAR_BIT; i <<= 1)
        num |= (num >> i);

    return num + 1;
}

template <class T>
inline constexpr T Min(T a, T b) noexcept
{
    return a < b ? a : b;
}

/// Checks if adding `a` and `b` will overflow. if overflow is detected throws an exception
/// with the given message.
template <class SizeType, class ThrowClass>
inline void ThrowIfOverflow(SizeType    a,
                            SizeType    b,
                            const Char* message)
{
    if (a > (std::numeric_limits<SizeType>::max() - b))
        throw ThrowClass(message);
}

} // namespace CoreContainer

} // namespace Detail

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_CORECONTAINER_INL
