/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_RENDERER_SPRITEPACKER_HPP
#define AXIS_RENDERER_SPRITEPACKER_HPP
#pragma once

#include "../../System/Include/Axis/HashMap.hpp"
#include "../../System/Include/Axis/List.hpp"
#include "../../System/Include/Axis/Rectangle.hpp"
#include "../../System/Include/Axis/Vector2.hpp"
#include "RendererExport.hpp"


namespace Axis
{

namespace Renderer
{

/// \brief Struct for storing the result of packed sprites.
template <System::RawType Key, System::HasherType<Key> Hasher = System::Hash<Key>>
struct SpritePackingResult
{
    /// \brief The size of the packed texture.
    System::Vector2UI PackedTextureSize;

    /// \brief Contains all the locations of the packed sprites in the packed texture.
    System::HashMap<Key, System::RectangleUI, Hasher> SpriteLocations;
};

/// \brief Packs sprites into a texture.
///
/// \tparam Key The type of the key used to store the sprite.
///
/// \param[in] spriteList The list of sprites to pack.
///
template <System::RawType Key = Size, System::HasherType<Key> Hasher = System::Hash<Key>>
AXIS_NODISCARD SpritePackingResult<Key, Hasher> PackSprite(System::List<System::Pair<Key, System::Vector2UI>>&& spiteList);

/// \brief Packs sprites into a texture.
///
/// \tparam Key The type of the key used to store the sprite.
///
/// \param[in] spriteList The list of sprites to pack.
///
template <System::RawType Key = Size, System::HasherType<Key> Hasher = System::Hash<Key>>
AXIS_NODISCARD SpritePackingResult<Key, Hasher> PackSprite(const System::List<System::Pair<Key, System::Vector2UI>>& spiteList);

} // namespace Renderer

} // namespace Axis

#include "../../Private/Axis/PackSpriteImpl.inl"

#endif // AXIS_RENDERER_SPRITEPACKER_HPP