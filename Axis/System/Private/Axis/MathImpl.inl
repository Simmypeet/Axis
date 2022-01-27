/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_MATHIMPL_INL
#define AXIS_SYSTEM_MATHIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/Math.hpp"
#include <cmath>


namespace Axis
{

namespace Math
{

template <FloatingPointType T, FloatingPointType U>
inline constexpr Bool IsFloatEqual(T a, U b) noexcept
{
    return std::abs(a - b) < std::numeric_limits<T>::epsilon();
}

template <IntegralType T>
inline constexpr T AssignBitToPosition(T bitStorage, Uint8 position, Bool value) noexcept
{
#pragma warning(push)
#pragma warning(disable : 4804)
    T copy = bitStorage;
    copy ^= (-value ^ copy) & (T{1} << position);
#pragma warning(pop)
    return copy;
}

template <IntegralType T>
inline constexpr Bool ReadBitPosition(T bitStorage, Uint8 position) noexcept
{
    return (bitStorage >> position) & T{1};
}

template <IntegralType T>
inline constexpr T GetLeastSignificantBit(T value) noexcept
{
    if (value == T{0})
        return 0;

    auto val = value & ~(value - T{1});
    return val;
}

template <ArithmeticType T, ArithmeticType U, ArithmeticType V>
inline constexpr T Clamp(T value,
                         U min,
                         V max)
{
    if (min > max)
        throw InvalidArgumentException("`min` value was greater than `max` value.");

    return value < min ?
                      min :
        value > max ? max :
                      value;
}

template <ArithmeticType T, ArithmeticType U, ArithmeticType V>
inline constexpr Bool IsInRange(T value,
                                U min,
                                V max)
{
    if (min > max)
        throw InvalidArgumentException("`min` value was greater than `max` value.");

    return value >= min &&
        value <= max;
}

template <ArithmeticType T>
inline constexpr T Max(T first, T second) noexcept
{
    return first > second ? first : second;
}

template <ArithmeticType T>
inline constexpr T Min(T first, T second) noexcept
{
    return first < second ? first : second;
}

inline constexpr Size HashCombine(Size hash,
                                  Size anotherHash) noexcept
{
    Size combinedHash = hash ^ anotherHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);

    return combinedHash;
}

template <ArithmeticType T>
inline constexpr T Abs(T value) noexcept
{
    return value < 0 ? -value : value;
}

template <IntegralType T>
inline constexpr T RoundUp(T numToRound, T multipleOf) noexcept
{
    if constexpr (std::is_signed_v<T>)
    {
        if (multipleOf == 0)
            return numToRound;

        T remainder = Abs(numToRound) % multipleOf;
        if (remainder == 0)
            return numToRound;

        if (numToRound < 0)
            return -(Abs(numToRound) - remainder);
        else
            return numToRound + multipleOf - remainder;
    }
    else
    {
        if (multipleOf == 0)
            return numToRound;

        T remainder = numToRound % multipleOf;

        return numToRound + multipleOf - remainder;
    }
}

template <ArithmeticType T>
constexpr auto Modulo(T x, T y) noexcept -> decltype(auto)
{
    return (std::is_floating_point<T>::value) ? (x < T() ? T(-1) : T(1)) * ((x < T() ? -x : x) - static_cast<long long int>((x / y < T() ? -x / y : x / y)) * (y < T() ? -y : y)) : (static_cast<typename std::conditional<std::is_floating_point<T>::value, int, T>::type>(x) % static_cast<typename std::conditional<std::is_floating_point<T>::value, int, T>::type>(y));
}

template <ArithmeticType T, FloatingPointType FloatingPointPrecision>
inline constexpr FloatingPointPrecision ToRadians(T degree) noexcept
{
    constexpr FloatingPointPrecision piValue = FloatingPointPrecision(3.1415926535897932384626433832795028841971);
    return FloatingPointPrecision(degree) * (piValue / FloatingPointPrecision(180));
}

template <UnsignedIntegralType T>
inline constexpr T RoundToNextPowerOfTwo(T num) noexcept
{
    if (num == 0)
        return 1;

    num--;

    for (Size i = 1; i < sizeof(Size) * CHAR_BIT; i <<= 1)
        num |= (num >> i);

    return num + 1;
}

template <UnsignedIntegralType T>
inline constexpr T IsPrime(T num) noexcept
{
    // Corner cases
    if (num <= 1) return false;
    if (num <= 3) return true;

    // This is checked so that we can skip
    // middle five numbers in below loop
    if (num % 2 == 0 || num % 3 == 0) return false;

    for (Size i = 5; i * i <= num; i = i + 6)
        if (num % i == 0 || num % (i + 2) == 0)
            return false;

    return true;
}

template <UnsignedIntegralType T>
inline constexpr T NextPrime(T num) noexcept
{
    // Base case
    if (num <= 1)
        return 2;

    Size prime = num;

    // Loop continuously until isPrime returns
    // true for a number greater than n
    while (true)
    {
        prime++;

        if (IsPrime(prime))
            break;
    }

    return prime;
}

} // namespace Math

} // namespace Axis

#endif // AXIS_SYSTEM_MATHIMPL_IN