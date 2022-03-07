#include <Axis/Allocator.hpp>
#include <doctest.h>
#include <unordered_map>

namespace Axis
{

namespace System
{

namespace Test
{

template <AllocatorType Alloc,                          // Underlying allocator to use
          Bool          PropogateOnContainerCopyAssign, // Enable propogate on container copy assign
          Bool          PropogateOnContainerMoveAssign, // Enable propogate on container move assign
          Bool          IsAlwaysEqual,                  // Enable is always equal
          Bool          CompareEqual /* Boolean value to return when comparing two allocator */>
class TestAllocator // Allocator that keeps track of memory allocations.
{
private:
    // Underlying allocator type traits' type
    using UnderlyingAllocTraits = AllocatorTraits<Alloc>;

public:
    using ValueType            = typename UnderlyingAllocTraits::ValueType;            // Allocator's value type
    using PointerType          = typename UnderlyingAllocTraits::PointerType;          // Allocator's pointer type
    using ConstPointerType     = typename UnderlyingAllocTraits::ConstPointerType;     // Allocator's const pointer type.
    using DifferenceType       = typename UnderlyingAllocTraits::DifferenceType;       // Allocator's difference type.
    using SizeType             = typename UnderlyingAllocTraits::SizeType;             // Allocator's size type
    using VoidPointerType      = typename UnderlyingAllocTraits::VoidPointerType;      // Allocator's void pointer type.
    using ConstVoidPointerType = typename UnderlyingAllocTraits::ConstVoidPointerType; // Allocator's const void pointer type.

    // Underlying Allocator
    Alloc Allocator = {};

private:
};

} // namespace Test

} // namespace System

} // namespace Axis
