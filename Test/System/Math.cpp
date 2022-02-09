#include <Axis/System>
#include <doctest.h>

using namespace Axis;
using namespace Axis::System;

DOCTEST_TEST_CASE("Math functions : [Axis::System]")
{
    DOCTEST_SUBCASE("IsFloatEqual")
    {
        constexpr Float32 a = 1.0f / 3.0f;
        constexpr Float32 b = 10.0f / 30.0f;
        constexpr Float32 c = 0.333333333333333f;

        DOCTEST_CHECK(Math::IsFloatEqual(a, b));
        DOCTEST_CHECK(Math::IsFloatEqual(a, c));
        DOCTEST_CHECK(Math::IsFloatEqual(b, c));
    }

    DOCTEST_SUBCASE("AssignBitToPosition")
    {
        DOCTEST_CHECK(Math::AssignBitToPosition(0x0, 0, 1) == 0x1);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x0, 1, 1) == 0x2);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x0, 2, 1) == 0x4);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x0, 3, 1) == 0x8);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x0, 4, 1) == 0x10);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x0, 5, 1) == 0x20);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x0, 6, 1) == 0x40);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x0, 7, 1) == 0x80);

        DOCTEST_CHECK(Math::AssignBitToPosition(0x1, 0, 0) == 0x0);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x2, 1, 0) == 0x0);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x4, 2, 0) == 0x0);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x8, 3, 0) == 0x0);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x10, 4, 0) == 0x0);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x20, 5, 0) == 0x0);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x40, 6, 0) == 0x0);
        DOCTEST_CHECK(Math::AssignBitToPosition(0x80, 7, 0) == 0x0);
    }

    DOCTEST_SUBCASE("ReadBitPosition")
    {
        DOCTEST_CHECK(Math::ReadBitPosition(0x1, 0) == true);
        DOCTEST_CHECK(Math::ReadBitPosition(0x2, 1) == true);
        DOCTEST_CHECK(Math::ReadBitPosition(0x4, 2) == true);
        DOCTEST_CHECK(Math::ReadBitPosition(0x8, 3) == true);
        DOCTEST_CHECK(Math::ReadBitPosition(0x10, 4) == true);
        DOCTEST_CHECK(Math::ReadBitPosition(0x20, 5) == true);
        DOCTEST_CHECK(Math::ReadBitPosition(0x40, 6) == true);
        DOCTEST_CHECK(Math::ReadBitPosition(0x80, 7) == true);

        DOCTEST_CHECK(Math::ReadBitPosition(0x0, 0) == false);
        DOCTEST_CHECK(Math::ReadBitPosition(0x0, 1) == false);
        DOCTEST_CHECK(Math::ReadBitPosition(0x0, 2) == false);
        DOCTEST_CHECK(Math::ReadBitPosition(0x0, 3) == false);
        DOCTEST_CHECK(Math::ReadBitPosition(0x0, 4) == false);
        DOCTEST_CHECK(Math::ReadBitPosition(0x0, 5) == false);
        DOCTEST_CHECK(Math::ReadBitPosition(0x0, 6) == false);
        DOCTEST_CHECK(Math::ReadBitPosition(0x0, 7) == false);
    }

    DOCTEST_SUBCASE("GetLeastSignificantBit")
    {
        constexpr Size case1 = 0x2 | 0x4;
        constexpr Size case2 = 0x8 | 0x10;

        DOCTEST_CHECK(Math::GetLeastSignificantBit(case1) == 0x2);
        DOCTEST_CHECK(Math::GetLeastSignificantBit(case2) == 0x8);
    }

    DOCTEST_SUBCASE("Clamp")
    {

        DOCTEST_CHECK(Math::Clamp(0.0f, 0.25f, 0.75f) == 0.25f);
        DOCTEST_CHECK(Math::Clamp(0.5f, 0.25f, 0.75f) == 0.5f);
        DOCTEST_CHECK(Math::Clamp(1.0f, 0.25f, 0.75f) == 0.75f);
    }

    DOCTEST_SUBCASE("IsInRange")
    {
        DOCTEST_CHECK(!Math::IsInRange(0.0f, 0.25f, 0.75f));
        DOCTEST_CHECK(Math::IsInRange(0.5f, 0.25f, 0.75f));
        DOCTEST_CHECK(!Math::IsInRange(1.0f, 0.25f, 0.75f));
    }

    DOCTEST_SUBCASE("Max")
    {
        DOCTEST_CHECK(Math::Max(0.0f, 0.25f) == 0.25f);
        DOCTEST_CHECK(Math::Max(0.5f, 0.25f) == 0.5f);
        DOCTEST_CHECK(Math::Max(1.0f, 0.25f) == 1.0f);
    }

    DOCTEST_SUBCASE("Min")
    {
        DOCTEST_CHECK(Math::Min(0.0f, 0.25f) == 0.0f);
        DOCTEST_CHECK(Math::Min(0.5f, 0.25f) == 0.25f);
        DOCTEST_CHECK(Math::Min(1.0f, 0.25f) == 0.25f);
    }

    DOCTEST_SUBCASE("Abs")
    {
        DOCTEST_CHECK(Math::Abs(0.0f) == 0.0f);
        DOCTEST_CHECK(Math::Abs(0.25f) == 0.25f);
        DOCTEST_CHECK(Math::Abs(-0.25f) == 0.25f);
        DOCTEST_CHECK(Math::Abs(0.5f) == 0.5f);
        DOCTEST_CHECK(Math::Abs(-0.5f) == 0.5f);
        DOCTEST_CHECK(Math::Abs(1.0f) == 1.0f);
        DOCTEST_CHECK(Math::Abs(-1.0f) == 1.0f);
    }

    DOCTEST_SUBCASE("RoundUp")
    {
        DOCTEST_CHECK(Math::RoundUp(23, 10) == 30);
        DOCTEST_CHECK(Math::RoundUp(26, 10) == 30);
        DOCTEST_CHECK(Math::RoundUp(30, 10) == 30);
        DOCTEST_CHECK(Math::RoundUp(10, 10) == 10);
        DOCTEST_CHECK(Math::RoundUp(-2, 10) == 0);
    }


    DOCTEST_SUBCASE("RoundToNextPowerOfTwo")
    {
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(0)) == 1);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(1)) == 1);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(2)) == 2);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(3)) == 4);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(4)) == 4);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(5)) == 8);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(8)) == 8);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(9)) == 16);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(16)) == 16);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(17)) == 32);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(32)) == 32);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(33)) == 64);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(64)) == 64);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(65)) == 128);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(128)) == 128);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(129)) == 256);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(256)) == 256);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(257)) == 512);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(512)) == 512);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(513)) == 1024);
        DOCTEST_CHECK(Math::RoundToNextPowerOfTwo(Size(1024)) == 1024);
    }
}