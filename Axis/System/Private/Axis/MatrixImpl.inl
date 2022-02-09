/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_MATH_MATRIXIMPL_INL
#define AXIS_MATH_MATRIXIMPL_INL
#pragma once

#include "../../Include/Axis/Math.hpp"
#include "../../Include/Axis/Matrix.hpp"
#include "../../Include/Axis/Utility.hpp"
#include <cmath>

namespace Axis
{

namespace System
{

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor>::Matrix() noexcept {}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor>::Matrix(const T (&matrix)[RowSize][ColumnSize]) noexcept
{
    for (Size row = 0; row < RowSize; row++)
    {
        for (Size column = 0; column < ColumnSize; column++)
        {
            At(row, column) = matrix[row][column];
        }
    }
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr const T& Matrix<T, RowSize, ColumnSize, IsColumnMajor>::operator()(Size row, Size column) const noexcept
{
    AXIS_VALIDATE(row < RowSize, "`row` index was out of range!");
    AXIS_VALIDATE(row < RowSize, "`column` index was out of range!");

    if constexpr (IsColumnMajor)
    {
        return _matrices[column * RowSize + row];
    }
    else
    {
        return _matrices[row * ColumnSize + column];
    }
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr T& Matrix<T, RowSize, ColumnSize, IsColumnMajor>::operator()(Size row, Size column) noexcept
{
    AXIS_VALIDATE(row < RowSize, "`row` index was out of range!");
    AXIS_VALIDATE(row < RowSize, "`column` index was out of range!");

    if constexpr (IsColumnMajor)
    {
        return _matrices[column * RowSize + row];
    }
    else
    {
        return _matrices[row * ColumnSize + column];
    }
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr const T& Matrix<T, RowSize, ColumnSize, IsColumnMajor>::At(Size row, Size column) const noexcept
{
    AXIS_VALIDATE(row < RowSize, "`row` index was out of range!");
    AXIS_VALIDATE(row < RowSize, "`column` index was out of range!");

    if constexpr (IsColumnMajor)
    {
        return _matrices[column * RowSize + row];
    }
    else
    {
        return _matrices[row * ColumnSize + column];
    }
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr T& Matrix<T, RowSize, ColumnSize, IsColumnMajor>::At(Size row, Size column) noexcept
{
    AXIS_VALIDATE(row < RowSize, "`row` index was out of range!");
    AXIS_VALIDATE(row < RowSize, "`column` index was out of range!");

    if constexpr (IsColumnMajor)
    {
        return _matrices[column * RowSize + row];
    }
    else
    {
        return _matrices[row * ColumnSize + column];
    }
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
template <Size OtherRowSize, Size OtherColumnSize>
inline constexpr Matrix<T, RowSize, OtherColumnSize, IsColumnMajor> Matrix<T, RowSize, ColumnSize, IsColumnMajor>::operator*(const Matrix<T, OtherRowSize, OtherColumnSize, IsColumnMajor>& other) const noexcept
{
    static_assert(ColumnSize == OtherRowSize, "Column matrix size didn't not match to the other row matrix size!");

    Matrix<T, RowSize, OtherColumnSize, IsColumnMajor> matrixToReturn = {};

    for (Size row = 0; row < RowSize; row++)
    {
        for (Size column = 0; column < OtherColumnSize; column++)
        {
            T result = 0;
            for (Size commonSize = 0; commonSize < ColumnSize; commonSize++)
                result += At(row, commonSize) * other(commonSize, column);

            matrixToReturn(row, column) = result;
        }
    }

    return matrixToReturn;
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor>& Matrix<T, RowSize, ColumnSize, IsColumnMajor>::operator*=(const Matrix<T, RowSize, ColumnSize, IsColumnMajor>& other) noexcept
{
    auto result  = *this * other;
    return *this = result;
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor> Matrix<T, RowSize, ColumnSize, IsColumnMajor>::operator*(T scalar) const noexcept
{
    Matrix<T, RowSize, ColumnSize, IsColumnMajor> matrixToReturn = {};

    for (Size row = 0; row < RowSize; row++)
    {
        for (Size column = 0; column < ColumnSize; column++)
        {
            matrixToReturn(row, column) = At(row, column) * scalar;
        }
    }

    return matrixToReturn;
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor> Matrix<T, RowSize, ColumnSize, IsColumnMajor>::GetTranslationMatrix(const Vector3<T>& translation) noexcept requires(RowSize == 4 && ColumnSize == 4)
{
    Matrix<T, 4, 4, IsColumnMajor> matrix = {};
    matrix(0, 0)                          = 1;
    matrix(1, 1)                          = 1;
    matrix(2, 2)                          = 1;
    matrix(3, 3)                          = 1;
    matrix(0, 3)                          = translation.X;
    matrix(1, 3)                          = translation.Y;
    matrix(2, 3)                          = translation.Z;

    return matrix;
}


template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor> Matrix<T, RowSize, ColumnSize, IsColumnMajor>::GetScalingMatrix(const Vector3<T>& scale) noexcept requires(RowSize == 4 && ColumnSize == 4)
{
    Matrix<T, 4, 4, IsColumnMajor> matrix = {};
    matrix(0, 0)                          = scale.X;
    matrix(1, 1)                          = scale.Y;
    matrix(2, 2)                          = scale.Z;
    matrix(3, 3)                          = 1;

    return matrix;
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor> Matrix<T, RowSize, ColumnSize, IsColumnMajor>::GetIdentityMatrix() noexcept requires(RowSize == 4 && ColumnSize == 4)
{
    Matrix<T, 4, 4, IsColumnMajor> matrix;

    matrix(0, 0) = T(1);
    matrix(1, 1) = T(1);
    matrix(2, 2) = T(1);
    matrix(3, 3) = T(1);

    return matrix;
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor> Matrix<T, RowSize, ColumnSize, IsColumnMajor>::GetMatrixPerspective(T fovy,
                                                                                                                                   T aspect,
                                                                                                                                   T zNear,
                                                                                                                                   T zFar) noexcept requires(RowSize == 4 && ColumnSize == 4)
{
    T range  = std::tan(fovy / T(2)) * zNear;
    T left   = -range * aspect;
    T right  = range * aspect;
    T bottom = -range;
    T top    = range;

    Matrix<T, 4, 4, IsColumnMajor> result;
    result(0, 0) = (T(2) * zNear) / (right - left);
    result(1, 1) = (T(2) * zNear) / (top - bottom);
    result(2, 2) = -(zFar + zNear) / (zFar - zNear);
    result(3, 2) = -T(1);
    result(2, 3) = -(T(2) * zFar * zNear) / (zFar - zNear);

    return result;
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor> Matrix<T, RowSize, ColumnSize, IsColumnMajor>::GetMatrixLookAt(const Vector3<T>& eye,
                                                                                                                              const Vector3<T>& center,
                                                                                                                              const Vector3<T>& up) noexcept requires(RowSize == 4 && ColumnSize == 4)
{
    Vector3<T> f = (center - eye).NormalizeCopy();
    Vector3<T> u = up.NormalizeCopy();
    Vector3<T> s = (f.CrossProduct(u)).NormalizeCopy();
    u            = s.CrossProduct(f);

    Matrix<T, 4, 4, IsColumnMajor> Result = GetIdentityMatrix();
    Result(0, 0)                          = s.X;
    Result(0, 1)                          = s.Y;
    Result(0, 2)                          = s.Z;
    Result(1, 0)                          = u.X;
    Result(1, 1)                          = u.Y;
    Result(1, 2)                          = u.Z;
    Result(2, 0)                          = -f.X;
    Result(2, 1)                          = -f.Y;
    Result(2, 2)                          = -f.Z;
    Result(0, 3)                          = -s.DotProduct(eye);
    Result(1, 3)                          = -(u.DotProduct(eye));
    Result(2, 3)                          = f.DotProduct(eye);

    return Result;
}

template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor>
inline constexpr Matrix<T, RowSize, ColumnSize, IsColumnMajor> Matrix<T, RowSize, ColumnSize, IsColumnMajor>::GetMatrixRotationY(T radian) noexcept requires(RowSize == 4 && ColumnSize == 4)
{
    // round 2 pi.
    Matrix result = GetIdentityMatrix();

    T cosVal = std::cos(radian);
    T sinVal = std::sin(radian);

    result(0, 0) = cosVal;
    result(0, 2) = sinVal;
    result(2, 0) = -sinVal;
    result(2, 2) = cosVal;

    return result;
}

} // namespace System

} // namespace Axis

#endif // AXIS_MATH_MATRIXIMPL_INL