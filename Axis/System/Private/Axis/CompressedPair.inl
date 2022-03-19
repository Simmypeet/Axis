/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_COMPRESSEDPAIR_HPP
#define AXIS_SYSTEM_COMPRESSEDPAIR_HPP
#pragma once

#include "../../Include/Axis/Trait.hpp"
#include "CallTraits.inl"

namespace Axis::System
{

/// \brief Constructs a compressed pair elements via perfect forwarding.
struct CompressedPairPerfectForwardTagType
{};

/// \brief Constructs a compressed pair elements via perfect forwarding.
inline constexpr CompressedPairPerfectForwardTagType PerfectForwardTag = {};

namespace Detail::CompressedPair
{

/// \brief The class uses empty base optimization to reduce the size of
///        the object.
template <class First, class Second, Size Case>
class CompressedPairImpl;

/// \brief The class uses empty base optimization to reduce the size of
///        the object.
template <class First, class Second>
class CompressedPairImpl<First, Second, 0>
{
public:
    using FirstType                = First;                                                       // The first type of the pair.
    using SecondType               = Second;                                                      // The second type of the pair.
    using FirstParamType           = typename Detail::CallTraits<FirstType>::ParamType;           // The parameter type of the first type of the pair.
    using SecondParamType          = typename Detail::CallTraits<SecondType>::ParamType;          // The parameter type of the second type of the pair.
    using FirstReferenceType       = typename Detail::CallTraits<FirstType>::ReferenceType;       // The reference type of the first type of the pair.
    using SecondReferenceType      = typename Detail::CallTraits<SecondType>::ReferenceType;      // The reference type of the second type of the pair.
    using FirstConstReferenceType  = typename Detail::CallTraits<FirstType>::ConstReferenceType;  // The const reference type of the first type of the pair.
    using SecondConstReferenceType = typename Detail::CallTraits<SecondType>::ConstReferenceType; // The const reference type of the second type of the pair.

    /// \brief The constructor.
    CompressedPairImpl() noexcept(IsNothrowDefaultConstructible<FirstType>&& IsNothrowDefaultConstructible<SecondType>) = default;

    /// \brief The constructor.
    ///
    /// \param[in] first The first value of the pair.
    /// \param[in] second The second value of the pair.
    CompressedPairImpl(FirstParamType first, SecondParamType second) noexcept(IsNothrowConstructible<FirstType, FirstParamType>&& IsNothrowConstructible<SecondType, SecondParamType>) :
        _first(first), _second(second) {}

    /// \brief Constructs a pair for only the first type.
    ///
    /// \param[in] first The first value of the pair.
    explicit CompressedPairImpl(FirstParamType first) noexcept(IsNothrowConstructible<FirstType, FirstParamType>&& IsNothrowDefaultConstructible<SecondType>) :
        _first(first), _second() {}

    /// \brief Constructs a pair for only the second type.
    ///
    /// \param[in] second The second value of the pair.
    explicit CompressedPairImpl(SecondParamType second) noexcept(IsNothrowDefaultConstructible<FirstType>&& IsNothrowConstructible<SecondType, SecondParamType>) :
        _first(), _second(second) {}

    /// \brief The constructor, using perfect forwarding.
    ///
    /// \param[in] perfectForwardTag The tag to use perfect forwarding.
    /// \param[in] first The first value of the pair.
    /// \param[in] second The second value of the pair.
    template <class FirstOther, class SecondOther>
    CompressedPairImpl(CompressedPairPerfectForwardTagType perfectForwardTag,
                       FirstOther&&                        first,
                       SecondOther&&                       second) noexcept(IsNothrowConstructible<FirstType, FirstOther>&& IsNothrowConstructible<SecondType, SecondOther>) :
        _first(Forward<FirstOther>(first)), _second(Forward<SecondOther>(second))
    {}

    /// \brief Retrieves the first reference from the pair.
    FirstReferenceType GetFirst() noexcept { return _first; }

    /// \brief Retrieves the first const reference from the pair.
    FirstConstReferenceType GetFirst() const noexcept { return _first; }

    /// \brief Retrieves the second reference from the pair.
    SecondReferenceType GetSecond() noexcept { return _second; }

    /// \brief Retrieves the second const reference from the pair.
    SecondConstReferenceType GetSecond() const noexcept { return _second; }

private:
    FirstType  _first  = {};
    SecondType _second = {};
};

/// \brief The class uses empty base optimization to reduce the size of
///        the object.
template <class First, class Second>
class CompressedPairImpl<First, Second, 1> : private First
{
public:
    using FirstType                = First;                                                       // The first type of the pair.
    using SecondType               = Second;                                                      // The second type of the pair.
    using FirstParamType           = typename Detail::CallTraits<FirstType>::ParamType;           // The parameter type of the first type of the pair.
    using SecondParamType          = typename Detail::CallTraits<SecondType>::ParamType;          // The parameter type of the second type of the pair.
    using FirstReferenceType       = typename Detail::CallTraits<FirstType>::ReferenceType;       // The reference type of the first type of the pair.
    using SecondReferenceType      = typename Detail::CallTraits<SecondType>::ReferenceType;      // The reference type of the second type of the pair.
    using FirstConstReferenceType  = typename Detail::CallTraits<FirstType>::ConstReferenceType;  // The const reference type of the first type of the pair.
    using SecondConstReferenceType = typename Detail::CallTraits<SecondType>::ConstReferenceType; // The const reference type of the second type of the pair.

    /// \brief The constructor.
    CompressedPairImpl() noexcept(IsNothrowDefaultConstructible<FirstType>&& IsNothrowDefaultConstructible<SecondType>) = default;

    /// \brief The constructor.
    ///
    /// \param[in] first The first value of the pair.
    /// \param[in] second The second value of the pair.
    CompressedPairImpl(FirstParamType first, SecondParamType second) noexcept(IsNothrowConstructible<FirstType, FirstParamType>&& IsNothrowConstructible<SecondType, SecondParamType>) :
        FirstType(first), _second(second) {}

    /// \brief Constructs a pair for only the first type.
    ///
    /// \param[in] first The first value of the pair.
    explicit CompressedPairImpl(FirstParamType first) noexcept(IsNothrowConstructible<FirstType, FirstParamType>&& IsNothrowDefaultConstructible<SecondType>) :
        FirstType(first), _second() {}

    /// \brief Constructs a pair for only the second type.
    ///
    /// \param[in] second The second value of the pair.
    explicit CompressedPairImpl(SecondParamType second) noexcept(IsNothrowDefaultConstructible<FirstType>&& IsNothrowConstructible<SecondType, SecondParamType>) :
        FirstType(), _second(second) {}

    /// \brief The constructor, using perfect forwarding.
    ///
    /// \param[in] perfectForwardTag The tag to use perfect forwarding.
    /// \param[in] first The first value of the pair.
    /// \param[in] second The second value of the pair.
    template <class FirstOther, class SecondOther>
    CompressedPairImpl(CompressedPairPerfectForwardTagType perfectForwardTag,
                       FirstOther&&                        first,
                       SecondOther&&                       second) noexcept(IsNothrowConstructible<FirstType, FirstOther>&& IsNothrowConstructible<SecondType, SecondOther>) :
        FirstType(Forward<FirstOther>(first)), _second(Forward<SecondOther>(second))
    {}

    /// \brief Retrieves the first reference from the pair.
    FirstReferenceType GetFirst() noexcept { return *this; }

    /// \brief Retrieves the first const reference from the pair.
    FirstConstReferenceType GetFirst() const noexcept { return *this; }

    /// \brief Retrieves the second reference from the pair.
    SecondReferenceType GetSecond() noexcept { return _second; }

    /// \brief Retrieves the second const reference from the pair.
    SecondConstReferenceType GetSecond() const noexcept { return _second; }

private:
    SecondType _second = {};
};

/// \brief The class uses empty base optimization to reduce the size of
///        the object.
template <class First, class Second>
class CompressedPairImpl<First, Second, 2> : private Second
{
public:
    using FirstType                = First;                                                       // The first type of the pair.
    using SecondType               = Second;                                                      // The second type of the pair.
    using FirstParamType           = typename Detail::CallTraits<FirstType>::ParamType;           // The parameter type of the first type of the pair.
    using SecondParamType          = typename Detail::CallTraits<SecondType>::ParamType;          // The parameter type of the second type of the pair.
    using FirstReferenceType       = typename Detail::CallTraits<FirstType>::ReferenceType;       // The reference type of the first type of the pair.
    using SecondReferenceType      = typename Detail::CallTraits<SecondType>::ReferenceType;      // The reference type of the second type of the pair.
    using FirstConstReferenceType  = typename Detail::CallTraits<FirstType>::ConstReferenceType;  // The const reference type of the first type of the pair.
    using SecondConstReferenceType = typename Detail::CallTraits<SecondType>::ConstReferenceType; // The const reference type of the second type of the pair.

    /// \brief The constructor.
    CompressedPairImpl() noexcept(IsNothrowDefaultConstructible<FirstType>&& IsNothrowDefaultConstructible<SecondType>) = default;

    /// \brief The constructor.
    ///
    /// \param[in] first The first value of the pair.
    /// \param[in] second The second value of the pair.
    CompressedPairImpl(FirstParamType first, SecondParamType second) noexcept(IsNothrowConstructible<FirstType, FirstParamType>&& IsNothrowConstructible<SecondType, SecondParamType>) :
        SecondType(second), _first(first) {}

    /// \brief Constructs a pair for only the first type.
    ///
    /// \param[in] first The first value of the pair.
    explicit CompressedPairImpl(FirstParamType first) noexcept(IsNothrowConstructible<FirstType, FirstParamType>&& IsNothrowDefaultConstructible<SecondType>) :
        SecondType(), _first(first) {}

    /// \brief Constructs a pair for only the second type.
    ///
    /// \param[in] second The second value of the pair.
    explicit CompressedPairImpl(SecondParamType second) noexcept(IsNothrowDefaultConstructible<FirstType>&& IsNothrowConstructible<SecondType, SecondParamType>) :
        SecondType(second), _first() {}

    /// \brief The constructor, using perfect forwarding.
    ///
    /// \param[in] perfectForwardTag The tag to use perfect forwarding.
    /// \param[in] first The first value of the pair.
    /// \param[in] second The second value of the pair.
    template <class FirstOther, class SecondOther>
    CompressedPairImpl(CompressedPairPerfectForwardTagType perfectForwardTag,
                       FirstOther&&                        first,
                       SecondOther&&                       second) noexcept(IsNothrowConstructible<FirstType, FirstOther>&& IsNothrowConstructible<SecondType, SecondOther>) :
        SecondType(Forward<SecondOther>(second)), _first(Forward<FirstOther>(first))
    {}

    /// \brief Retrieves the first reference from the pair.
    FirstReferenceType GetFirst() noexcept { return _first; }

    /// \brief Retrieves the first const reference from the pair.
    FirstConstReferenceType GetFirst() const noexcept { return _first; }

    /// \brief Retrieves the second reference from the pair.
    SecondReferenceType GetSecond() noexcept { return *this; }

    /// \brief Retrieves the second const reference from the pair.
    SecondConstReferenceType GetSecond() const noexcept { return *this; }

private:
    FirstType _first = {};
};

/// \brief The class uses empty base optimization to reduce the size of
///        the object.
template <class First, class Second>
class CompressedPairImpl<First, Second, 3> : private First, private Second
{
public:
    using FirstType                = First;                                                       // The first type of the pair.
    using SecondType               = Second;                                                      // The second type of the pair.
    using FirstParamType           = typename Detail::CallTraits<FirstType>::ParamType;           // The parameter type of the first type of the pair.
    using SecondParamType          = typename Detail::CallTraits<SecondType>::ParamType;          // The parameter type of the second type of the pair.
    using FirstReferenceType       = typename Detail::CallTraits<FirstType>::ReferenceType;       // The reference type of the first type of the pair.
    using SecondReferenceType      = typename Detail::CallTraits<SecondType>::ReferenceType;      // The reference type of the second type of the pair.
    using FirstConstReferenceType  = typename Detail::CallTraits<FirstType>::ConstReferenceType;  // The const reference type of the first type of the pair.
    using SecondConstReferenceType = typename Detail::CallTraits<SecondType>::ConstReferenceType; // The const reference type of the second type of the pair.

    /// \brief The constructor.
    CompressedPairImpl() noexcept(IsNothrowDefaultConstructible<FirstType>&& IsNothrowDefaultConstructible<SecondType>) = default;

    /// \brief The constructor.
    ///
    /// \param[in] first The first value of the pair.
    /// \param[in] second The second value of the pair.
    CompressedPairImpl(FirstParamType first, SecondParamType second) noexcept(IsNothrowConstructible<FirstType, FirstParamType>&& IsNothrowConstructible<SecondType, SecondParamType>) :
        FirstType(first), SecondType(second) {}

    /// \brief Constructs a pair for only the first type.
    ///
    /// \param[in] first The first value of the pair.
    explicit CompressedPairImpl(FirstParamType first) noexcept(IsNothrowConstructible<FirstType, FirstParamType>&& IsNothrowDefaultConstructible<SecondType>) :
        FirstType(first), SecondType() {}

    /// \brief Constructs a pair for only the second type.
    ///
    /// \param[in] second The second value of the pair.
    explicit CompressedPairImpl(SecondParamType second) noexcept(IsNothrowDefaultConstructible<FirstType>&& IsNothrowConstructible<SecondType, SecondParamType>) :
        FirstType(), SecondType(second) {}

    /// \brief The constructor, using perfect forwarding.
    ///
    /// \param[in] perfectForwardTag The tag to use perfect forwarding.
    /// \param[in] first The first value of the pair.
    /// \param[in] second The second value of the pair.
    template <class FirstOther, class SecondOther>
    CompressedPairImpl(CompressedPairPerfectForwardTagType perfectForwardTag,
                       FirstOther&&                        first,
                       SecondOther&&                       second) noexcept(IsNothrowConstructible<FirstType, FirstOther>&& IsNothrowConstructible<SecondType, SecondOther>) :
        FirstType(Forward<FirstOther>(first)), SecondType(Forward<SecondOther>(second))
    {}

    /// \brief Retrieves the first reference from the pair.
    FirstReferenceType GetFirst() noexcept { return *this; }

    /// \brief Retrieves the first const reference from the pair.
    FirstConstReferenceType GetFirst() const noexcept { return *this; }

    /// \brief Retrieves the second reference from the pair.
    SecondReferenceType GetSecond() noexcept { return *this; }

    /// \brief Retrieves the second const reference from the pair.
    SecondConstReferenceType GetSecond() const noexcept { return *this; }
};

/// \brief The class uses empty base optimization to reduce the size of
///        the object.
template <class First, class Second>
class CompressedPairImpl<First, Second, 4> : private First
{
public:
    using FirstType                = First;                                                       // The first type of the pair.
    using SecondType               = Second;                                                      // The second type of the pair.
    using FirstParamType           = typename Detail::CallTraits<FirstType>::ParamType;           // The parameter type of the first type of the pair.
    using SecondParamType          = typename Detail::CallTraits<SecondType>::ParamType;          // The parameter type of the second type of the pair.
    using FirstReferenceType       = typename Detail::CallTraits<FirstType>::ReferenceType;       // The reference type of the first type of the pair.
    using SecondReferenceType      = typename Detail::CallTraits<SecondType>::ReferenceType;      // The reference type of the second type of the pair.
    using FirstConstReferenceType  = typename Detail::CallTraits<FirstType>::ConstReferenceType;  // The const reference type of the first type of the pair.
    using SecondConstReferenceType = typename Detail::CallTraits<SecondType>::ConstReferenceType; // The const reference type of the second type of the pair.

    /// \brief The constructor.
    CompressedPairImpl() noexcept(IsNothrowDefaultConstructible<FirstType>&& IsNothrowDefaultConstructible<SecondType>) = default;

    /// \brief Constructs a pair for only the first type.
    ///
    /// \param[in] first The first value of the pair.
    explicit CompressedPairImpl(FirstParamType first) noexcept(IsNothrowConstructible<FirstType, FirstParamType>&& IsNothrowDefaultConstructible<SecondType>) :
        FirstType(first) {}

    /// \brief The constructor, using perfect forwarding.
    ///
    /// \param[in] perfectForwardTag The tag to use perfect forwarding.
    /// \param[in] first The first value of the pair.
    template <class FirstOther, class SecondOther>
    CompressedPairImpl(CompressedPairPerfectForwardTagType perfectForwardTag,
                       FirstOther&&                        first) noexcept(IsNothrowConstructible<FirstType, FirstOther>) :
        FirstType(Forward<FirstOther>(first))
    {}

    /// \brief Retrieves the first reference from the pair.
    FirstReferenceType GetFirst() noexcept { return *this; }

    /// \brief Retrieves the first const reference from the pair.
    FirstConstReferenceType GetFirst() const noexcept { return *this; }

    /// \brief Retrieves the second reference from the pair.
    SecondReferenceType GetSecond() noexcept { return *this; }

    /// \brief Retrieves the second const reference from the pair.
    SecondConstReferenceType GetSecond() const noexcept { return *this; }
};

template <bool isSame, bool firstEmpty, bool secondEmpty>
struct CompressedPairSwitch;

template <>
struct CompressedPairSwitch<false, false, false>
{
    static constexpr Size Value = 0;
};

template <>
struct CompressedPairSwitch<false, true, false>
{
    static constexpr Size Value = 1;
};

template <>
struct CompressedPairSwitch<false, false, true>
{
    static constexpr Size Value = 2;
};

template <>
struct CompressedPairSwitch<false, true, true>
{
    static constexpr Size Value = 3;
};

template <>
struct CompressedPairSwitch<true, true, true>
{
    static constexpr Size Value = 4;
};

template <>
struct CompressedPairSwitch<true, false, false>
{
    static constexpr Size Value = 0;
};

template <class First, class Second>
inline constexpr Size GetSwitch = CompressedPairSwitch<Concept::IsSame<First, Second>, IsEmpty<First>, IsEmpty<Second>>::Value;

} // namespace Detail::CompressedPair


/// \brief The class uses empty base optimization to reduce the size of
///        the object.
template <class First, class Second>
using CompressedPair = Detail::CompressedPair::CompressedPairImpl<First, Second, Detail::CompressedPair::GetSwitch<First, Second>>;

} // namespace Axis::System

#endif // AXIS_SYSTEM_COMPRESSEDPAIR_HPP
