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

/// \brief Struct for storing the result of packed sprites.
///
template <class Key, class Hasher = Hash<Key>>
struct SpritePackingResult
{
    /// \brief The size of the packed texture.
    ///
    Vector2UI PackedTextureSize;

    /// \brief Contains all the locations of the packed sprites in the packed texture.
    ///
    HashMap<Key, RectangleUI, Hasher> SpriteLocations;
};

/// \brief Packs sprites into a texture.
///
/// \tparam Key The type of the key used to store the sprite.
///
/// \param[in] spriteList The list of sprites to pack.
///
template <class Key = Size, class Hasher = Hash<Key>>
AXIS_NODISCARD SpritePackingResult<Key, Hasher> PackSprite(List<Pair<Key, Vector2UI>>&& spiteList) noexcept;

/// \brief Packs sprites into a texture.
///
/// \tparam Key The type of the key used to store the sprite.
///
/// \param[in] spriteList The list of sprites to pack.
///
template <class Key = Size, class Hasher = Hash<Key>>
AXIS_NODISCARD SpritePackingResult<Key, Hasher> PackSprite(const List<Pair<Key, Vector2UI>>& spiteList) noexcept;

} // namespace Axis

#include "../../Private/Axis/PackSpriteImpl.inl"

#endif // AXIS_RENDERER_SPRITEPACKER_HPP