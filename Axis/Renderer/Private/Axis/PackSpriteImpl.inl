/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_RENDERER_PACKSPRITEIMPL_INL
#define AXIS_RENDERER_PACKSPRITEIMPL_INL
#pragma once

#include "../../Include/Axis/PackSprite.hpp"
#include "../../System/Include/Axis/SmartPointer.hpp"
#include <algorithm>

namespace Axis
{

namespace Renderer
{

namespace Detail
{

/// Reference: https://codeincomplete.com/articles/bin-packing/
///
/// Packs the sprites into a texture using binary tree bin packing algorithm.

// Data structure for storing the packed sprites.
struct Node
{
    Bool                        Used  = false;   //< Whether the node area is occupied.
    System::RectangleUI         Area  = {};      //< The area of the node.
    System::SharedPointer<Node> Right = nullptr; //< The right child of the node.
    System::SharedPointer<Node> Down  = nullptr; //< The down child of the node.
};

inline System::SharedPointer<Node> FindNode(const System::SharedPointer<Node>& root, const System::Vector2UI& size)
{
    if (root->Used)
    {
        auto rightNode = FindNode(root->Right, size);
        if (rightNode)
            return rightNode;

        return FindNode(root->Down, size);
    }
    else if ((size.X <= root->Area.Width) && (size.Y <= root->Area.Height))
        return root;
    else
        return nullptr;
}

inline System::RectangleUI SplitNode(const System::SharedPointer<Node>& node, const System::Vector2UI& size)
{
    node->Used = true;

    // Down node
    auto downNode  = System::MakeShared<Node>();
    downNode->Area = {node->Area.X, node->Area.Y + size.Y, node->Area.Width, node->Area.Height - size.Y};
    node->Down     = downNode;

    // Right node
    auto rightNode  = System::MakeShared<Node>();
    rightNode->Area = {node->Area.X + size.X, node->Area.Y, node->Area.Width - size.X, size.Y};
    node->Right     = rightNode;

    return {node->Area.X, node->Area.Y, size.X, size.Y};
}

inline System::RectangleUI GrowRight(System::SharedPointer<Node>& root, const System::Vector2UI& size)
{
    auto newRoot  = Axis::System::MakeShared<Node>();
    newRoot->Used = true;
    newRoot->Area = {0, 0, root->Area.Width + size.X, root->Area.Height};
    newRoot->Down = root;

    newRoot->Right       = System::MakeShared<Node>();
    newRoot->Right->Area = {root->Area.Width, 0, size.X, root->Area.Height};

    root = newRoot;

    if (auto node = FindNode(root, size))
        return SplitNode(node, size);
    else
        throw System::InvalidOperationException("Failed to pack sprites into texture.");
}

inline System::RectangleUI GrowDown(System::SharedPointer<Node>& root, const System::Vector2UI& size)
{
    auto newRoot   = Axis::System::MakeShared<Node>();
    newRoot->Used  = true;
    newRoot->Area  = {0, 0, root->Area.Width, root->Area.Height + size.Y};
    newRoot->Right = root;

    newRoot->Down       = System::MakeShared<Node>();
    newRoot->Down->Area = {0, root->Area.Height, root->Area.Width, size.Y};

    root = newRoot;

    if (auto node = FindNode(root, size))
        return SplitNode(node, size);
    else
        throw System::InvalidOperationException("Failed to pack sprites into texture.");
}


inline System::RectangleUI GrowNode(System::SharedPointer<Node>& root, const System::Vector2UI& size)
{
    auto canGrowDown  = (size.X <= root->Area.Width);
    auto canGrowRight = (size.Y <= root->Area.Height);

    auto shouldGrowRight = canGrowRight && (root->Area.Height >= (root->Area.Width + size.X)); // attempt to keep square-ish by growing right when height is much greater than width
    auto shouldGrowDown  = canGrowDown && (root->Area.Width >= (root->Area.Height + size.Y));  // attempt to keep square-ish by growing down  when width  is much greater than height

    if (shouldGrowRight)
        return GrowRight(root, size);
    else if (shouldGrowDown)
        return GrowDown(root, size);
    else if (canGrowRight)
        return GrowRight(root, size);
    else if (canGrowDown)
        return GrowDown(root, size);
    else
        throw System::InvalidOperationException("Failed to pack sprites into texture.");
}

template <System::RawType Key, System::HasherType<Key> Hasher>
inline SpritePackingResult<Key, Hasher> PackSpriteInternal(const System::List<System::Pair<Key, System::Vector2UI>>& spriteList)
{
    System::SharedPointer<Node> root = Axis::System::MakeShared<Node>();

    if (spriteList[0].Second.X == 0 || spriteList[0].Second.Y == 0)
        throw System::InvalidArgumentException("`spriteList` contained sprite with invalid size!");

    root->Area.Width  = spriteList[0].Second.X;
    root->Area.Height = spriteList[0].Second.Y;

    SpritePackingResult<Key, Hasher> spritePackingResult = {};

    for (const auto& sprite : spriteList)
    {
        auto                node       = FindNode(root, sprite.Second);
        System::RectangleUI spriteArea = {};

        if (node)
            spriteArea = SplitNode(node, sprite.Second);
        else
            spriteArea = GrowNode(root, sprite.Second);

        spritePackingResult.SpriteLocations.Insert({sprite.First, spriteArea});
    }


    spritePackingResult.PackedTextureSize = {root->Area.Width, root->Area.Height};

    return spritePackingResult;
}

// Predicate for sorting sprites by size
template <System::RawType Key>
constexpr auto Predicate = [](const System::Pair<Key, System::Vector2UI>& lhs, const System::Pair<Key, System::Vector2UI>& rhs) noexcept {
    return System::Math::Max(lhs.Second.X, lhs.Second.Y) > System::Math::Max(rhs.Second.X, rhs.Second.Y);
};

} // namespace Detail

template <System::RawType Key, System::HasherType<Key> Hasher>
inline SpritePackingResult<Key, Hasher> PackSprite(System::List<System::Pair<Key, System::Vector2UI>>&& spriteList)
{
    if (!spriteList)
        throw System::InvalidArgumentException("`spriteList` was nullptr!");

    // Moves the sprite list to the new one
    auto sortedSpriteList = std::move(spriteList);

    // Sorts the sprite list by the largest dimension
    std::sort(sortedSpriteList.begin(), sortedSpriteList.end(), Detail::Predicate<Key>);

    // Packs the sprites
    return Detail::PackSpriteInternal<Key, Hasher>(sortedSpriteList);
}

template <System::RawType Key, System::HasherType<Key> Hasher>
inline SpritePackingResult<Key, Hasher> PackSprite(const System::List<System::Pair<Key, System::Vector2UI>>& spriteList)
{
    if (!spriteList)
        throw System::InvalidArgumentException("`spriteList` was nullptr!");

    // Creates a copy of the sprite list
    auto sortedSpriteList = spriteList;

    // Sorts the sprite list by the largest dimension
    std::sort(sortedSpriteList.begin(), sortedSpriteList.end(), Detail::Predicate<Key>);

    // Packs the sprites
    return Detail::PackSpriteInternal<Key, Hasher>(sortedSpriteList);
}

} // namespace Renderer

} // namespace Axis

#endif // AXIS_RENDERER_PACKSPRITEIMPL_INL
