/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_MATRIX_HPP
#define AXIS_SYSTEM_MATRIX_HPP
#pragma once

#include "Config.hpp"
#include "Vector3.hpp"
#include <type_traits>

namespace Axis
{

namespace System
{

/// \brief Flag determines the default template parameter for  matrices.
constexpr Bool DefaultIsColumnMajor = true;

/// \brief Represents 2-Dimensional mathematical matrix template definition.
///
/// \tparam T The arithmetic value type used in calculation.
/// \tparam RowSize The size of the row matrix.
/// \tparam ColumnSize The size of the column matrix.
/// \tparam IsColumnMajor Specifies the memory layout of the matrix.
template <ArithmeticType T, Size RowSize, Size ColumnSize, Bool IsColumnMajor = DefaultIsColumnMajor>
struct Matrix
{
public:
    /// \brief Identifies whether the matrix is square matrix or not.
    static constexpr Bool IsSquareMatrix = RowSize == ColumnSize;

    /// \brief Default constructor
    constexpr Matrix() noexcept;

    /// \brief Constructs the matrix with the static array.
    constexpr Matrix(const T (&matrix)[RowSize][ColumnSize]) noexcept;

    /// \brief Gets the element of the matrix. using function operator.
    ///
    /// \param[in] row Specifies the row index. (Starts from zero).
    /// \param[in] column Specifies the column index. (Starts from zero).
    AXIS_NODISCARD constexpr const T& operator()(Size row, Size column) const noexcept;

    /// \brief Gets the element of the matrix. using function operator.
    ///
    /// \param[in] row Specifies the row index. (Starts from zero).
    /// \param[in] column Specifies the column index. (Starts from zero).
    AXIS_NODISCARD constexpr T& operator()(Size row, Size column) noexcept;

    /// \brief Gets the element of the matrix.
    ///
    /// \param[in] row Specifies the row index. (Starts from zero).
    /// \param[in] column Specifies the column index. (Starts from zero).
    AXIS_NODISCARD constexpr const T& At(Size row, Size column) const noexcept;

    /// \brief Gets the element of the matrix.
    ///
    /// \param[in] column Specifies the column index. (Starts from zero).
    /// \param[in] row Specifies the row index. (Starts from zero).
    AXIS_NODISCARD constexpr T& At(Size row, Size column) noexcept;

    /// \brief Mutiplies two matrices together.
    ///
    /// \warning Matrix size's condition must satisfied in order to mutiplies the matrix.
    template <Size OtherRowSize, Size OtherColumnSize>
    AXIS_NODISCARD constexpr Matrix<T, RowSize, OtherColumnSize, IsColumnMajor> operator*(const Matrix<T, OtherRowSize, OtherColumnSize, IsColumnMajor>& other) const noexcept;

    /// \brief Multiplies two matrices together and store the result.
    constexpr Matrix& operator*=(const Matrix& other) noexcept;

    /// \brief Mutiplies matrix with scalar.
    AXIS_NODISCARD constexpr Matrix operator*(T scalar) const noexcept;

    static_assert(RowSize, "`template <class T, Size ColumnSize, Size RowSize, Bool IsColumnMajor> Axis::Math::Matrix` template parameter `RowSize` was 0.");
    static_assert(ColumnSize, "`template <class T, Size ColumnSize, Size RowSize, Bool IsColumnMajor> Axis::Math::Matrix` template parameter `ColumnSize` was 0.");

    /// \brief Gets translation matrix.
    ///
    /// \param[in] translation Translation for each axis of the matrix (x, y, z).
    constexpr static Matrix GetTranslationMatrix(const Vector3<T>& translation) noexcept requires(RowSize == 4 && ColumnSize == 4);

    /// \brief Gets scaling matrix.
    ///
    /// \param[in] scaling Scaling for each axis of the matrix (x, y, z).
    constexpr static Matrix GetScalingMatrix(const Vector3<T>& translation) noexcept requires(RowSize == 4 && ColumnSize == 4);

    /// \brief Gets identity matrix.
    constexpr static Matrix GetIdentityMatrix() noexcept requires(RowSize == 4 && ColumnSize == 4);

    /// \brief Creates a matrix for a symmetric perspective-view frustum.
    ///
    /// \remark This code is taken from the GLM library
    ///          Github repo: https://github.com/Groovounet/glm
    constexpr static Matrix GetMatrixPerspective(T fovy,
                                                 T aspect,
                                                 T zNear,
                                                 T zFar) noexcept requires(RowSize == 4 && ColumnSize == 4);

    /// \brief Creates a look at view matrix.
    ///
    /// \remark This code is taken from the GLM library
    ///         Github repo: https://github.com/Groovounet/glm
    constexpr static Matrix GetMatrixLookAt(const Vector3<T>& eye,
                                            const Vector3<T>& center,
                                            const Vector3<T>& up) noexcept requires(RowSize == 4 && ColumnSize == 4);

    /// \brief Creates a rotation matrix along the Y-axis.
    ///
    /// \param[in] randian angle in radian
    constexpr static Matrix GetMatrixRotationY(T randian) noexcept requires(RowSize == 4 && ColumnSize == 4);

private:
    T _matrices[ColumnSize * RowSize] = {};
};

/// \brief Templated 4 by 4 column major matrix type.
template <ArithmeticType T>
using Matrix4x4 = Matrix<T, 4, 4, DefaultIsColumnMajor>;

/// \brief 4 by 4, floating-point, column-major matrix.
typedef Matrix<Float32, 4, 4, DefaultIsColumnMajor> FloatMatrix4x4;

} // namespace System

} // namespace Axis

#include "../../Private/Axis/MatrixImpl.inl"

#endif // AXIS_SYSTEM_MATRIX_HPP