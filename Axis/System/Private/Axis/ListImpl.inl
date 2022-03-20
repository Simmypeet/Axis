/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_LISTIMPL_INL
#define AXIS_SYSTEM_LISTIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/List.hpp"
#include "../../Include/Axis/Utility.hpp"

namespace Axis::System
{

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
struct List<T, Alloc, IteratorDebugging>::Data
{
    PointerType Begin           = PointerType(nullptr); // Pointer to the first element
    SizeType    AllocatedSize   = SizeType(0);          // Number of elements allocated
    SizeType    InitializedSize = SizeType(0);          // Number of elements initialized
};

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
struct List<T, Alloc, IteratorDebugging>::SpacedContainerHolder
{
    AllocType* AllocatorPointer = nullptr;
    Data       Data             = {};
    SizeType   SpaceIndex       = {};
    SizeType   SpaceSize        = {};
    SizeType   SpaceConstructed = {};

    inline void Tidy() noexcept
    {
        for (SizeType i = 0; i < Data.InitializedSize; ++i)
        {
            const SizeType extraIndex = i < SpaceIndex ? 0 : SpaceSize;

            AllocTraits::Destruct(*AllocatorPointer, Data.Begin + i + extraIndex);
        }

        for (SizeType i = 0; i < SpaceConstructed; ++i)
            AllocTraits::Destruct(*AllocatorPointer, Data.Begin + i + SpaceIndex);

        AllocTraits::Deallocate(*AllocatorPointer, Data.Begin, Data.AllocatedSize);
    }
};

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
class List<T, Alloc, IteratorDebugging>::BaseIterator : protected ConditionalType<IteratorDebugging, Detail::CoreContainer::BaseDebugIterator, Detail::CoreContainer::Empty>
{
protected:
    List<T, Alloc, IteratorDebugging>::PointerType _currentPointer = {}; // Pointer to the current element.
};

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
template <bool IsConst>
class List<T, Alloc, IteratorDebugging>::IteratorTemplate final : private List<T, Alloc, IteratorDebugging>::BaseIterator
{
private:
    using ListType = List<T, Alloc, IteratorDebugging>;

public:
    using DifferenceType = typename ListType::DifferenceType;
    using SizeType       = typename ListType::SizeType;
    using ReferenceType  = ConditionalType<IsConst, const T&, T&>;
    using ElementType    = ConditionalType<IsConst, const T, T>;

    IteratorTemplate() noexcept                              = default;
    IteratorTemplate(const IteratorTemplate& other) noexcept = default;
    IteratorTemplate(IteratorTemplate&& other) noexcept      = default;
    IteratorTemplate& operator=(const IteratorTemplate& other) noexcept = default;
    IteratorTemplate& operator=(IteratorTemplate&& other) noexcept = default;

    IteratorTemplate(NullptrType) noexcept {}

    template <bool OtherIsConst>
    inline IteratorTemplate(const IteratorTemplate<OtherIsConst>& other) noexcept requires(IsConst) :
        BaseIterator((const BaseIterator&)other) {}

    template <bool OtherIsConst>
    inline IteratorTemplate(IteratorTemplate<OtherIsConst>&& other) noexcept requires(IsConst) :
        BaseIterator((BaseIterator &&) other) {}

    template <bool OtherIsConst>
    inline IteratorTemplate& operator=(const IteratorTemplate<OtherIsConst>& other) noexcept requires(IsConst)
    {
        BaseIterator::operator=((const BaseIterator&)other);
        return *this;
    }

    template <bool OtherIsConst>
    inline IteratorTemplate& operator=(IteratorTemplate<OtherIsConst>&& other) noexcept requires(IsConst)
    {
        BaseIterator::operator=((BaseIterator &&) other);
        return *this;
    }

private :
    IteratorTemplate(typename List<T, Alloc, IteratorDebugging>::PointerType pointer) noexcept
    {
        BaseIterator::_currentPointer = pointer;
    }

    inline void ValidateRange(DifferenceType index = 0) const noexcept requires(IteratorDebugging)
    {
        if constexpr (IteratorDebugging)
        {
            if (!Detail::CoreContainer::BaseDebugIterator::_skipValidation)
            {
                BaseIterator::BasicValidate();

                auto& listData = ((ListType*)Detail::CoreContainer::BaseDebugIterator::_debuggingTracker->DebugContainer)->_dataAllocPair.GetFirst();

                // Checks if _currentPointer is within the bounds of the list
                AXIS_VALIDATE((BaseIterator::_currentPointer + index) >= listData.Begin && (BaseIterator::_currentPointer + index) < listData.Begin + listData.InitializedSize, "Iterator was out of bounds!");
            }
        }
    }

    template <Bool OtherIsConst>
    inline void ValidateContainer(const IteratorTemplate<OtherIsConst>& other) const noexcept requires(IteratorDebugging)
    {
        // Checks if the other iterator is from the same container
        if constexpr (IteratorDebugging)
        {
            if (!Detail::CoreContainer::BaseDebugIterator::_skipValidation)
            {
                BaseIterator::BasicValidate();

                auto thisContaienr  = Detail::CoreContainer::BaseDebugIterator::_debuggingTracker->DebugContainer;
                auto otherContainer = other._debuggingTracker->DebugContainer;

                // Checks whether if they are the from the same container
                AXIS_VALIDATE(thisContaienr == otherContainer, "Attempted to do comparisons between the iterators there weren't from the same container!");
            }
        }
    }

public :
    inline ReferenceType
    operator*() const noexcept
    {
        if constexpr (IteratorDebugging)
            ValidateRange();

        return *BaseIterator::_currentPointer;
    }

    inline ConditionalType<IsConst, typename List<T, Alloc, IteratorDebugging>::ConstPointerType, typename List<T, Alloc, IteratorDebugging>::PointerType> operator->() const noexcept
    {
        if constexpr (IteratorDebugging)
            ValidateRange();

        return BaseIterator::_currentPointer;
    }

    // Comparison operators
    template <bool OtherIsConst>
    inline bool operator==(const IteratorTemplate<OtherIsConst>& other) const noexcept
    {
        if constexpr (IteratorDebugging)
            ValidateContainer(other);

        return BaseIterator::_currentPointer == other._currentPointer;
    }

    template <bool OtherIsConst>
    inline bool operator!=(const IteratorTemplate<OtherIsConst>& other) const noexcept
    {
        if constexpr (IteratorDebugging)
            ValidateContainer(other);

        return BaseIterator::_currentPointer != other._currentPointer;
    }

    template <bool OtherIsConst>
    inline bool operator<(const IteratorTemplate<OtherIsConst>& other) const noexcept
    {
        if constexpr (IteratorDebugging)
            ValidateContainer(other);

        return BaseIterator::_currentPointer < other._currentPointer;
    }

    template <bool OtherIsConst>
    inline bool operator>(const IteratorTemplate<OtherIsConst>& other) const noexcept
    {
        if constexpr (IteratorDebugging)
            ValidateContainer(other);

        return BaseIterator::_currentPointer > other._currentPointer;
    }

    template <bool OtherIsConst>
    inline bool operator<=(const IteratorTemplate<OtherIsConst>& other) const noexcept
    {
        if constexpr (IteratorDebugging)
            ValidateContainer(other);

        return BaseIterator::_currentPointer <= other._currentPointer;
    }

    template <bool OtherIsConst>
    inline bool operator>=(const IteratorTemplate<OtherIsConst>& other) const noexcept
    {
        if constexpr (IteratorDebugging)
            ValidateContainer(other);

        return BaseIterator::_currentPointer >= other._currentPointer;
    }

    // Arithmetic operators
    inline IteratorTemplate operator+(const DifferenceType difference) const noexcept // Addition
    {
        IteratorTemplate copy = *this;
        copy._currentPointer += difference;
        return copy;
    }

    inline IteratorTemplate operator-(const DifferenceType difference) const noexcept // Subtraction
    {
        IteratorTemplate copy = *this;
        copy._currentPointer -= difference;
        return copy;
    }

    inline IteratorTemplate& operator+=(const DifferenceType difference) noexcept // Addition assignment
    {
        BaseIterator::_currentPointer += difference;
        return *this;
    }

    inline IteratorTemplate& operator-=(const DifferenceType difference) noexcept // Subtraction assignment
    {
        BaseIterator::_currentPointer -= difference;
        return *this;
    }

    template <bool OtherIsConst>
    inline DifferenceType operator-(const IteratorTemplate<OtherIsConst>& other) const noexcept // Difference
    {
        if constexpr (IteratorDebugging)
            ValidateContainer(other);

        return BaseIterator::_currentPointer - other._currentPointer;
    }

    // Increment and decrement operators
    inline IteratorTemplate& operator++() noexcept // Pre-increment
    {
        ++BaseIterator::_currentPointer;
        return *this;
    }

    inline IteratorTemplate operator++(int) noexcept // Post-increment
    {
        IteratorTemplate copy = *this;
        ++BaseIterator::_currentPointer;
        return copy;
    }

    inline IteratorTemplate& operator--() noexcept // Pre-decrement
    {
        --BaseIterator::_currentPointer;
        return *this;
    }

    inline IteratorTemplate operator--(int) noexcept // Post-decrement
    {
        IteratorTemplate copy = *this;
        --BaseIterator::_currentPointer;
        return copy;
    }

    inline ReferenceType operator[](DifferenceType index) const noexcept
    {
        if constexpr (IteratorDebugging)
            ValidateRange(index);

        return BaseIterator::_currentPointer[index];
    }

    inline IteratorTemplate& operator=(NullptrType) noexcept
    {
        BaseIterator::_currentPointer = nullptr;
        return *this;
    }

    friend class List<T, Alloc, IteratorDebugging>;

    template <bool>
    friend class IteratorTemplate;
};

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::Reserve(SizeType elementCount)
{
    if (elementCount <= _dataAllocPair.GetFirst().AllocatedSize)
        return;

    if (elementCount > GetMaxSize())
        throw ContainerExceededMaxSizeException();

    auto newDataCopy = CreateCopy<false>(elementCount);

    if (newDataCopy.Second)
        Tidy();

    _dataAllocPair.GetFirst() = newDataCopy.First;
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::Append(const T& value)
{
    return EmplaceBack<const T&>(value);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::Append(T&& value)
{
    return EmplaceBack<T&&>(Axis::System::Move(value));
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
template <Concept::PointerToValue<const T> IteratorType>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::AppendRange(const IteratorType& begin,
                                                                                                           const IteratorType& end)
{
    if (begin == end)
        throw InvalidArgumentException("The range is empty!");

    if (begin > end)
        throw InvalidArgumentException("`begin` was greater than `end`!");

    DifferenceType count = end - begin;

    if (count < 0)
        throw InvalidArgumentException("Iterators `begin` or `end` were invalid!");

    Reserve(CheckNewElement(count));

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

    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    AppendGuard appendGuard = {
        AddressOf(data),
        AddressOf(alloc),
        data.InitializedSize};

    Detail::CoreContainer::TidyGuard<AppendGuard> guard(AddressOf(appendGuard));

    for (auto it = begin; it != end; ++it)
    {
        const auto& value = *it;
        AllocTraits::Construct(alloc, data.Begin + data.InitializedSize, value);
        ++data.InitializedSize;
    }

    guard.Target = nullptr;

    return GetIterator<false>(data.InitializedSize - count);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::PopBack() noexcept
{
    if (_dataAllocPair.GetFirst().InitializedSize == 0)
        return;

    AllocTraits::Destruct(_dataAllocPair.GetSecond(), _dataAllocPair.GetFirst().Begin + (_dataAllocPair.GetFirst().InitializedSize - 1));
    --_dataAllocPair.GetFirst().InitializedSize;
}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::RemoveAt(SizeType index,
                                                                                                        SizeType count)
{
    if (count == 0)
        throw InvalidArgumentException("`count` was zero!");

    if (index >= _dataAllocPair.GetFirst().InitializedSize)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    if (index + count > _dataAllocPair.GetFirst().InitializedSize)
        throw ArgumentOutOfRangeException("`count` was out of range!");

    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    // Uses move / copy assign to move the elements (if nothrow)
    constexpr bool Assign = IsNothrowMoveAssignable<T> || IsNothrowCopyAssignable<T>;

    // Uses move / copy construct to move the elements (if nothrow)
    constexpr bool Construct = IsNothrowMoveConstructible<T> || IsNothrowCopyConstructible<T>;

    if constexpr (Assign || Construct)
    {
        auto moveCount = data.InitializedSize - (index + count);

        if constexpr (Construct && !Assign)
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

        Detail::CoreContainer::TidyGuard guard = {AddressOf(newDataHolder)};

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

    return GetIterator<false>(index);
}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::TidyData(const Data& data) noexcept
{
    Data&      dataInstance  = _dataAllocPair.GetFirst();
    AllocType& allocInstance = _dataAllocPair.GetSecond();

    for (SizeType i = 0; i < dataInstance.InitializedSize; ++i)
        AllocTraits::Destruct(allocInstance, dataInstance.Begin + i);

    if (dataInstance.Begin)
        AllocTraits::Deallocate(allocInstance, dataInstance.Begin, dataInstance.AllocatedSize);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
template <Bool ForceNewAllocation>
inline Pair<typename List<T, Alloc, IteratorDebugging>::Data, Bool> List<T, Alloc, IteratorDebugging>::CreateCopy(SizeType elementCount)
{
    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    if constexpr (!ForceNewAllocation)
    {
        if (elementCount <= data.AllocatedSize)
            return {data, false};
    }

    ContainerHolder containerHolder = {AddressOf(alloc),
                                       {AllocTraits::Allocate(alloc, elementCount), elementCount, 0}};

    Detail::CoreContainer::TidyGuard<ContainerHolder> guard(AddressOf(containerHolder));

    for (SizeType i = 0; i < data.InitializedSize; ++i)
    {
        AllocTraits::Construct(alloc, containerHolder.Data.Begin + i, ::Axis::System::MoveConstructIfNoThrow(data.Begin[i]));
        ++containerHolder.Data.InitializedSize;
    }

    guard.Target = nullptr;

    return {containerHolder.Data, true};
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::SizeType List<T, Alloc, IteratorDebugging>::GetCapacity() const noexcept
{
    return _dataAllocPair.GetFirst().AllocatedSize;
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::SizeType List<T, Alloc, IteratorDebugging>::GetMaxSize() const noexcept
{
    SizeType           allocMaxSize      = AllocTraits::MaxAllocationSize;
    constexpr SizeType differenceTypeMax = static_cast<SizeType>(std::numeric_limits<DifferenceType>::max());

    return Detail::CoreContainer::Min(allocMaxSize, differenceTypeMax);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
template <Bool ForceNewAllocation>
inline Pair<typename List<T, Alloc, IteratorDebugging>::SpacedContainerHolder, Bool> List<T, Alloc, IteratorDebugging>::CreateSpace(SizeType index,
                                                                                                                                    SizeType elementCount)
{
    constexpr bool ConstructNoExcept = IsNothrowCopyConstructible<T> || IsNothrowMoveConstructible<T>;
    constexpr bool AssignNoExcept    = IsNothrowCopyAssignable<T> || IsNothrowMoveAssignable<T>;

    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    const bool allocateNewMemory = ForceNewAllocation || (data.InitializedSize + elementCount > data.AllocatedSize) || !ConstructNoExcept;

    if (allocateNewMemory)
    {
        auto newElementCount = CheckNewElement(elementCount);

        SpacedContainerHolder spacedContainerHolder = {
            AddressOf(alloc),
            {AllocTraits::Allocate(alloc, Detail::CoreContainer::RoundToNextPowerOfTwo(data.InitializedSize + elementCount)),
             newElementCount,
             0},
            index,
            elementCount,
            0};

        Detail::CoreContainer::TidyGuard<SpacedContainerHolder> guard(AddressOf(spacedContainerHolder));

        // Moves / copies the old elements that are before the ones to insert
        for (SizeType i = 0; i < index; ++i)
        {
            AllocTraits::Construct(alloc, spacedContainerHolder.Data.Begin + i, ::Axis::System::MoveConstructIfNoThrow(data.Begin[i]));
            ++spacedContainerHolder.Data.InitializedSize;
        }

        // Moves / copies the old elements that are after the ones to insert
        for (SizeType i = index; i < data.InitializedSize; ++i)
        {
            AllocTraits::Construct(alloc, spacedContainerHolder.Data.Begin + i + elementCount,
                                   ::Axis::System::MoveConstructIfNoThrow(data.Begin[i]));
            ++spacedContainerHolder.Data.InitializedSize;
        }

        guard.Target = nullptr;

        return {spacedContainerHolder, true};
    }
    else
    {
        SpacedContainerHolder spacedContainerHolder = {
            AddressOf(alloc),
            data,
            index,
            elementCount,
            0};

        if constexpr (AssignNoExcept)
        {
            spacedContainerHolder.SpaceConstructed = elementCount;

            // Source index
            SizeType currentSouceIndex = spacedContainerHolder.Data.InitializedSize - 1;

            // Moves / copies elements to uninitialized memory
            for (SizeType i = 0; i < elementCount; i++)
            {
                AllocTraits::Construct(alloc,
                                       spacedContainerHolder.Data.Begin + currentSouceIndex + elementCount,
                                       ::Axis::System::MoveConstructIfNoThrow(spacedContainerHolder.Data.Begin[currentSouceIndex]));

                currentSouceIndex--;
            }

            // Moves / copies assign elements to initialized memory
            SizeType moveAssignCount = spacedContainerHolder.Data.InitializedSize - (index + elementCount);
            for (SizeType i = 0; i < moveAssignCount; i++)
            {
                spacedContainerHolder.Data.Begin[currentSouceIndex + elementCount] = ::Axis::System::MoveAssignIfNoThrow(spacedContainerHolder.Data.Begin[currentSouceIndex]);

                currentSouceIndex--;
            }

            return {spacedContainerHolder, false};
        }
        else
        {
            const SizeType moveConstructCount = spacedContainerHolder.Data.InitializedSize - index;

            for (SizeType i = 0; i < moveConstructCount; ++i)
            {
                const SizeType sourceIndex = data.InitializedSize - i - 1;
                const SizeType targetIndex = sourceIndex + elementCount;

                // Moves / copies the element from source to target
                AllocTraits::Construct(alloc,
                                       spacedContainerHolder.Data.Begin + targetIndex,
                                       ::Axis::System::MoveConstructIfNoThrow(spacedContainerHolder.Data.Begin[sourceIndex]));

                // Destructs the source
                AllocTraits::Destruct(alloc, spacedContainerHolder.Data.Begin + sourceIndex);
            }

            return {spacedContainerHolder, false};
        }
    }
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
template <class... Args>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::Emplace(SizeType index,
                                                                                                       Args&&... args)
{
    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    if (index == data.InitializedSize)
    {
        return EmplaceBack(Forward<Args>(args)...);
    }

    constexpr auto ConstructNoThrow = IsNothrowConstructible<T, Args...>;

    auto spacedDataPair = CreateSpace<!ConstructNoThrow>(index,
                                                         1);

    Detail::CoreContainer::TidyGuard<SpacedContainerHolder> tidyGuard;
    tidyGuard.Target = spacedDataPair.Second ? nullptr : AddressOf(spacedDataPair.First);

    if (spacedDataPair.First.SpaceConstructed)
    {
        AllocTraits::Destruct(alloc, spacedDataPair.First.Data.Begin + spacedDataPair.First.SpaceIndex);
        spacedDataPair.First.SpaceConstructed = 0;
    }

    AllocTraits::Construct(alloc, spacedDataPair.First.Data.Begin + index, ::Axis::System::Forward<Args>(args)...);
    ++spacedDataPair.First.Data.InitializedSize;

    tidyGuard.Target = nullptr;

    if (spacedDataPair.Second)
        Tidy();

    data = spacedDataPair.First.Data;

    return GetIterator<false>(index);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::Insert(typename List<T, Alloc, IteratorDebugging>::SizeType index,
                                                                                                      const T&                                             element)
{
    return Emplace<const T&>(index, element);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::Insert(typename List<T, Alloc, IteratorDebugging>::SizeType index,
                                                                                                      T&&                                                  element)
{
    return Emplace<T&&>(index, ::Axis::System::Move(element));
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
template <Concept::PointerToValue<const T> IteratorType>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::InsertRange(typename List<T, Alloc, IteratorDebugging>::SizeType index,
                                                                                                           const IteratorType&                                  begin,
                                                                                                           const IteratorType&                                  end)
{
    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    if (index == data.InitializedSize)
    {
        return AppendRange(begin,
                           end);
    }

    constexpr auto CopyConstructNoThrow = IsNothrowCopyConstructible<T>;
    constexpr auto CopyAssignNoThrow    = IsNothrowCopyAssignable<T>;

    DifferenceType elementCountSigned = end - begin;

    if (elementCountSigned <= 0)
        throw InvalidArgumentException("`begin` or `end` iterators were invalid!");

    SizeType elementCount = (SizeType)elementCountSigned;

    auto spacedDataPair = CreateSpace<!CopyConstructNoThrow>(index,
                                                             elementCount);

    Detail::CoreContainer::TidyGuard<SpacedContainerHolder> tidyGuard;
    tidyGuard.Target = spacedDataPair.Second ? nullptr : AddressOf(spacedDataPair.First);

    const auto useAssign = CopyAssignNoThrow && (Bool)(spacedDataPair.First.SpaceConstructed);

    if (useAssign)
    {
        if constexpr (CopyAssignNoThrow)
        {
            // Uses copy assign at the initialized elements.

            for (SizeType i = 0; i < elementCount; ++i)
                spacedDataPair.First.Data.Begin[index + i] = *(begin + i);
        }
    }
    else
    {
        if ((Bool)spacedDataPair.First.SpaceConstructed)
        {
            // If there are initialized elements there, destruct them
            for (SizeType i = 0; i < elementCount; ++i)
                AllocTraits::Destruct(alloc, spacedDataPair.First.Data.Begin + spacedDataPair.First.SpaceIndex + i);

            spacedDataPair.First.SpaceConstructed = 0;
        }

        auto currentIterator = begin;

        // Uses copy construct at the uninitialized elements.
        for (SizeType i = 0; i < elementCount; ++i)
        {
            AllocTraits::Construct(alloc, spacedDataPair.First.Data.Begin + index + i, *currentIterator);
            ++spacedDataPair.First.SpaceConstructed;

            ++currentIterator;
        }

        if ((Bool)spacedDataPair.First.SpaceConstructed)
            spacedDataPair.First.Data.InitializedSize += elementCount;
    }

    tidyGuard.Target = nullptr;

    if (spacedDataPair.Second)
        Tidy();

    data = spacedDataPair.First.Data;

    return GetIterator<false>(index);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
template <Bool DeallocateMemory>
inline void List<T, Alloc, IteratorDebugging>::Clear() noexcept
{
    if constexpr (DeallocateMemory)
    {
        Tidy();
    }
    else
    {
        auto& data  = _dataAllocPair.GetFirst();
        auto& alloc = _dataAllocPair.GetSecond();

        for (SizeType i = 0; i < data.InitializedSize; ++i)
            AllocTraits::Destruct(alloc, data.Begin + i);

        data.InitializedSize = 0;
    }
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::Resize(typename List<T, Alloc, IteratorDebugging>::SizeType newSize)
{
    const auto Construct = [this](typename List<T, Alloc, IteratorDebugging>::PointerType ptr) {
        AllocTraits::Construct(_dataAllocPair.GetSecond(), ptr);
    };

    ResizeInternal(newSize, Construct);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::Resize(typename List<T, Alloc, IteratorDebugging>::SizeType newSize,
                                                      const T&                                             value)
{
    const auto Construct = [&](typename List<T, Alloc, IteratorDebugging>::PointerType ptr) {
        AllocTraits::Construct(_dataAllocPair.GetSecond(), ptr, value);
    };

    ResizeInternal(newSize, Construct);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::SizeType List<T, Alloc, IteratorDebugging>::CheckNewElement(SizeType newElementCount)
{
    auto& data = _dataAllocPair.GetFirst();

    Detail::CoreContainer::ThrowIfOverflow<SizeType, InvalidOperationException>(newElementCount,
                                                                                data.InitializedSize,
                                                                                "Couldn't append more elements to the container!");

    return Detail::CoreContainer::Min(Detail::CoreContainer::RoundToNextPowerOfTwo(data.InitializedSize + newElementCount), GetMaxSize());
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
template <bool IsConst>
inline typename List<T, Alloc, IteratorDebugging>::template IteratorTemplate<IsConst> List<T, Alloc, IteratorDebugging>::GetIterator(typename List<T, Alloc, IteratorDebugging>::SizeType index) const noexcept
{
    if constexpr (IteratorDebugging)
    {
        auto it = IteratorTemplate<IsConst>(_dataAllocPair.GetFirst().Begin + index);

        BaseType::AssignIterator(it);

        return it;
    }
    else

        return IteratorTemplate<IsConst>(_dataAllocPair.GetFirst().Begin + index);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
template <class... Args>
inline typename List<T, Alloc, IteratorDebugging>::Iterator List<T, Alloc, IteratorDebugging>::EmplaceBack(Args&&... args)
{
    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    Reserve(Detail::CoreContainer::RoundToNextPowerOfTwo(data.InitializedSize + 1));

    AllocTraits::Construct(alloc, data.Begin + data.InitializedSize, Forward<Args>(args)...);

    ++data.InitializedSize;

    return GetIterator<false>(data.InitializedSize - 1);
}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
template <class Lambda>
inline void List<T, Alloc, IteratorDebugging>::ConstructContinuousContainer(SizeType elementCount, const Lambda& construct)
{
    if (elementCount == 0)
        return;

    Detail::CoreContainer::TidyGuard<List<T, Alloc, IteratorDebugging>> guard(this);

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
template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List() noexcept(DefaultConstructorNoexcept) :
    _dataAllocPair() {}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::~List() noexcept { Tidy(); }

// Constructor with allocator instance
template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List(const AllocType& allocator) noexcept(AllocatorCopyConstructorNoexcept) :
    _dataAllocPair(Data(), allocator) {}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List(const List& other) :
    _dataAllocPair(Data(), AllocTraits::SelectOnContainerCopyConstructor(other._dataAllocPair.GetSecond()))
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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::List(List&& other) noexcept(MoveConstructorNoexcept) :
    _dataAllocPair(PerfectForwardTag, Data(other._dataAllocPair.GetFirst()), Axis::System::Move(other._dataAllocPair.GetSecond()))
{
    auto& otherData           = other._dataAllocPair.GetFirst();
    otherData.Begin           = PointerType(nullptr);
    otherData.AllocatedSize   = SizeType(0);
    otherData.InitializedSize = SizeType(0);

    other.MoveTrackerTo(*this);
}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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
    const bool propagatedEqual = AllocTraits::PropagateOnContainerCopyAssignment ? AllocTraits::CompareEqual(thisAlloc, otherAlloc) : true;

    const bool useOldMemoryOnlyAssignment                      = propagatedEqual && thisData.InitializedSize >= otherData.InitializedSize && IsNothrowCopyAssignable<ValueType>;
    const bool useOldMemoryWithAssignmentAndExtraConstructions = propagatedEqual && thisData.InitializedSize < otherData.InitializedSize && thisData.AllocatedSize >= otherData.InitializedSize && IsNothrowCopyAssignable<ValueType> && IsNothrowCopyConstructible<T>;
    const bool spareMemory                                     = propagatedEqual && thisData.AllocatedSize >= otherData.InitializedSize && IsNothrowCopyConstructible<T>;

    if (useOldMemoryOnlyAssignment)
    {
        // Prevents ill-formedness
        if constexpr (IsNothrowCopyAssignable<ValueType>)
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
        if constexpr (IsNothrowCopyAssignable<ValueType> && IsNothrowCopyConstructible<T>)
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
        if constexpr (IsNothrowCopyConstructible<T>)
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

        Detail::CoreContainer::TidyGuard<ContainerHolder> guard(AddressOf(containerHolder));

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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
template <class Lambda>
inline void List<T, Alloc, IteratorDebugging>::ResizeInternal(SizeType      newSize,
                                                              const Lambda& construct)
{
    if (newSize > GetMaxSize())
        throw InvalidArgumentException("`newSize` exceeded max size!");

    if (newSize == GetSize())
        return;

    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    if (newSize > data.InitializedSize)
    {
        const bool createCopy = IsNothrowDefaultConstructible<T> || newSize > data.AllocatedSize;

        if (createCopy)
        {
            ContainerHolder containerHolder = {
                AddressOf(alloc),
                CreateCopy<true>(newSize).First};

            auto constructCount = newSize - data.InitializedSize;

            Detail::CoreContainer::TidyGuard<ContainerHolder> tidyGuard(AddressOf(containerHolder));

            for (SizeType i = 0; i < constructCount; ++i)
            {
                construct(containerHolder.Data.Begin + containerHolder.Data.InitializedSize);
                ++containerHolder.Data.InitializedSize;
            }

            tidyGuard.Target = nullptr;

            Tidy();

            data = containerHolder.Data;
        }
        else
        {
            auto constructCount = newSize - data.InitializedSize;

            for (SizeType i = 0; i < constructCount; ++i)
            {
                // Default constructor should always nothrow
                construct(data.Begin + data.InitializedSize);
                ++data.InitializedSize;
            }
        }
    }
    else
    {
        // If there are initialized elements there, destruct them
        for (SizeType i = newSize; i < data.InitializedSize; ++i)
            AllocTraits::Destruct(alloc, data.Begin + i);

        data.InitializedSize = newSize;
    }
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
template <Bool ForceNewAllocation, class Lambda>
inline void List<T, Alloc, IteratorDebugging>::ResetInternal(const Lambda& construct)
{
    if constexpr (!ForceNewAllocation)
    {
        for (SizeType i = 0; i < _dataAllocPair.GetFirst().InitializedSize; ++i)
        {
            AllocTraits::Destruct(_dataAllocPair.GetSecond(), _dataAllocPair.GetFirst().Begin + i);
            construct(_dataAllocPair.GetFirst().Begin + i);
        }
    }
    else
    {
        auto& data  = _dataAllocPair.GetFirst();
        auto& alloc = _dataAllocPair.GetSecond();

        ContainerHolder containerHolder = {AddressOf(alloc), {AllocTraits::Allocate(alloc, data.InitializedSize), data.InitializedSize, 0}};

        Detail::CoreContainer::TidyGuard<ContainerHolder> tidyGuard = {AddressOf(containerHolder)};

        for (SizeType i = 0; i < data.InitializedSize; ++i)
        {
            construct(containerHolder.Data.Begin + i);
            ++containerHolder.Data.InitializedSize;
        }
        tidyGuard.Target = nullptr;

        Tidy();

        data = containerHolder.Data;
    }
}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>& List<T, Alloc, IteratorDebugging>::operator=(List<T, Alloc, IteratorDebugging>&& other) noexcept(MoveAssignmentNoexcept)
{
    if (this == AddressOf(other))
        return *this;

    auto& thisData  = _dataAllocPair.GetFirst();
    auto& otherData = other._dataAllocPair.GetFirst();

    const bool equal = AllocTraits::CompareEqual(_dataAllocPair.GetSecond(), other._dataAllocPair.GetSecond());

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
        const bool useOldMemoryOnlyAssignment                      = thisData.InitializedSize >= otherData.InitializedSize && (IsNothrowCopyAssignable<ValueType> || IsNothrowMoveAssignable<ValueType>);
        const bool useOldMemoryWithAssignmentAndExtraConstructions = thisData.InitializedSize < otherData.InitializedSize && thisData.AllocatedSize >= otherData.InitializedSize && (IsNothrowCopyAssignable<ValueType> || IsNothrowMoveAssignable<ValueType>)&&(IsNothrowCopyConstructible<T> || IsNothrowMoveConstructible<T>);
        const bool spareMemory                                     = thisData.AllocatedSize >= otherData.InitializedSize && (IsNothrowCopyConstructible<T> || IsNothrowMoveConstructible<T>);

        if (useOldMemoryOnlyAssignment)
        {
            if constexpr (IsNothrowCopyAssignable<ValueType> || IsNothrowMoveAssignable<ValueType>)
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
            if constexpr ((IsNothrowCopyAssignable<ValueType> || IsNothrowMoveAssignable<ValueType>)&&(IsNothrowCopyConstructible<T> || IsNothrowMoveConstructible<T>))
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
            if constexpr (IsNothrowCopyConstructible<T> || IsNothrowMoveConstructible<T>)
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


            Detail::CoreContainer::TidyGuard<ContainerHolder> guard(AddressOf(containerHolder));

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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::SizeType List<T, Alloc, IteratorDebugging>::GetSize() const noexcept
{
    return _dataAllocPair.GetFirst().InitializedSize;
}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline T& List<T, Alloc, IteratorDebugging>::operator[](SizeType index)
{
    if (index >= _dataAllocPair.GetFirst().InitializedSize)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return _dataAllocPair.GetFirst().Begin[index];
}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline const T& List<T, Alloc, IteratorDebugging>::operator[](SizeType index) const
{
    if (index >= _dataAllocPair.GetFirst().InitializedSize)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return _dataAllocPair.GetFirst().Begin[index];
}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
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

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline List<T, Alloc, IteratorDebugging>::operator Bool() const noexcept
{
    return (Bool)_dataAllocPair.GetFirst().InitializedSize;
}

template <Concept::Pure T, template <typename> class Alloc, Bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::Reset() noexcept(IsNothrowDefaultConstructible<T>)
{
    const auto constructLambda = [this](PointerType ptr) {
        AllocTraits::Construct(_dataAllocPair.GetSecond(), ptr);
    };

    ResetInternal<!IsNothrowDefaultConstructible<T>, decltype(constructLambda)>(constructLambda);
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline void List<T, Alloc, IteratorDebugging>::Reset(const T& value) noexcept(IsNothrowCopyConstructible<T>)
{
    if constexpr (IsNothrowCopyAssignable<T>)
    {
        for (SizeType i = 0; i < _dataAllocPair.GetFirst().InitializedSize; ++i)
        {
            _dataAllocPair.GetFirst().Begin[i] = value;
        }
    }
    else
    {
        const auto constructLambda = [&](PointerType ptr) {
            AllocTraits::Construct(_dataAllocPair.GetSecond(), ptr, value);
        };

        ResetInternal<!IsNothrowCopyConstructible<T>, decltype(constructLambda)>(constructLambda);
    }
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::PointerType List<T, Alloc, IteratorDebugging>::GetData() noexcept
{
    return _dataAllocPair.GetFirst().Begin;
}

template <Concept::Pure T, template <typename> class Alloc, bool IteratorDebugging>
inline typename List<T, Alloc, IteratorDebugging>::ConstPointerType List<T, Alloc, IteratorDebugging>::GetData() const noexcept
{
    return _dataAllocPair.GetFirst().Begin;
}

} // namespace Axis::System

#endif // AXIS_SYSTEM_LISTIMPL_INL
