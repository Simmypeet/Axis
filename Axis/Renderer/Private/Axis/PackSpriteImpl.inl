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

namespace Detail
{

template <class Key, class Hasher>
inline SpritePackingResult<Key, Hasher> PackSpriteInternal(const List<Pair<Key, Vector2UI>>& sortedSpriteList) noexcept
{
    /// Reference: https://codeincomplete.com/articles/bin-packing/
    ///
    /// Packs the sprites into a texture using binary tree bin packing algorithm.

    // Data structure for storing the packed sprites.
    struct Node
    {
        Bool                Used  = false;   //< Whether the node area is occupied.
        RectangleUI         Area  = {};      //< The area of the node.
        UniquePointer<Node> Right = nullptr; //< The right child of the node.
        UniquePointer<Node> Down  = nullptr; //< The down child of the node.
    };

    // The root of the node tree
    UniquePointer<Node> root = UniquePointer<Node>(Axis::New<Node>());

    // Picks the first sprite as the size
    root->Area = RectangleUI(0, 0, sortedSpriteList[0].Second.X, sortedSpriteList[0].Second.Y);

    // Stores the result here
    SpritePackingResult<Key, Hasher> result = {};

    // Loops through the rest of the sprites
    for (Size i = 0; i < sortedSpriteList.GetLength(); i++)
    {
        // Invalid sprite size!
        if (sortedSpriteList[i].Second.X == 0 || sortedSpriteList[i].Second.Y == 0)
            continue;

        // Finds the available node and assigns the result here
        UniquePointer<Node>* resultNode = nullptr;

        constexpr auto FindNode = [](UniquePointer<Node>& node, UniquePointer<Node>*& nodeOut, const Vector2UI& size) -> void {
            constexpr auto NodeFit = [](UniquePointer<Node>& node, const Vector2UI& size) -> Bool {
                if (!node->Used)
                    return node->Area.Width >= size.X && node->Area.Height >= size.Y;
                else
                    return false;
            };

            // Stores temporary stack here
            List<UniquePointer<Node>*> stack = {};

            // Current node
            UniquePointer<Node>* current = &node;

            while (*current != nullptr || stack.GetLength() > 0)
            {
                while (*current != nullptr)
                {
                    if (NodeFit(*current, size))
                    {
                        // Found a node that can fit the sprite
                        nodeOut = current;
                        return;
                    }

                    // Pushes the current node to the stack
                    stack.Append(current);

                    current = &((*current)->Right);
                }

                // Pop last node from the stack
                current = stack[stack.GetLength() - 1];

                if (NodeFit(*current, size))
                {
                    // Found a node that can fit the sprite
                    nodeOut = current;
                    return;
                }

                stack.RemoveBack();

                current = &((*current)->Down);
            }
        };

        constexpr auto SplitNode = [](UniquePointer<Node>& node, const Vector2UI& size) -> void {
            node->Right = UniquePointer<Node>(Axis::New<Node>());
            node->Down  = UniquePointer<Node>(Axis::New<Node>());

            node->Right->Area = RectangleUI(node->Area.X + size.X, node->Area.Y, node->Area.Width - size.X, node->Area.Height);
            node->Down->Area  = RectangleUI(node->Area.X, node->Area.Y + size.Y, node->Area.Width, node->Area.Height - size.Y);
        };

        // Finds the available node and assigns the result here
        FindNode(root, resultNode, sortedSpriteList[i].Second);

        // If the node is found then assign the sprite to it
        if (resultNode != nullptr)
        {
            // Assigns the sprite to the node
            (*resultNode)->Used = true;

            // Assigns the sprite to the result
            result.SpriteLocations.Insert({sortedSpriteList[i].First, RectangleUI((*resultNode)->Area.X, (*resultNode)->Area.Y, sortedSpriteList[i].Second.X, sortedSpriteList[i].Second.Y)});

            // Split the node
            SplitNode(*resultNode, sortedSpriteList[i].Second);
        }
        else
        {
            /// If the node is not found we need to create a new node.
            ///
            /// Tries to grow the node in the square shape, either growing right-wards or downwards.

            // Grows node in right-wards direction
            constexpr auto GrowRight = [](UniquePointer<Node>& root, const Vector2UI& size) -> UniquePointer<Node>* {
                auto newRoot   = UniquePointer<Node>(Axis::New<Node>());
                auto rightRoot = UniquePointer<Node>(Axis::New<Node>());

                rightRoot->Area = {
                    root->Area.Width,
                    0,
                    size.X,
                    root->Area.Height};

                newRoot->Used  = true;
                newRoot->Area  = {0, 0, root->Area.Width + size.X, root->Area.Height};
                newRoot->Down  = std::move(root);
                newRoot->Right = std::move(rightRoot);

                root = std::move(newRoot);

                return &root->Right;
            };

            // Grows node in downwards direction
            constexpr auto GrowDown = [](UniquePointer<Node>& root, const Vector2UI& size) -> UniquePointer<Node>* {
                auto newRoot  = UniquePointer<Node>(Axis::New<Node>());
                auto downRoot = UniquePointer<Node>(Axis::New<Node>());

                downRoot->Area = {
                    0,
                    root->Area.Height,
                    root->Area.Width,
                    size.Y};

                newRoot->Used  = true;
                newRoot->Area  = {0, 0, root->Area.Width, root->Area.Height + size.Y};
                newRoot->Down  = std::move(downRoot);
                newRoot->Right = std::move(root);

                root = std::move(newRoot);

                return &root->Down;
            };

            // Keeps square-like nodes
            const Bool canGrowDown     = sortedSpriteList[i].Second.X <= root->Area.Width;
            const Bool canGrowRight    = sortedSpriteList[i].Second.Y <= root->Area.Height;
            const Bool shouldGrowRight = canGrowRight && (root->Area.Height >= root->Area.Width + sortedSpriteList[i].Second.X);
            const Bool shouldGrowDown  = canGrowDown && (root->Area.Width >= root->Area.Height + sortedSpriteList[i].Second.Y);

            UniquePointer<Node>* grewNode = nullptr;
            if (shouldGrowRight)
                grewNode = GrowRight(root, sortedSpriteList[i].Second);
            else if (shouldGrowDown)
                grewNode = GrowDown(root, sortedSpriteList[i].Second);
            else if (canGrowRight)
                grewNode = GrowRight(root, sortedSpriteList[i].Second);
            else if (canGrowDown)
                grewNode = GrowDown(root, sortedSpriteList[i].Second);
            else
                AXIS_ASSERT(false, "Failed to grow the node!");

            // Assigns the sprite to the node
            (*grewNode)->Used = true;

            // Assigns the sprite to the result
            result.SpriteLocations.Insert({sortedSpriteList[i].First, RectangleUI((*grewNode)->Area.X, (*grewNode)->Area.Y, sortedSpriteList[i].Second.X, sortedSpriteList[i].Second.Y)});

            // Split the node
            SplitNode(*grewNode, sortedSpriteList[i].Second);
        }
    }

    result.PackedTextureSize = {(Uint32)root->Area.Width, (Uint32)root->Area.Height};

    return result;
}

// Predicate for sorting sprites by size
template <class Key>
constexpr auto Predicate = [](const Pair<Key, Vector2UI>& lhs, const Pair<Key, Vector2UI>& rhs) noexcept {
    return Math::Max(lhs.Second.X, lhs.Second.Y) > Math::Max(rhs.Second.X, rhs.Second.Y);
};

} // namespace Detail

template <class Key, class Hasher>
inline SpritePackingResult<Key, Hasher> PackSprite(List<Pair<Key, Vector2UI>>&& spriteList) noexcept
{
    // Moves the sprite list to the new one
    auto sortedSpriteList = std::move(spriteList);

    // Sorts the sprite list by the largest dimension
    std::sort(sortedSpriteList.begin(), sortedSpriteList.end(), Detail::Predicate<Key>);

    // Packs the sprites
    return Detail::PackSpriteInternal<Key, Hasher>(sortedSpriteList);
}

template <class Key, class Hasher>
inline SpritePackingResult<Key, Hasher> PackSprite(const List<Pair<Key, Vector2UI>>& spriteList) noexcept
{
    // Creates a copy of the sprite list
    auto sortedSpriteList = spriteList;

    // Sorts the sprite list by the largest dimension
    std::sort(sortedSpriteList.begin(), sortedSpriteList.end(), Detail::Predicate<Key>);

    // Packs the sprites
    return Detail::PackSpriteInternal<Key, Hasher>(sortedSpriteList);
}

} // namespace Axis

#endif // AXIS_RENDERER_PACKSPRITEIMPL_INL