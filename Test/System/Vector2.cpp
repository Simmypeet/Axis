#include <Axis/System>
#include <doctest.h>

using namespace Axis;
using namespace Axis::System;

DOCTEST_TEST_CASE("Vector2 template class : [Axis-System]")
{
    DOCTEST_SUBCASE("`Axis::Vector2` constructors")
    {
        DOCTEST_SUBCASE("Default constructor")
        {

            // Default vector
            Vector2<Int32> vector;

            // Checks if the vector is empty
            DOCTEST_CHECK(vector.X == 0);
            DOCTEST_CHECK(vector.Y == 0);

            // Default vector in float
            Vector2<Float32> vectorF;

            // Checks if the vector is empty
            DOCTEST_CHECK(vectorF.X == 0.0f);
            DOCTEST_CHECK(vectorF.Y == 0.0f);
        }

        DOCTEST_SUBCASE("Coordinate constructor")
        {

            // Constructs a vector
            Vector2<Int32> vector(1, 2.0f);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == 1);
            DOCTEST_CHECK(vector.Y == 2);

            // Constructs a vector in float
            Vector2<Float32> vectorF = {1.0f, 2.0f};

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vectorF.X == 1.0f);
            DOCTEST_CHECK(vectorF.Y == 2.0f);
        }

        DOCTEST_SUBCASE("Conversion constructor")
        {

            // Constructs a vector
            Vector2<Int32> vector(Vector2<Float32>(1.0f, 2.0f));

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == 1);
            DOCTEST_CHECK(vector.Y == 2);
        }
    }

    DOCTEST_SUBCASE("Arithmetic operators")
    {
        DOCTEST_SUBCASE("Addition")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(3, 4);
            Vector2<Int32> vector3 = vector1 + vector2;

            DOCTEST_CHECK(vector3.X == 4);
            DOCTEST_CHECK(vector3.Y == 6);
        }

        DOCTEST_SUBCASE("Subtraction")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(3, 4);
            Vector2<Int32> vector3 = vector1 - vector2;

            DOCTEST_CHECK(vector3.X == -2);
            DOCTEST_CHECK(vector3.Y == -2);
        }

        DOCTEST_SUBCASE("Multiplication")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(3, 4);
            Vector2<Int32> vector3 = vector1 * vector2;

            DOCTEST_CHECK(vector3.X == 3);
            DOCTEST_CHECK(vector3.Y == 8);
        }

        DOCTEST_SUBCASE("Division")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(3, 4);
            Vector2<Int32> vector3 = vector1 / vector2;

            DOCTEST_CHECK(vector3.X == 1 / 3);
            DOCTEST_CHECK(vector3.Y == 2 / 4);
        }

        DOCTEST_SUBCASE("Multiplication by a scalar value")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2 = vector1 * 2;

            DOCTEST_CHECK(vector2.X == 2);
            DOCTEST_CHECK(vector2.Y == 4);
        }

        DOCTEST_SUBCASE("Division by a scalar value")
        {

            Vector2<Int32> vector1(2, 4);
            Vector2<Int32> vector2 = vector1 / 2;

            DOCTEST_CHECK(vector2.X == 1);
            DOCTEST_CHECK(vector2.Y == 2);
        }
    }

    DOCTEST_SUBCASE("Compound arithmetic operators")
    {
        DOCTEST_SUBCASE("Addition")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(3, 4);

            vector1 += vector2;

            DOCTEST_CHECK(vector1.X == 4);
            DOCTEST_CHECK(vector1.Y == 6);
        }

        DOCTEST_SUBCASE("Subtraction")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(3, 4);

            vector1 -= vector2;

            DOCTEST_CHECK(vector1.X == -2);
            DOCTEST_CHECK(vector1.Y == -2);
        }

        DOCTEST_SUBCASE("Multiplication")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(3, 4);

            vector1 *= vector2;

            DOCTEST_CHECK(vector1.X == 3);
            DOCTEST_CHECK(vector1.Y == 8);
        }

        DOCTEST_SUBCASE("Division")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(3, 4);

            vector1 /= vector2;

            DOCTEST_CHECK(vector1.X == 1 / 3);
            DOCTEST_CHECK(vector1.Y == 2 / 4);
        }

        DOCTEST_SUBCASE("Multiplication by a scalar value")
        {

            Vector2<Int32> vector1(1, 2);

            vector1 *= 2;

            DOCTEST_CHECK(vector1.X == 2);
            DOCTEST_CHECK(vector1.Y == 4);
        }

        DOCTEST_SUBCASE("Division by a scalar value")
        {

            Vector2<Int32> vector1(2, 4);

            vector1 /= 2;

            DOCTEST_CHECK(vector1.X == 1);
            DOCTEST_CHECK(vector1.Y == 2);
        }
    }

    DOCTEST_SUBCASE("Comparison operators")
    {
        DOCTEST_SUBCASE("Equality")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(1, 2);
            Vector2<Int32> vector3(2, 2);

            DOCTEST_CHECK(vector1 == vector2);
            DOCTEST_CHECK(!(vector1 == vector3));
        }

        DOCTEST_SUBCASE("Inequality")
        {

            Vector2<Int32> vector1(1, 2);
            Vector2<Int32> vector2(1, 2);
            Vector2<Int32> vector3(2, 2);

            DOCTEST_CHECK(!(vector1 != vector2));
            DOCTEST_CHECK(vector1 != vector3);
        }
    }

    DOCTEST_SUBCASE("Magnitude")
    {

        Vector2<Int32> vector(3, 4);

        DOCTEST_CHECK(Math::IsFloatEqual(vector.GetMagnitude(), 5.0f));
    }

    DOCTEST_SUBCASE("Normalize")
    {

        Vector2<Float32> vector(3, 4);
        vector.Normalize();

        DOCTEST_CHECK(Math::IsFloatEqual(vector.X, 3.0f / 5.0f));
        DOCTEST_CHECK(Math::IsFloatEqual(vector.Y, 4.0f / 5.0f));
    }

    DOCTEST_SUBCASE("DotProduct")
    {

        Vector2<Int32> vector1(1, 2);
        Vector2<Int32> vector2(3, 4);

        DOCTEST_CHECK(vector1.DotProduct(vector2) == 11);
    }
}