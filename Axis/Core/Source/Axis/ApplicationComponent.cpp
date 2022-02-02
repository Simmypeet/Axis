/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/CorePch.hpp>

#include <Axis/ApplicationComponent.hpp>

namespace Axis
{

ApplicationComponent::ApplicationComponent(Int32 updateOrder,
                                           Int32 renderOrder,
                                           Bool  isActive,
                                           Bool  isVisible) noexcept :
    _updateOrder(updateOrder),
    _renderOrder(renderOrder),
    _isActive(isActive),
    _isVisible(isVisible) {}

void ApplicationComponent::SetUpdateOrder(Int32 updateOrder)
{
    _updateOrder = updateOrder;
    _updateOrderChangedEvent.Invoke(*this, (Int32)updateOrder);
}

void ApplicationComponent::SetRenderOrder(Int32 renderOrder)
{
    _renderOrder = renderOrder;
    _renderOrderChangedEvent.Invoke(*this, (Int32)renderOrder);
}

void ApplicationComponent::SetActive(Bool isActive)
{
    _isActive = isActive;
    _activeChangedEvent.Invoke(*this, (Bool)isActive);
}

void ApplicationComponent::SetVisible(Bool isVisible)
{
    _isVisible = isVisible;
    _visibleChangedEvent.Invoke(*this, (Bool)isVisible);
}

} // namespace Axis