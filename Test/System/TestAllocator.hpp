#include <Axis/Allocator.hpp>
#include <doctest.h>
#include <unordered_set>

namespace Axis
{

namespace System
{

namespace Test
{

template <MemoryResourceType MemRes>
class TestMemoryResource
{
public:
    inline static PVoid Allocate(Size byteSize,
                                 Size alignment) {}

    inline static void Deallocate(PVoid memoryPtr) noexcept
    {}

    inline static std::size_t GetAllocationCount()

        private:
};

template <RawType T,                                     // Underlying allocator to use
          Bool    PropogateOnContainerCopyAssignmentVal, // Enable propogate on container copy assign
          Bool    PropogateOnContainerMoveAssignmentVal, // Enable propogate on container move assign
          Bool    IsAlwaysEqualVal,                      // Enable is always equal
          Bool    CompareEqualVal /* Boolean value to return when comparing two allocator */>
class TestAllocator // Allocator that keeps track of memory allocations.
{
private:
    // Allocator type
    using Alloc = Allocator<T, DefaultMemoryResource>;

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

    // Default constructor
    TestAllocator() = default;

    // Asserts when allocations are left
    ~TestAllocator() noexcept
    {
        // Should have zero allocation left
        DOCTEST_CHECK(_allocInfoTracker.size() == 0);
    }

    // Copy constructor
    TestAllocator(const TestAllocator& other) :
        MyAlloc(other.MyAlloc) {}

    // Moves constructor
    TestAllocator(TestAllocator&& other) :
        MyAlloc(::Axis::System::Move(other.MyAlloc)),
        _allocInfoTracker(::Axis::System::Move(other._allocInfoTracker))
    {
        // Makes sure that other alloc tracker is cleared.
        other._allocInfoTracker.clear();
    }

    // Copy assignment operator
    TestAllocator& operator=(const TestAllocator& other) noexcept(PropogateOnContainerCopyAssignmentVal ? true : false)
    {
        if (this == ::Axis::System::AddressOf(other))
            return *this;

        // There shouldn't be any memory allocation left!
        DOCTEST_CHECK(_allocInfoTracker.size() == 0);

        MyAlloc = other.MyAlloc;

        return *this;
    }

    // Move assignment operator
    TestAllocator& operator=(TestAllocator&& other) noexcept
    {
        if (this == ::Axis::System::AddressOf(other))
            return *this;

        for (SizeType i = 0; i <)

            MyAlloc = ::Axis::System::Move(other.MyAlloc);

        return *this;
    }

    // Allocates memory
    inline PointerType Allocate(SizeType elementCount)
    {
        PointerType pointer = MyAlloc.Allocate(elementCount);

        _allocInfoTracker.insert({pointer, elementCount});

        return pointer;
    }

    // Deallocates memory
    inline void Deallocate(PointerType memoryPtr, SizeType elementCount) noexcept
    {
        auto it = _allocInfoTracker.find({memoryPtr, elementCount});

        DOCTEST_CHECK(it != _allocInfoTracker.end());

        _allocInfoTracker.erase(it);

        MyAlloc.Deallocate(memoryPtr, elementCount);
    }

    // Constructs object
    template <class... Args>
    inline void Construct(PointerType memoryPtr, Args&&... args) noexcept(noexcept(MyAlloc.Construct(memoryPtr, Forward<Args>(args)...)))
    {
        MyAlloc.Construct(memoryPtr, Forward<Args>(args)...);
    }

    // Destructs object
    inline void Destruct(PointerType memoryPtr) noexcept
    {
        MyAlloc.Destruct(memoryPtr);
    }

    // Is always equal
    using IsAlwaysEqual = ConditionalType<IsAlwaysEqualVal, TrueType, FalseType>;

    // Propogate on container copy assignment
    using PropagateOnContainerCopyAssignment = ConditionalType<PropogateOnContainerCopyAssignmentVal, TrueType, FalseType>;

    // Propogate on container move assignment
    using PropagateOnContainerMoveAssignment = ConditionalType<PropogateOnContainerMoveAssignmentVal, TrueType, FalseType>;

    // Underlying Allocator
    Alloc MyAlloc = {};

    inline constexpr Bool operator==(const TestAllocator& other) const noexcept { return CompareEqualVal; }

    inline constexpr Bool operator!=(const TestAllocator& other) const noexcept { return !CompareEqualVal; }

private:
    struct AllocationInfo // Memory allocation info
    {
        PointerType Pointer      = {}; // pointer to the memory allocation
        SizeType    ElementCount = {}; // number of element in the allocation

        inline Bool operator==(const AllocationInfo& other) const noexcept { return Pointer == other.Pointer && ElementCount == other.ElementCount; }
        inline Bool operator!=(const AllocationInfo& other) const noexcept { return Pointer != other.Pointer || ElementCount != other.ElementCount; }

        struct Hasher // Hasher object
        {
            std::size_t operator()(const AllocationInfo& object) const
            {
                std::size_t hashPointer      = (std::size_t)(object.Pointer);
                std::size_t hashElementCount = (std::size_t)(object.ElementCount);

                hashPointer ^= hashElementCount + 0x9e3779b9 + (hashPointer << 6) + (hashPointer >> 2);

                return hashPointer;
            }
        };
    };

    std::unordered_set<AllocationInfo, typename AllocationInfo::Hasher, std::equal_to<AllocationInfo>, std::allocator<AllocationInfo>> _allocInfoTracker = {}; // Tracks memory allocation.
};

} // namespace Test

} // namespace System

} // namespace Axis
