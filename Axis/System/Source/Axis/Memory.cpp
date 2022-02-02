/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/SystemPch.hpp>

#include <Axis/Assert.hpp>
#include <Axis/Exception.hpp>
#include <Axis/Memory.hpp>
#include <functional>


namespace Axis
{

PVoid MallocAllocator::Allocate(Size size, Size alignment)
{
    // Arguments validation
    if ((alignment & (alignment - 1)) != 0)
        throw InvalidArgumentException("`alignment` was not a power of two!");

    // Calculates the padding size.
    Int64 offset = alignment - 1 + sizeof(PVoid);

    // Malloc'ed memory
    PVoid originalMemory = std::malloc(size + offset);

    // Failed to allocate memory
    if (originalMemory == nullptr)
        throw OutOfMemoryException();

    // Calculates the aligned memory address.
    PVoid* alignedMemory = (PVoid*)(((Size)(originalMemory) + offset) & ~(alignment - 1)); // Aligned block

    // Stores the original memory address before the aligned memory address.
    alignedMemory[-1] = originalMemory;

    return (PVoid)alignedMemory;
}

void MallocAllocator::Deallocate(PVoid ptr) noexcept
{
    std::free(((PVoid*)ptr)[-1]);
}

// Allocator for the pool
class FixedPoolAllocator
{
public:
    // Constructor
    FixedPoolAllocator(Size size,
                       Size alignment) noexcept :
        _size(size),
        _alignment(alignment) {}

    FixedPoolAllocator(const FixedPoolAllocator&) = delete;

    FixedPoolAllocator(FixedPoolAllocator&& other) noexcept :
        _size(other._size),
        _alignment(other._alignment),
        _memoryBlockHeader(other._memoryBlockHeader)
    {
        other._memoryBlockHeader = nullptr;
    }

    ~FixedPoolAllocator() noexcept
    {
        // Iterates over the memory blocks and deallocates them.
        auto currentBlock = _memoryBlockHeader;

        while (currentBlock)
        {
            auto next = currentBlock->Next;

            std::free(currentBlock->AllocatedOriginalPointer);

            currentBlock = next;
        }
    }

    // Allocates memory with the given size and alignment
    PVoid Allocate()
    {
        // There are available blocks
        if (_memoryBlockHeader)
        {
            auto block = _memoryBlockHeader;

            // Gets the next block
            _memoryBlockHeader = _memoryBlockHeader->Next;

            // Forwards the block
            return (PVoid)((Size)block + sizeof(MemoryBlockHeader));
        }
        else
        {
            // Calculates the padding size.
            Int64 offset = _alignment - 1 + sizeof(MemoryBlockHeader);

            // Malloc'ed memory
            PVoid originalMemory = std::malloc(_size);

            // Failed to allocate memory
            if (originalMemory == nullptr)
                throw OutOfMemoryException();

            // Calculates the aligned memory address.
            PVoid* alignedMemory = (PVoid*)(((Size)(originalMemory) + offset) & ~(_alignment - 1)); // Aligned block

            ((MemoryBlockHeader*)alignedMemory)[-1].Next                     = nullptr;
            ((MemoryBlockHeader*)alignedMemory)[-1].AllocatedOriginalPointer = originalMemory;
            ((MemoryBlockHeader*)alignedMemory)[-1].Pool                     = this;

            return (PVoid)alignedMemory;
        }
    }

    // Returns the memory block to the pool
    void Return(PVoid userPtr) noexcept
    {
        // Backwards the userPtr to the memory block header
        auto block = (MemoryBlockHeader*)((Size)userPtr - sizeof(MemoryBlockHeader));

        // Assigns it to the free list
        block->Next = _memoryBlockHeader;

        // Assigns the new free list
        _memoryBlockHeader = block;
    }

    struct MemoryBlockHeader
    {
        MemoryBlockHeader*  Next                     = nullptr; // <- Points to the next memory block in the pool
        PVoid               AllocatedOriginalPointer = nullptr; // <- Points to the original memory block pointer
        FixedPoolAllocator* Pool                     = nullptr; // <- Points to the pool
    };

private:
    Size               _size              = 0;       // <-- Size of the memory block
    Size               _alignment         = 0;       // <-- Alignment of the memory block
    MemoryBlockHeader* _memoryBlockHeader = nullptr; // <-- Memory block header
};

// Struct for retrieving FixedPoolAllocator
struct PoolAllocatorKey
{
    Size MemorySize; // <-- Size of the memory block
    Size Alignment;  // <-- Alignment of the memory block

    // Gets the hash code of the key
    struct Hash
    {
        inline Size operator()(const PoolAllocatorKey& key) const noexcept
        {
            return (Size)key.MemorySize ^ (Size)key.Alignment;
        }
    };

    // Checks if two keys are the same
    struct Equal
    {
        inline bool operator()(const PoolAllocatorKey& lhs, const PoolAllocatorKey& rhs) const noexcept
        {
            return lhs.MemorySize == rhs.MemorySize && lhs.Alignment == rhs.Alignment;
        }
    };
};

// Allocator map for the pool
using PoolAllocatorMap = std::unordered_map<PoolAllocatorKey, std::unique_ptr<FixedPoolAllocator>, PoolAllocatorKey::Hash, PoolAllocatorKey::Equal>;

static PoolAllocatorMap s_poolAllocatorMap      = {}; // Global pool allocator map
static std::mutex       s_poolAllocatorMapMutex = {}; // Mutex for accessing the pool allocator map

PVoid PoolAllocator::Allocate(Size size,
                              Size alignment)
{
    // Arguments validation
    if ((alignment & (alignment - 1)) != 0)
        throw InvalidArgumentException("`alignment` was not a power of two!");

    // Calculates the actual size to allocate
    Size actualSize = size + sizeof(FixedPoolAllocator::MemoryBlockHeader) + alignment - 1;

    // Round to the next nearest power of 2
    actualSize--;

    for (Size i = 1; i < sizeof(Size) * CHAR_BIT; i <<= 1)
        actualSize |= (actualSize >> i);

    actualSize = actualSize + 1;

    // Gets the key
    PoolAllocatorKey key = {actualSize, alignment};

    // Gets the allocator
    std::lock_guard<std::mutex> lock(s_poolAllocatorMapMutex);

    auto it = s_poolAllocatorMap.find(key);

    // Allocator not found
    if (it == s_poolAllocatorMap.end())
    {
        // Inserts the allocator
        s_poolAllocatorMap.insert({key, std::move(std::make_unique<FixedPoolAllocator>(actualSize, alignment))});

        // Gets the allocator
        it = s_poolAllocatorMap.find(key);
    }

    // Gets the allocator
    auto& allocator = it->second;

    // Allocates memory
    return allocator->Allocate();
}

void PoolAllocator::Deallocate(PVoid ptr) noexcept
{
    if (ptr == nullptr)
        return;

    // Backwards the userPtr to the memory block header
    auto block = (FixedPoolAllocator::MemoryBlockHeader*)((Size)ptr - sizeof(FixedPoolAllocator::MemoryBlockHeader));

    // Gets the allocator
    auto allocator = block->Pool;

    std::lock_guard<std::mutex> lock(s_poolAllocatorMapMutex);

    // Returns the memory block to the pool
    allocator->Return(ptr);
}

} // namespace Axis