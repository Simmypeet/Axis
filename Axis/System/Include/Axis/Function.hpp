/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Function.hpp
///
/// \brief Contains `Axis::Function` template class.

#ifndef AXIS_SYSTEM_FUNCTION_HPP
#define AXIS_SYSTEM_FUNCTION_HPP
#pragma once

#include "Memory.hpp"
#include "Trait.hpp"
#include "Utility.hpp"

namespace Axis
{

/// \brief Template type erasure container class for containing all callable objects.
template <class T, AllocatorType Allocator = DefaultAllocator>
struct Function;

/// \brief Template type erasure container class for containing all callable objects.
///
/// To use this class, the functor object that you want to store must have the following conditions:
/// - Must have noexcept copy and move constructors.
/// - The functor object must have a function call operator.
template <AllocatorType Allocator, class ReturnType, class... Args>
struct Function<ReturnType(Args...), Allocator>
{
public:
    /// \brief Default constructor
    ///
    /// The object can't be called if it was constructed with this constructor
    Function() noexcept;

    /// \brief Constructs object in null state.
    ///
    /// The object can't be called if it was constructed with this constructor
    Function(NullptrType) noexcept;

    /// \brief Constructs and copies the functor object into this `Axis::Function` object.
    ///
    /// \param[in] f Callable object to copy.
    template <Callable<ReturnType, Args...> Functor, typename = std::enable_if_t<!std::is_same_v<Functor, Function>>>
    Function(const Functor& f);

    /// \brief Constructs and moves the functor object into this `Axis::Function` object.
    ///
    /// \param[in] f Callable object to move.
    template <Callable<ReturnType, Args...> Functor, typename = std::enable_if_t<!std::is_same_v<Functor, Function>>>
    Function(Functor&& f);

    /// \brief Copy constructor
    ///
    /// \param[in] other Instance to copy
    Function(const Function& other);

    /// \brief Move constructor
    ///
    /// \param[in] other Instance to move
    Function(Function&& other) noexcept;

    /// \brief Destructor
    ~Function() noexcept;

    /// \brief Invokes the underlying function object with specified arguments.
    ///
    /// \param[in] args Variadic template arguments to invoke the underlying callable object.
    ReturnType operator()(Args&&... args);

    /// \brief Copy assignemt operator
    ///
    /// \param[in] other Instance to copy
    Function& operator=(const Function& other);

    /// \brief Move assignment operator
    ///
    /// \param[in] other Instance to move
    Function& operator=(Function&& other) noexcept;

    /// \brief Checks whether the object is in null state or not.
    Bool operator==(NullptrType) const noexcept;

    /// \brief Checks whether the object is in null state or not.
    Bool operator!=(NullptrType) const noexcept;

    /// \brief Turns this object into null state.
    Function& operator=(NullptrType) noexcept;

    /// \brief Specifies whether this `Axis::Function` object is callable.
    operator Bool() const noexcept;


private:
    /// Size of small buffer contained in the object.
    static constexpr Size SmallBufferOptimizationSize = Axis::PointerSize;

    /// Function pointer signature using
    using InvokePtr        = ReturnType (*)(void*, Args&&...);
    using DestructorPtr    = void (*)(void*);
    using CopyConstructPtr = void (*)(void*, const void*);
    using MoveConstructPtr = void (*)(void*, void*);

    /// Emulates the vtable that stores different kinds of functions.
    struct VTable final
    {
        InvokePtr        Invoke        = nullptr; // Invokes the functor's operator()
        DestructorPtr    Destruct      = nullptr; // Invokes the functor's destructor
        CopyConstructPtr CopyConstruct = nullptr; // Invokes copy constructor
        MoveConstructPtr MoveConstruct = nullptr; // Invokes move constructor
    };

    void Destroy() noexcept;

    const VTable* _pVtable           = nullptr; // Pointer to the vtable object
    Size          _dynamicBufferSize = 0;       // Size of dynamic buffer
    union
    {
        PVoid                                      DynamicBuffer;
        StaticStorage<SmallBufferOptimizationSize> Storage;
    };
};

} // namespace Axis

#include "../../Private/Axis/FunctionImpl.inl"

#endif // AXIS_SYSTEM_FUNCTION_HPP