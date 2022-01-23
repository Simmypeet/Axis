/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_RANDOMIMPL_INL
#define AXIS_SYSTEM_RANDOMIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/Math.hpp"
#include "../../Include/Axis/Random.hpp"

namespace Axis
{

inline constexpr Random::Random(Int32 seed) noexcept :
    inext(0),
    inextp(21)
{
    Int32 ii;
    Int32 mj, mk;

    //Initialize our Seed array.
    //This algorithm comes from Numerical Recipes in C (2nd Ed.)
    Int32 subtraction = (seed == INT32_MIN) ? INT32_MAX : Math::Abs(seed);
    mj                = MSEED - subtraction;
    SeedArray[55]     = mj;
    mk                = 1;
    for (int i = 1; i < 55; i++)
    { //Apparently the range [1..55] is special (Knuth) and so we're wasting the 0'th position.
        ii            = (21 * i) % 55;
        SeedArray[ii] = mk;
        mk            = mj - mk;
        if (mk < 0) mk += MBIG;
        mj = SeedArray[ii];
    }
    for (int k = 1; k < 5; k++)
    {
        for (int i = 1; i < 56; i++)
        {
            SeedArray[i] -= SeedArray[1 + (i + 30) % 55];
            if (SeedArray[i] < 0) SeedArray[i] += MBIG;
        }
    }
}

inline constexpr Int32 Random::Next() noexcept
{
    Int32 retVal;
    Int32 locINext  = inext;
    Int32 locINextp = inextp;

    if (++locINext >= 56) locINext = 1;
    if (++locINextp >= 56) locINextp = 1;

    retVal = SeedArray[locINext] - SeedArray[locINextp];

    if (retVal == MBIG) retVal--;
    if (retVal < 0) retVal += MBIG;

    SeedArray[locINext] = retVal;

    inext  = locINext;
    inextp = locINextp;

    return retVal;
}


inline constexpr Int32 Random::Next(Int32 minValue, Int32 maxValue)
{
    if (minValue > maxValue)
        throw InvalidArgumentException("`minValue` was greater than `maxValue`!");

    Int64 range = (Int64)maxValue - minValue;
    if (range <= (Int64)(2147483647))
        return ((Int32)(Sample() * range) + minValue);
    else
        return (Int32)((Int64)(GetSampleForLargeRange() * range) + minValue);
}

inline constexpr Int32 Random::Next(Int32 maxValue)
{
    if (maxValue < 0)
        throw InvalidArgumentException("`maxValue` was less than zero!");

    return (Int32)(Sample() * maxValue);
}

inline constexpr Float64 Random::NextDouble() noexcept
{
    return Sample();
}

inline constexpr Uint8 Random::NextByte() noexcept
{
    return (Uint8)Next() % (std::numeric_limits<Uint8>::max() + 1);
}

inline constexpr Size Random::NextSize() noexcept
{
    // Calls NextByte() and fills the Size's bytes with random data.

    constexpr auto ByteSize = sizeof(Size);

    Size result = 0;

    for (Size i = 0; i < ByteSize; i++)
        result |= (Size)NextByte() << (i * 8);

    return result;
}

inline constexpr Float64 Random::Sample() noexcept
{
    //Including this division at the end gives us significantly improved
    //random number distribution.
    return (Next() * (1.0 / MBIG));
}

inline constexpr Float64 Random::GetSampleForLargeRange() noexcept
{
    // The distribution of Float64 value returned by Sample
    // is not distributed well enough for a large range.
    // If we use Sample for a range [Int32.MinValue..Int32.MaxValue)
    // We will end up getting even numbers only.

    Int32 result = Next();
    // Note we can't use addition here. The distribution will be bad if we do that.
    const Bool negative = (Next() % 2 == 0) ? true : false; // decide the sign based on second sample
    if (negative)
    {
        result = -result;
    }
    Float64 d = result;
    d += (2147483647 - 1); // gets a number in range [0 .. 2 * Int32MaxValue - 1)
    d /= 2 * (Uint32)2147483647 - 1;
    return d;
}

} // namespace Axis

#endif // AXIS_SYSTEM_RANDOMIMPL_INL