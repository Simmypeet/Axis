/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_VARIANT_HPP
#define AXIS_SYSTEM_VARIANT_HPP
#pragma once

#include "Utility.hpp"
#include <limits>

namespace Axis::System
{

namespace Detail::Variant
{

template <Size T, Size... Args>
struct Max; // Gets the maximum value of a set of sizes.

template <Uint8 Index, class... Args>
struct GetTypeAtIndex; // Gets the type at a given index.

template <class... Args>
struct TypeChecker; // Checks if all types are valid.

template <class... Args>
struct VariantIsNothrowCopyConstructible; // True if all types are nothrow copy constructible.

template <class... Args>
struct VariantIsNothrowMoveConstructible; // True if all types are nothrow copy constructible.

} // namespace Detail::Variant

/// \brief The class which capable of storing various types of data in a single storage.
///
/// The class can represent any type of data, but it is limited to only one type at a time.
template <class... Types>
class Variant
{
public:
    // Types check
    static_assert(Detail::Variant::TypeChecker<Types...>::Value, "The variant contained invalid types.");

    // Types check
    static_assert(sizeof...(Types) > 0, "The variant had no types.");

    // Types check
    static_assert(sizeof...(Types) < std::numeric_limits<Uint8>::max(), "The variant contained more than 255 types.");

    // The index of the valueless variant.
    static constexpr Uint8 ValuelessIndex = std::numeric_limits<Uint8>::max();

    /// \brief Gets the type of the variant at the specified index
    template <Uint8 Index>
    using TypeAt = typename Detail::Variant::GetTypeAtIndex<Index, Types...>::Type;

    /// \brief Default constructor, the variant is valueless.
    Variant() noexcept;

    /// \brief Copy constructor
    ///
    /// \param[in] other The variant to copy from.
    ///
    /// The copy constructor is nothrow if all types are nothrow copy constructible.
    Variant(const Variant& other) noexcept(Detail::Variant::VariantIsNothrowCopyConstructible<Types...>::Value);

    /// \brief Constructs the variant from the specified type index.
    ///
    /// The variant destructs the current stored type and constructs a new one.
    ///
    /// \tparam Index The index of the type to construct.
    /// \tparam Args The arguments to pass to the constructor of the type.
    ///
    /// \param[in] args The arguments to pass to the constructor of the type.
    template <Uint8 Index, class... Args>
    void Construct(Args&&... args) noexcept(IsNothrowConstructible<TypeAt<Index>, Args...>);

    /// \brief Destructor; the current type index is destructed.
    ~Variant() noexcept;

    /// \brief Gets the current index of the type that is currently stored.
    ///
    /// \return Returns the current index of the type that is currently stored if the variant is not valueless, else
    ///         returns 255.
    AXIS_NODISCARD Uint8 GetTypeIndex() const noexcept;

    /// \brief Checks if the variant is currently storing a type.
    AXIS_NODISCARD Bool IsValueless() const noexcept;

private:
    static constexpr Size MaxSize  = Detail::Variant::Max<sizeof(Types)...>::Value;  // Max size of the variant
    static constexpr Size MaxAlign = Detail::Variant::Max<alignof(Types)...>::Value; // Max alignment of the variant

    StaticStorage<MaxSize, MaxAlign> _storage   = {}; // Storage for the variant
    Uint8                            _typeIndex = 0;  // Type of the variant
};

} // namespace Axis::System

#include "../../Private/Axis/VariantImpl.inl"

#endif // AXIS_SYSTEM_VARIANT_HPP
