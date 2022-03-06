/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_LIST_HPP
#define AXIS_SYSTEM_LIST_HPP
#pragma once

#include "../../Private/Axis/CoreContainer.inl"
#include "Allocator.hpp"
#include "Iterator.hpp"

namespace Axis
{

namespace System
{

/// \brief The container which stores the elements contiguously in the memory. It can be resized and
///        manipulated.
///
/// The list is dynamic which, can be resized to any specified size.
///
/// All the functions in this class are categorized as `strong exception guarantee`. Which means that
/// if an exception is thrown, the state of the object is rolled back to the state before the function.
template <RawType T, AllocatorType Alloc = Allocator<T, DefaultMemoryResource>, Bool IteratorDebugging = Detail::CoreContainer::DefaultIteratorDebug>
class List final : private ConditionalType<IteratorDebugging, Detail::CoreContainer::DebugIteratorContainer, Detail::CoreContainer::Empty>
{
public:
    using ThisType             = List<T, Alloc, IteratorDebugging>;          // Type of this class
    using AllocType            = Alloc;                                      // Type of the allocator base class
    using AllocTraits          = AllocatorTraits<AllocType>;                 // Allocator traits
    using ValueType            = typename AllocTraits::ValueType;            // Value type
    using SizeType             = typename AllocTraits::SizeType;             // Size type
    using DifferenceType       = typename AllocTraits::DifferenceType;       // Difference type
    using PointerType          = typename AllocTraits::PointerType;          // Pointer type
    using ConstPointerType     = typename AllocTraits::ConstPointerType;     // Const pointer type
    using VoidPointerType      = typename AllocTraits::VoidPointerType;      // Void pointer type
    using ConstVoidPointerType = typename AllocTraits::ConstVoidPointerType; // Const void pointer type

    // AllocatorTraits' value type must be the same as `T`
    static_assert(IsSame<ValueType, T>, "Allocator's value type mismatch");

private:
    using BaseType = ConditionalType<IteratorDebugging, Detail::CoreContainer::DebugIteratorContainer, Detail::CoreContainer::Empty>;

    struct Data; // Data structure for the container

    // Checks if default constructor is nooexcept
    static constexpr Bool DefaultConstructorNoexcept = IsNoThrowDefaultConstructible<AllocType>;

    // Checks if allocator copy constructor is noexcept
    static constexpr Bool AllocatorCopyConstructorNoexcept = IsNoThrowCopyConstructible<AllocType>;

    // Checks if move constructor is noexcept
    static constexpr Bool MoveConstructorNoexcept = IsNoThrowMoveConstructible<AllocType>;

    // Checks if move assignment is noexcept
    static constexpr Bool MoveAssignmentNoexcept = AllocTraits::IsAlwaysEqual || AllocTraits::PropagateOnContainerMoveAssignment;

    // Contains both the data and the allocator
    CompressedPair<Data, AllocType> _dataAllocPair;

    class BaseIterator; // Base iterator class

    template <Bool IsConst>
    class IteratorTemplate; // Iterator template class

public:
    /// \brief List's iterator class.
    using Iterator = IteratorTemplate<false>;

    /// \brief List's const iterator class.
    using ConstIterator = IteratorTemplate<true>;

    /// \brief Default constructor
    List() noexcept(DefaultConstructorNoexcept);

    /// \brief Destructor
    ~List() noexcept;

    /// \brief Constructs the container with custom allocator.
    explicit List(const AllocType& allocator) noexcept(AllocatorCopyConstructorNoexcept);

    /// \brief Constructs the container with defined size and custom allocator.
    ///
    /// \param[in] elementCount Number of elements to allocate memory for.
    /// \param[in] allocator Custom allocator to copy into the container.
    explicit List(SizeType         elementCount,
                  const AllocType& allocator = AllocType());

    /// \brief Constructs the container with defined size, custom allocator and initial value.
    ///
    /// \param[in] elementCount Number of elements to allocate memory for.
    /// \param[in] value Initial value for all the elements.
    /// \param[in] allocator Custom allocator to copy into the container.
    explicit List(SizeType         elementCount,
                  const T&         value,
                  const AllocType& allocator = AllocType());

    /// \brief Constructs the container with initializer list and custom allocator.
    ///
    /// \param[in] list Initializer list to initialize the container.
    /// \param[in] allocator Custom allocator to copy into the container.
    List(std::initializer_list<T> list,
         const AllocType&         allocator = AllocType());

    /// \brief Copy constructor
    ///
    /// \param[in] other Other container to copy from.
    List(const List& other);

    /// \brief Move constructor
    ///
    /// \param[in] other Other container to move from.
    List(List&& other) noexcept(MoveConstructorNoexcept);

    /// \brief Copy assignment operator
    ///
    /// \param[in] other Other container to copy from.
    List& operator=(const List& other);

    /// \brief Move assignment operator
    ///
    /// \param[in] other Other container to move from.
    List& operator=(List&& other) noexcept(MoveAssignmentNoexcept);

    /// \brief Gets the number of elements in the list.
    ///
    ///  \return Number of elements in the list.
    AXIS_NODISCARD SizeType GetSize() const noexcept;

    /// \brief Gets the reference to the element at the specified index.
    ///
    /// \param[in] index Index of the element.
    ///
    /// \return Reference to the element at the specified index.
    AXIS_NODISCARD T& operator[](SizeType index);

    /// \brief Gets the const reference to the element at the specified index.
    ///
    /// \param[in] index Index of the element.
    ///
    /// \return Const reference to the element at the specified index.
    AXIS_NODISCARD const T& operator[](SizeType index) const;

    /// \brief Gets the iterator to the first element in the list.
    ///
    /// \return Iterator to the first element in the list.
    AXIS_NODISCARD Iterator begin();

    /// \brief Gets the const iterator to the first element in the list.
    ///
    /// \return Const iterator to the first element in the list.
    AXIS_NODISCARD ConstIterator begin() const;

    /// \brief Gets the iterator to the element after the last element in the list.
    ///
    /// \return Iterator to the element after the last element in the list.
    AXIS_NODISCARD Iterator end();

    /// \brief Gets the const iterator to the element after the last element in the list.
    ///
    /// \return Const iterator to the element after the last element in the list.
    AXIS_NODISCARD ConstIterator end() const;

    /// \brief Gets the const iterator to the first element in the list.
    ///
    /// \return Const iterator to the first element in the list.
    AXIS_NODISCARD ConstIterator cbegin() const;

    /// \brief Gets the const iterator to the element after the last element in the list.
    ///
    /// \return Const iterator to the element after the last element in the list.
    AXIS_NODISCARD ConstIterator cend() const;

    /// \brief Reserves the memory space for further insertions.
    ///
    /// \param[in] elementCount Numbers of element to reserve the memory for.
    void Reserve(SizeType elementCount);

    /// \brief Emplaces the element at the end of the list.
    ///
    /// \param[in] args Arguments to pass to the constructor.
    ///
    /// \return The iterator to the newly appended element.
    template <class... Args>
    Iterator EmplaceBack(Args&&... args);

    /// \brief Appends the element at the end of the list.
    ///
    /// \param[in] value Value to append.
    ///
    /// \return The iterator to the newly appended element.
    Iterator Append(const T& value);

    /// \brief Appends the element at the end of the list.
    ///
    /// \param[in] value Value to append.
    ///
    /// \return The iterator to the newly appended element.
    Iterator Append(T&& value);

    /// \brief Appends the range of elements at the end of the list.
    ///
    /// \param[in] begin Random access iterator to the first element of the range.
    /// \param[in] end Random access iterator to the element after the last element of the range.
    ///
    /// \return The iterator to the first appended element range.
    template <RandomAccessReadIterator<T> IteratorType>
    Iterator AppendRange(const IteratorType& begin,
                         const IteratorType& end);

    /// \brief Removes the element at the end of the list.
    void PopBack() noexcept;

    /// \brief Removes the elements at the specified index.
    ///
    /// \param[in] index Index of the element to remove.
    /// \param[in] count The number of elements to remove.
    ///
    /// \return The iterator to the element after the last removed element.
    Iterator RemoveAt(SizeType index,
                      SizeType count = 1);

    /// \brief Gets the capacity of the list (number of elements that have been allocated for).
    ///
    /// \return Capacity of the list.
    SizeType GetCapacity() const noexcept;

    /// \brief Gets the maximum number of elements that can be stored in the list.
    ///
    /// \return Maximum number of elements that can be stored in the list.
    SizeType GetMaxSize() const noexcept;

    /// \brief Emplace the element at the specified index.
    ///
    /// \param[in] index Index of the element to emplace.
    /// \param[in] args  Arguments to pass to the constructor.
    ///
    /// \return The iterator to the newly emplaced element.
    template <class... Args>
    Iterator Emplace(SizeType index, Args&&... args);

    /// \brief Inserts the element at the specified index.
    ///
    /// \param[in] index Index of the element to insert.
    /// \param[in] value Value to insert.
    ///
    /// \return The iterator to the newly inserted element.
    Iterator Insert(SizeType index, const T& value);

    /// \brief Inserts the element at the specified index.
    ///
    /// \param[in] index Index of the element to insert.
    /// \param[in] value Value to insert.
    ///
    /// \return The iterator to the newly inserted element.
    Iterator Insert(SizeType index, T&& value);

    /// \brief Inserts the range of elements at the specified index.
    ///
    /// \param[in] index Index of the element to insert.
    /// \param[in] begin Random access iterator to the first element of the range.
    /// \param[in] end Random access iterator to the element after the last element of the range.
    ///
    /// \return The iterator to the first inserted element range.
    template <RandomAccessReadIterator<T> IteratorType>
    Iterator InsertRange(SizeType            index,
                         const IteratorType& begin,
                         const IteratorType& end);

    /// \brief Destroys all of the elements in the list.
    ///
    /// \tparam DeallocateMemory specifies whether to deallocate the
    ///                          memory also.
    template <Bool DeallocateMemory = false>
    void Clear() noexcept;

    /// \brief Resizes the container to the specified element count.
    ///
    /// If the new size is smaller than the current size, the elements at the
    /// end of the container are removed. If the new size is larger than the
    /// current size, the container is extended by constructing (default constructor) new elements
    /// at the end.
    ///
    /// \param[in] newSize The new size of the container.
    void Resize(SizeType newSize);

    /// \brief Resizes the container to the specified element count.
    ///
    /// If the new size is smaller than the current size, the elements at the
    /// end of the container are removed. If the new size is larger than the
    /// current size, the container is extended by copying the given value into the new elements.
    ///
    /// \param[in] newSize The new size of the container.
    /// \param[in] value Value to copy into the new elements.
    void Resize(SizeType newSize,
                const T& value);

    /// \brief Implicit conversion to Boolean, returns true if the list is not empty.
    ///
    /// \return True if the list is not empty.
    operator Bool() const noexcept;

    /// \brief Resets the elements in the list (destructs and default constructs them).
    void Reset() noexcept(IsNoThrowDefaultConstructible<T>);

    /// \brief Resets the elements in the list (destructs and copies construct them or copies assign them).
    ///
    /// \param[in] value The value used to reset the element in the list.
    void Reset(const T& value) noexcept(IsNoThrowCopyConstructible<T>);

    /// \brief Gets the underlying pointer to the list's buffer.
    ///
    /// \return The underlying pointer to the list's buffer.
    PointerType GetData() noexcept;

    /// \brief Gets the underlying pointer to the list's buffer.
    ///
    /// \return The underlying pointer to the list's buffer.
    ConstPointerType GetData() const noexcept;


private:
    struct ContainerHolder;
    struct SpacedContainerHolder;

    void Tidy() noexcept;
    void TidyData(const Data& data) noexcept;

    template <class Lambda>
    void ConstructContinuousContainer(SizeType      elementCount,
                                      const Lambda& construct);

    template <Bool ForceNewAllocation>
    Pair<Data, Bool> CreateCopy(SizeType elementCount);

    template <Bool ForceNewAllocation>
    Pair<SpacedContainerHolder, Bool> CreateSpace(SizeType index,
                                                  SizeType elementCount);

    /// Checks for the maximum number of elements that can be stored in the list.
    /// and returns the new number of elements to pre-allocate for.
    SizeType CheckNewElement(SizeType newElementCount);

    /// Gets the iterator at the specified index
    template <Bool IsConst>
    IteratorTemplate<IsConst> GetIterator(SizeType index) const noexcept;

    /// Resizes the container to the specified element count.
    template <class Lambda>
    void ResizeInternal(SizeType newSize, const Lambda& construct);

    template <Bool ForceNewAllocation, class Lambda>
    void ResetInternal(const Lambda& construct);

    template <class>
    friend struct Detail::CoreContainer::TidyGuard;
};

} // namespace System

} // namespace Axis

#include "../../Private/Axis/ListImpl.inl"

#endif // AXIS_SYSTEM_LIST_HPP
