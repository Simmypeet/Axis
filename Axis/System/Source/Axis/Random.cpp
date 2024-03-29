/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/SystemPch.hpp>

#include <Axis/Random.hpp>
#include <chrono>

namespace Axis
{

namespace System
{

using namespace std::chrono;

Random::Random() noexcept :
    Random((Int32)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()) {}

} // namespace System

} // namespace Axis