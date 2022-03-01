#ifndef AXIS_SYSTEM_LISTIMPL_INL
#define AXIS_SYSTEM_LISTIMPL_INL
#include "Axis/Trait.hpp"
#include "Axis/Utility.hpp"
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/List.hpp"

namespace Axis
{

namespace System
{

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
struct List<T, Alloc, IteratorDebugging>::ContainerHolder
{
    AllocType* AllocatorPointer = nullptr;
    Data       Data             = {};

    inline void Tidy() noexcept
    {
        for (SizeType i = SizeType(0); i < Data.InitializedSize; ++i)
            AllocTraits::Destruct(*AllocatorPointer, Data.Begin + i);

        // Deallocates the memory
        AllocTraits::Deallocate(*AllocatorPointer, Data.Begin, Data.AllocatedSize);
    }
};

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
template <class IteratorPointerType, class IteratorReferenceType>
class List<T, Alloc, IteratorDebugging>::BaseIterator final : private ConditionalType<IteratorDebugging, Detail::BaseDebugIterator, Detail::Empty>
{
private:
    using BaseType = ConditionalType<IteratorDebugging, Detail::BaseDebugIterator, Detail::Empty>;
    using ListType = List<T, Alloc, IteratorDebugging>;

public:
    using DifferenceType = typename ListType::DifferenceType;
    using SizeType       = typename ListType::SizeType;

private:
    IteratorPointerType _currentPointer = nullptr; // Pointer to the current element

public:
    BaseIterator() noexcept = default;
    BaseIterator(NullptrType) noexcept {}
    BaseIterator(const BaseIterator&) noexcept = default;
    BaseIterator(BaseIterator&&) noexcept      = default;
    BaseIterator& operator=(const BaseIterator&) noexcept = default;
    BaseIterator& operator=(BaseIterator&&) noexcept = default;

private:
    BaseIterator(PointerType pointer) :
        _currentPointer(pointer) {}

public:
    // Operators
    inline IteratorReferenceType operator*() const noexcept
    {
        if constexpr (IteratorDebugging)
        {
            if (!BaseType::_skipValidation)
            {
                BaseType::BasicValidate();

                auto& listData = ((ListType*)BaseType::_debuggingTracker.GetPointer())->_dataAllocPair.GetFirst();

                // Checks if _currentPointer is within the bounds of the list
                AXIS_VALIDATE(_currentPointer >= listData.Begin && _currentPointer < listData.Begin + listData.InitializedSize, "Iterator was out of bounds!");
            }
        }

        return *_currentPointer;
    }

    inline IteratorPointerType operator->() const noexcept
    {
        if constexpr (IteratorDebugging)
        {
            if (!BaseType::_skipValidation)
            {
                BaseType::BasicValidate();

                auto& listData = ((ListType*)BaseType::_debuggingTracker.GetPointer())->_dataAllocPair.GetFirst();

                // Checks if _currentPointer is within the bounds of the list
                AXIS_VALIDATE(_currentPointer >= listData.Begin && _currentPointer < listData.Begin + listData.InitializedSize, "Iterator was out of bounds!");
            }
        }

        return _currentPointer;
    }

    // Comparison operators
    template <class OtherIteratorPointerType, class OtherIteratorReferenceType>
    inline bool operator==(const BaseIterator<OtherIteratorPointerType, OtherIteratorReferenceType>& other) const noexcept
    {
        return _currentPointer == other._currentPointer;
    }

    template <class OtherIteratorPointerType, class OtherIteratorReferenceType>
    inline bool operator!=(const BaseIterator<OtherIteratorPointerType, OtherIteratorReferenceType>& other) const noexcept
    {
        return _currentPointer != other._currentPointer;
    }

    template <class OtherIteratorPointerType, class OtherIteratorReferenceType>
    inline bool operator<(const BaseIterator<OtherIteratorPointerType, OtherIteratorReferenceType>& other) const noexcept
    {
        return _currentPointer < other._currentPointer;
    }

    template <class OtherIteratorPointerType, class OtherIteratorReferenceType>
    inline bool operator>(const BaseIterator<OtherIteratorPointerType, OtherIteratorReferenceType>& other) const noexcept
    {
        return _currentPointer > other._currentPointer;
    }

    template <class OtherIteratorPointerType, class OtherIteratorReferenceType>
    inline bool operator<=(const BaseIterator<OtherIteratorPointerType, OtherIteratorReferenceType>& other) const noexcept
    {
        return _currentPointer <= other._currentPointer;
    }

    template <class OtherIteratorPointerType, class OtherIteratorReferenceType>
    inline bool operator>=(const BaseIterator<OtherIteratorPointerType, OtherIteratorReferenceType>& other) const noexcept
    {
        return _currentPointer >= other._currentPointer;
    }

    // Arithmetic operators
    inline BaseIterator operator+(const DifferenceType difference) const noexcept // Addition
    {
        BaseIterator copy = *this;
        copy._currentPointer += difference;
        return copy;
    }

    inline BaseIterator operator-(const DifferenceType difference) const noexcept // Subtraction
    {
        BaseIterator copy = *this;
        copy._currentPointer -= difference;
        return copy;
    }

    inline BaseIterator& operator+=(const DifferenceType difference) noexcept // Addition assignment
    {
        _currentPointer += difference;
        return *this;
    }

    inline BaseIterator& operator-=(const DifferenceType difference) noexcept // Subtraction assignment
    {
        _currentPointer -= difference;
        return *this;
    }

    template <class OtherIteratorPointerType, class OtherIteratorReferenceType>
    inline DifferenceType operator-(const BaseIterator<OtherIteratorPointerType, OtherIteratorReferenceType>& other) const noexcept // Difference
    {
        return _currentPointer - other._currentPointer;
    }

    // Increment and decrement operators
    inline BaseIterator& operator++() noexcept // Pre-increment
    {
        ++_currentPointer;
        return *this;
    }

    inline BaseIterator operator++(int) noexcept // Post-increment
    {
        BaseIterator copy = *this;
        ++_currentPointer;
        return copy;
    }

    inline BaseIterator& operator--() noexcept // Pre-decrement
    {
        --_currentPointer;
        return *this;
    }

    inline BaseIterator operator--(int) noexcept // Post-decrement
    {
        BaseIterator copy = *this;
        --_currentPointer;
        return copy;
    }

    friend class List<T, Alloc, IteratorDebugging>;
};

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::Reserve(SizeType elementCount)
{
    if (elementCount <= _dataAllocPair.GetFirst().AllocatedSize)
        return;

    auto newDataCopy = CreateCopy(elementCount);

    if (newDataCopy.Second)
        Tidy();

    _dataAllocPair.GetFirst() = newDataCopy.First;
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
inline T& List<T, Alloc, IteratorDebugging>::Append(const T& value)
{
    return EmplaceBack<const T&>(value);
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
inline T& List<T, Alloc, IteratorDebugging>::Append(T&& value)
{
    return EmplaceBack<T&&>(Axis::System::Move(value));
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
template <RandomAccessReadIterator<T> IteratorType>
inline void List<T, Alloc, IteratorDebugging>::Append(const IteratorType& begin,
                                                      const IteratorType& end)
{
    if (begin == end)
        return;

    if (begin > end)
        throw InvalidArgumentException("`begin` was greater than `end`!");

    SizeType elementToInsert = (SizeType)(end - begin);

    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    Reserve(Detail::RoundToNextPowerOfTwo(data.InitializedSize + elementToInsert));

    struct AppendGuard
    {
        Data*      Data           = {};
        AllocType* Allocator      = {};
        SizeType   AppendingIndex = {}; // The first index of the elements to append

        inline void Tidy() noexcept
        {
            auto appendedCount = Data->InitializedSize - AppendingIndex;

            for (SizeType i = 0; i < appendedCount; ++i)
                AllocTraits::Destruct(*Allocator, Data->Begin + AppendingIndex + i);
        }
    };

    AppendGuard appendGuard = {
        AddressOf(data),
        AddressOf(alloc),
        data.InitializedSize};

    Detail::TidyGuard<AppendGuard*> guard = {AddressOf(appendGuard)};

    for (auto it = begin; it != end; ++it)
    {
        const auto& value = *it;
        AllocTraits::Construct(alloc, data.Begin + data.InitializedSize, value);
        ++data.InitializedSize;
    }

    guard.Target = nullptr;
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::PopBack() noexcept
{
    if (_dataAllocPair.GetFirst().InitializedSize == 0)
        return;

    AllocTraits::Destruct(_dataAllocPair.GetSecond(), _dataAllocPair.GetFirst().Begin + (_dataAllocPair.GetFirst().InitializedSize - 1));
    --_dataAllocPair.GetFirst().InitializedSize;
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::RemoveAt(SizeType index,
                                                        SizeType count)
{
    if (count == 0)
        return;

    if (index >= _dataAllocPair.GetFirst().InitializedSize)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    if (index + count > _dataAllocPair.GetFirst().InitializedSize)
        throw ArgumentOutOfRangeException("`count` was out of range!");

    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    // Uses move / copy assign to move the elements (if nothrow)
    constexpr bool Assign = IsNoThrowMoveConstructible<T> || IsNoThrowCopyAssignable<T>;

    // Uses move / copy construct to move the elements (if nothrow)
    constexpr bool Construct = IsNoThrowMoveConstructible<T> || IsNoThrowCopyConstructible<T>;

    if constexpr (Assign || Construct)
    {
        auto moveCount = data.InitializedSize - (index + count);

        if constexpr (Construct)
        {
            for (SizeType i = 0; i < count; ++i)
                AllocTraits::Destruct(alloc, data.Begin + index + i);
        }

        for (SizeType i = 0; i < moveCount; ++i)
        {
            const SizeType sourceIndex = index + count + i;
            const SizeType destIndex   = index + i;

            if constexpr (Assign)
            {
                data.Begin[destIndex] = ::Axis::System::MoveAssignIfNoThrow(data.Begin[sourceIndex]);
            }
            else if constexpr (Construct)
            {
                // Moves / copies and destructs
                AllocTraits::Construct(alloc, data.Begin + destIndex, ::Axis::System::MoveConstructIfNoThrow(data.Begin[sourceIndex]));

                AllocTraits::Destruct(alloc, data.Begin + sourceIndex);
            }
        }

        if constexpr (Assign)
        {
            for (SizeType i = 0; i < count; ++i)
                AllocTraits::Destruct(alloc, data.Begin + (data.InitializedSize - 1 - i));
        }

        // Updates the initialized size
        data.InitializedSize -= count;
    }
    else
    {
        // Allocates new memory and copies the old elements and skips the ones to remove
        ContainerHolder newDataHolder = {
            {AllocTraits::Allocate(alloc, data.InitializedSize - count),
             data.InitializedSize - count,
             0},
            AddressOf(alloc)};

        Detail::TidyGuard<ContainerHolder*> guard = {AddressOf(newDataHolder)};

        // Copies / moves the old elements that are before the ones to remove
        for (SizeType i = 0; i < index; ++i)
        {
            AllocTraits::Construct(alloc, newDataHolder.GetFirst().Begin + i, ::Axis::System::MoveConstructIfNoThrow(data.Begin + i));
            ++newDataHolder.Data.InitializedSize;
        }

        // Copies / moves the old elements that are after the ones to remove
        for (SizeType i = index + count; i < data.InitializedSize; ++i)
        {
            AllocTraits::Construct(alloc, newDataHolder.GetFirst().Begin + i - count, ::Axis::System::MoveConstructIfNoThrow(data.Begin + i));
            ++newDataHolder.Data.InitializedSize;
        }

        guard.Target = nullptr;

        Tidy();

        data = newDataHolder.Data;
    }
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::Tidy() noexcept
{
    Data&      dataInstance  = _dataAllocPair.GetFirst();
    AllocType& allocInstance = _dataAllocPair.GetSecond();

    for (SizeType i = 0; i < dataInstance.InitializedSize; ++i)
        AllocTraits::Destruct(allocInstance, dataInstance.Begin + i);

    if (dataInstance.Begin)
        AllocTraits::Deallocate(allocInstance, dataInstance.Begin, dataInstance.AllocatedSize);

    dataInstance.Begin           = PointerType(nullptr);
    dataInstance.AllocatedSize   = SizeType(0);
    dataInstance.InitializedSize = SizeType(0);
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::TidyData(const Data& data) noexcept
{
    Data&      dataInstance  = _dataAllocPair.GetFirst();
    AllocType& allocInstance = _dataAllocPair.GetSecond();

    for (SizeType i = 0; i < dataInstance.InitializedSize; ++i)
        AllocTraits::Destruct(allocInstance, dataInstance.Begin + i);

    if (dataInstance.Begin)
        AllocTraits::Deallocate(allocInstance, dataInstance.Begin, dataInstance.AllocatedSize);
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
inline Pair<typename List<T, Alloc, IteratorDebugging>::Data, Bool> List<T, Alloc, IteratorDebugging>::CreateCopy(SizeType elementCount)
{
    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    if (elementCount <= data.AllocatedSize)
        return {data, false};

    ContainerHolder containerHolder = {AddressOf(alloc),
                                       {AllocTraits::Allocate(alloc, elementCount), elementCount, 0}};

    Detail::TidyGuard<ContainerHolder*> guard = {AddressOf(containerHolder)};

    for (SizeType i = 0; i < data.InitializedSize; ++i)
    {
        AllocTraits::Construct(alloc, containerHolder.Data.Begin + i, ::Axis::System::MoveConstructIfNoThrow(data.Begin[i]));
        ++containerHolder.Data.InitializedSize;
    }

    guard.Target = nullptr;

    return {containerHolder.Data, true};
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::SizeType List<T, Alloc, IteratorDebugging>::GetCapacity() const noexcept
{
    return _dataAllocPair.GetFirst().AllocatedSize;
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
template <Bool ForceNewAllocation>
inline Pair<typename List<T, Alloc, IteratorDebugging>::Data, Bool> List<T, Alloc, IteratorDebugging>::CreateSpace(SizeType index,
                                                                                                                   SizeType elementCount)
{
    constexpr bool constructNoExcept = IsNoThrowCopyConstructible<T> || IsNoThrowMoveConstructible<T>;
    constexpr bool assignNoExcept    = IsNoThrowCopyAssignable<T> || IsNoThrowMoveAssignable<T>;

    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    const bool allocateNewMemory = ForceNewAllocation || (data.InitializedSize + elementCount > data.AllocatedSize) || !constructNoExcept;

    if (allocateNewMemory)
    {
        Data newData = {
            AllocTraits::Allocate(alloc, Detail::RoundToNextPowerOfTwo(data.InitializedSize + elementCount))};
    }
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
template <class... Args>
inline T& List<T, Alloc, IteratorDebugging>::EmplaceBack(Args&&... args)
{
    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    Reserve(Detail::RoundToNextPowerOfTwo(data.InitializedSize + 1));

    AllocTraits::Construct(alloc, data.Begin, Forward<Args>(args)...);

    ++data.InitializedSize;

    return data.Begin[data.InitializedSize - 1];
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
template <class Lambda>
inline void List<T, Alloc, IteratorDebugging>::ConstructContinuousContainer(SizeType elementCount, const Lambda& construct)
{
    Detail::TidyGuard<ThisType*> guard = {this};

    // Allocate memory
    Data&      dataInstance  = _dataAllocPair.GetFirst();
    AllocType& allocInstance = _dataAllocPair.GetSecond();

    // Assigns data
    dataInstance.Begin         = AllocTraits::Allocate(allocInstance, elementCount);
    dataInstance.AllocatedSize = elementCount;

    construct(elementCount);

    guard.Target = nullptr;
}

// Default constructor
template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List() noexcept(DefaultConstructorNoexcept) :
    _dataAllocPair() {}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::~List() noexcept { Tidy(); }

// Constructor with allocator instance
template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List(const AllocType& allocator) noexcept(AllocatorCopyConstructorNoexcept) :
    _dataAllocPair(Data(), allocator) {}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List(SizeType elementCount, const AllocType& allocator) :
    _dataAllocPair(Data(), allocator)
{
    auto construct = [this](SizeType elementCount) -> void {
        for (SizeType i = 0; i < elementCount; ++i)
        {
            AllocTraits::Construct(_dataAllocPair.GetSecond(), _dataAllocPair.GetFirst().Begin + i);
            _dataAllocPair.GetFirst().InitializedSize++;
        }
    };

    ConstructContinuousContainer(elementCount, construct);
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List(SizeType elementCount, const T& value, const AllocType& allocator) :
    _dataAllocPair(Data(), allocator)
{
    auto construct = [&](SizeType elementCount) -> void {
        for (SizeType i = 0; i < elementCount; ++i)
        {
            AllocTraits::Construct(_dataAllocPair.GetSecond(), _dataAllocPair.GetFirst().Begin + i, value);
            ++_dataAllocPair.GetFirst().InitializedSize;
        }
    };

    ConstructContinuousContainer(elementCount, construct);
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List(std::initializer_list<T> list, const AllocType& allocator) :
    _dataAllocPair(Data(), allocator)
{
    auto construct = [&](SizeType elementCount) -> void {
        for (SizeType i = 0; i < elementCount; ++i)
        {
            AllocTraits::Construct(_dataAllocPair.GetSecond(), _dataAllocPair.GetFirst().Begin + i, *(list.begin() + i));
            ++_dataAllocPair.GetFirst().InitializedSize;
        }
    };

    ConstructContinuousContainer(list.size(), construct);
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List(const List& other) :
    _dataAllocPair(Data(), other._dataAllocPair.GetSecond())
{
    auto construct = [&](SizeType elementCount) -> void {
        for (SizeType i = 0; i < elementCount; ++i)
        {
            AllocTraits::Construct(_dataAllocPair.GetSecond(), _dataAllocPair.GetFirst().Begin + i, *(other._dataAllocPair.GetFirst().Begin + i));
            ++_dataAllocPair.GetFirst().InitializedSize;
        }
    };

    ConstructContinuousContainer(other._dataAllocPair.GetFirst().InitializedSize, construct);
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List(List&& other) noexcept(MoveConstructorNoexcept) :
    _dataAllocPair(PerfectForwardTag, Data(other._dataAllocPair.GetFirst()), Axis::System::Move(other._dataAllocPair.GetSecond()))
{
    auto& otherData           = other._dataAllocPair.GetFirst();
    otherData.Begin           = PointerType(nullptr);
    otherData.AllocatedSize   = SizeType(0);
    otherData.InitializedSize = SizeType(0);
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>& List<T, Alloc, IteratorDebugging>::operator=(const List<T, Alloc, IteratorDebugging>& other)
{
    if (this == AddressOf(other))
        return *this;

    auto& thisData   = _dataAllocPair.GetFirst();
    auto& thisAlloc  = _dataAllocPair.GetSecond();
    auto& otherData  = other._dataAllocPair.GetFirst();
    auto& otherAlloc = other._dataAllocPair.GetSecond();

    // Copy assignment
    if constexpr (AllocTraits::PropagateOnContainerCopyAssignment)
    {
        // Tidies this instance
        Tidy();

        // Copy assignment
        thisAlloc = otherAlloc;
    }


    // Checks if after the allocator's copy assignment, the allocator is the same
    const bool propagatedEqual = AllocTraits::PropagateOnContainerCopyAssignment ? (AllocTraits::IsAlwaysEqual ? true : thisAlloc == otherAlloc) : true;

    const bool useOldMemoryOnlyAssignment                      = propagatedEqual && thisData.InitializedSize >= otherData.InitializedSize && IsNoThrowCopyAssignable<ValueType>;
    const bool useOldMemoryWithAssignmentAndExtraConstructions = propagatedEqual && thisData.InitializedSize < otherData.InitializedSize && thisData.AllocatedSize >= otherData.InitializedSize && IsNoThrowCopyAssignable<ValueType> && IsNoThrowCopyConstructible<T>;
    const bool spareMemory                                     = propagatedEqual && thisData.AllocatedSize >= otherData.InitializedSize && IsNoThrowCopyConstructible<T>;

    if (useOldMemoryOnlyAssignment)
    {
        // Prevents ill-formedness
        if constexpr (IsNoThrowCopyAssignable<ValueType>)
        {
            // Assigns
            for (SizeType i = SizeType(0); i < otherData.InitializedSize; ++i)
                *(thisData.Begin + i) = *(otherData.Begin + i);

            // Destructs the rest
            for (SizeType i = otherData.InitializedSize; i < thisData.InitializedSize; ++i)
                AllocTraits::Destruct(_dataAllocPair.GetSecond(), thisData.Begin + i);

            thisData.InitializedSize = otherData.InitializedSize;
        }
    }
    else if (useOldMemoryWithAssignmentAndExtraConstructions)
    {
        if constexpr (IsNoThrowCopyAssignable<ValueType> && IsNoThrowCopyConstructible<T>)
        {
            auto assignRange = thisData.InitializedSize < otherData.InitializedSize ? thisData.InitializedSize : otherData.InitializedSize;

            // Assigns
            for (SizeType i = SizeType(0); i < assignRange; ++i)
                *(thisData.Begin + i) = MoveAssignIfNoThrow(*(otherData.Begin + i));

            // Constructs
            for (SizeType i = thisData.InitializedSize; i < otherData.InitializedSize; ++i)
                AllocTraits::Construct(_dataAllocPair.GetSecond(), thisData.Begin + i, *(otherData.Begin + i));

            thisData.InitializedSize = otherData.InitializedSize;
        }
    }
    else if (spareMemory)
    {
        if constexpr (IsNoThrowCopyConstructible<T>)
        {
            // Destructs all the elements
            for (SizeType i = SizeType(0); i < thisData.InitializedSize; ++i)
                AllocTraits::Destruct(_dataAllocPair.GetSecond(), thisData.Begin + i);

            // Constructs all the elements
            for (SizeType i = SizeType(0); i < otherData.InitializedSize; ++i)
                AllocTraits::Construct(_dataAllocPair.GetSecond(), thisData.Begin + i, *(otherData.Begin + i));

            thisData.InitializedSize = otherData.InitializedSize;
        }
    }
    else
    {
        Data newThisData = {
            AllocTraits::Allocate(_dataAllocPair.GetSecond(), otherData.InitializedSize),
            otherData.InitializedSize,
            0};

        ContainerHolder containerHolder = {
            AddressOf(_dataAllocPair.GetSecond()),
            newThisData};

        Detail::TidyGuard<ContainerHolder*> guard = {AddressOf(containerHolder)};

        // Constructs all the elements
        for (SizeType i = SizeType(0); i < otherData.InitializedSize; ++i)
        {
            AllocTraits::Construct(_dataAllocPair.GetSecond(), containerHolder.Data.Begin + i, *(otherData.Begin + i));
            ++containerHolder.Data.InitializedSize;
        }

        guard.Target = nullptr;

        if constexpr (!AllocTraits::PropagateOnContainerCopyAssignment)
            Tidy();

        thisData = containerHolder.Data;
    }

    return *this;
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>& List<T, Alloc, IteratorDebugging>::operator=(List<T, Alloc, IteratorDebugging>&& other) noexcept(MoveAssignmentNoexcept)
{
    if (this == AddressOf(other))
        return *this;

    auto& thisData  = _dataAllocPair.GetFirst();
    auto& otherData = other._dataAllocPair.GetFirst();

    const bool equal = AllocTraits::IsAlwaysEqual || (_dataAllocPair.GetSecond() == other._dataAllocPair.GetSecond());

    if (AllocTraits::PropagateOnContainerMoveAssignment || equal)
    {
        Tidy();

        if constexpr (AllocTraits::PropagateOnContainerMoveAssignment)
            _dataAllocPair.GetSecond() = Axis::System::MoveAssignIfNoThrow(other._dataAllocPair.GetSecond());

        // Simply moves the pointer
        thisData.Begin           = otherData.Begin;
        thisData.AllocatedSize   = otherData.AllocatedSize;
        thisData.InitializedSize = otherData.InitializedSize;

        otherData.Begin           = PointerType(nullptr);
        otherData.AllocatedSize   = SizeType(0);
        otherData.InitializedSize = SizeType(0);
    }
    else
    {
        const bool useOldMemoryOnlyAssignment                      = thisData.InitializedSize >= otherData.InitializedSize && (IsNoThrowCopyAssignable<ValueType> || IsNoThrowMoveAssignable<ValueType>);
        const bool useOldMemoryWithAssignmentAndExtraConstructions = thisData.InitializedSize < otherData.InitializedSize && thisData.AllocatedSize >= otherData.InitializedSize && (IsNoThrowCopyAssignable<ValueType> || IsNoThrowMoveAssignable<ValueType>)&&(IsNoThrowCopyConstructible<T> || IsNoThrowMoveConstructible<T>);
        const bool spareMemory                                     = thisData.AllocatedSize >= otherData.InitializedSize && (IsNoThrowCopyConstructible<T> || IsNoThrowMoveConstructible<T>);

        if (useOldMemoryOnlyAssignment)
        {
            if constexpr (IsNoThrowCopyAssignable<ValueType> || IsNoThrowMoveAssignable<ValueType>)
            {
                // Assigns
                for (SizeType i = SizeType(0); i < otherData.InitializedSize; ++i)
                    *(thisData.Begin + i) = MoveAssignIfNoThrow(*(otherData.Begin + i));

                // Destructs the rest
                for (SizeType i = otherData.InitializedSize; i < thisData.InitializedSize; ++i)
                    AllocTraits::Destruct(_dataAllocPair.GetSecond(), thisData.Begin + i);

                thisData.InitializedSize = otherData.InitializedSize;
            }
        }
        else if (useOldMemoryWithAssignmentAndExtraConstructions)
        {
            if constexpr ((IsNoThrowCopyAssignable<ValueType> || IsNoThrowMoveAssignable<ValueType>)&&(IsNoThrowCopyConstructible<T> || IsNoThrowMoveConstructible<T>))
            {
                auto assignRange = thisData.InitializedSize < otherData.InitializedSize ? thisData.InitializedSize : otherData.InitializedSize;

                // Assigns
                for (SizeType i = SizeType(0); i < assignRange; ++i)
                    *(thisData.Begin + i) = MoveAssignIfNoThrow(*(otherData.Begin + i));

                // Constructs
                for (SizeType i = thisData.InitializedSize; i < otherData.InitializedSize; ++i)
                    AllocTraits::Construct(_dataAllocPair.GetSecond(), thisData.Begin + i, MoveConstructIfNoThrow(*(otherData.Begin + i)));

                thisData.InitializedSize = otherData.InitializedSize;
            }
        }
        else if (spareMemory)
        {
            if constexpr (IsNoThrowCopyConstructible<T> || IsNoThrowMoveConstructible<T>)
            {
                // Destructs all the elements
                for (SizeType i = SizeType(0); i < thisData.InitializedSize; ++i)
                    AllocTraits::Destruct(_dataAllocPair.GetSecond(), thisData.Begin + i);

                // Constructs all the elements
                for (SizeType i = SizeType(0); i < otherData.InitializedSize; ++i)
                    AllocTraits::Construct(_dataAllocPair.GetSecond(), thisData.Begin + i, MoveConstructIfNoThrow(*(otherData.Begin + i)));

                thisData.InitializedSize = otherData.InitializedSize;
            }
        }
        else
        {
            Data newThisData = {
                AllocTraits::Allocate(_dataAllocPair.GetSecond(), otherData.InitializedSize),
                otherData.InitializedSize,
                0};

            ContainerHolder containerHolder = {
                AddressOf(_dataAllocPair.GetSecond()),
                newThisData};


            Detail::TidyGuard<ContainerHolder*> guard = {AddressOf(containerHolder)};

            // Constructs all the elements
            for (SizeType i = SizeType(0); i < otherData.InitializedSize; ++i)
            {
                AllocTraits::Construct(_dataAllocPair.GetSecond(), containerHolder.Data.Begin + i, MoveConstructIfNoThrow(*(otherData.Begin + i)));
                ++containerHolder.Data.InitializedSize;
            }

            guard.Target = nullptr;

            Tidy();

            thisData = containerHolder.Data;
        }
    }

    return *this;
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::SizeType List<T, Alloc, IteratorDebugging>::GetSize() const noexcept
{
    return _dataAllocPair.GetFirst().InitializedSize;
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline T& List<T, Alloc, IteratorDebugging>::operator[](SizeType index)
{
    if (index >= _dataAllocPair.GetFirst().InitializedSize)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return _dataAllocPair.GetFirst().Begin[index];
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline const T& List<T, Alloc, IteratorDebugging>::operator[](SizeType index) const
{
    if (index >= _dataAllocPair.GetFirst().InitializedSize)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return _dataAllocPair.GetFirst().Begin[index];
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::begin()
{
    if constexpr (IteratorDebugging)
    {
        auto it = Iterator(_dataAllocPair.GetFirst().Begin);

        BaseType::AssignIterator(it);

        return it;
    }
    else
        return Iterator(_dataAllocPair.GetFirst().Begin);
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::ConstIterator List<T, Alloc, IteratorDebugging>::begin() const
{
    if constexpr (IteratorDebugging)
    {
        auto it = ConstIterator(_dataAllocPair.GetFirst().Begin);

        BaseType::AssignIterator(it);

        return it;
    }
    else
        return ConstIterator(_dataAllocPair.GetFirst().Begin);
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::end()
{
    if constexpr (IteratorDebugging)
    {
        auto it = Iterator(_dataAllocPair.GetFirst().Begin + _dataAllocPair.GetFirst().InitializedSize);

        BaseType::AssignIterator(it);

        return it;
    }
    else
        return Iterator(_dataAllocPair.GetFirst().Begin + _dataAllocPair.GetFirst().InitializedSize);
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::ConstIterator List<T, Alloc, IteratorDebugging>::end() const
{
    if constexpr (IteratorDebugging)
    {
        auto it = ConstIterator(_dataAllocPair.GetFirst().Begin + _dataAllocPair.GetFirst().InitializedSize);

        BaseType::AssignIterator(it);

        return it;
    }
    else
        return ConstIterator(_dataAllocPair.GetFirst().Begin + _dataAllocPair.GetFirst().InitializedSize);
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::ConstIterator List<T, Alloc, IteratorDebugging>::cbegin() const
{
    if constexpr (IteratorDebugging)
    {
        auto it = ConstIterator(_dataAllocPair.GetFirst().Begin);

        BaseType::AssignIterator(it);

        return it;
    }
    else
        return ConstIterator(_dataAllocPair.GetFirst().Begin);
}

template <RawType T, AllocatorType Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::ConstIterator List<T, Alloc, IteratorDebugging>::cend() const
{
    if constexpr (IteratorDebugging)
    {
        auto it = ConstIterator(_dataAllocPair.GetFirst().Begin + _dataAllocPair.GetFirst().InitializedSize);

        BaseType::AssignIterator(it);

        return it;
    }
    else
        return ConstIterator(_dataAllocPair.GetFirst().Begin + _dataAllocPair.GetFirst().InitializedSize);
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_LISTIMPL_INL
