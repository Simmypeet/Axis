/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/CorePch.hpp>

#include <Axis/ComponentCollection.hpp>

namespace Axis
{

namespace Core
{

namespace Detail
{

// Predicate for update order
constexpr auto UpdateOrderPredicate = [](ApplicationComponent& component) -> Int32 {
    return component.GetUpdateOrder();
};

// Predicate for render order
constexpr auto RenderOrderPredicate = [](ApplicationComponent& component) -> Int32 {
    return component.GetRenderOrder();
};

template <class Predicate>
inline void AppendSortedList(System::List<System::SharedPointer<ApplicationComponent>>& array,
                             ApplicationComponent*                                      component,
                             const Predicate&                                           predicate)
{
    // The array is already sorted.
    // Inserts the new element in the correct position.

    // Using binary search to find the correct position.
    // The array is sorted in ascending order.

    ApplicationComponent& componentRef = *component;

    Int32 value = predicate(componentRef);

    Size start = 0;
    Size end   = array.GetLength() - 1;

    while (start <= end)
    {
        Size middle = (start + end) / 2;

        if (value < predicate(*array[middle]))
        {
            end = middle - 1;
        }
        else
        {
            start = middle + 1;
        }
    }

    array.Emplace(start, component);
}

template <class Predicate>
inline void RemoveSortedList(System::List<System::SharedPointer<ApplicationComponent>>& array,
                             ApplicationComponent*                                      component,
                             const Predicate&                                           predicate)
{
    // The array is already sorted.
    // Removes the element from the correct position.

    // Using binary search to find the correct position.
    // The array is sorted in ascending order.

    ApplicationComponent& componentRef = *component;

    Int32 value = predicate(componentRef);

    Size start = 0;
    Size end   = array.GetLength() - 1;

    while (start <= end)
    {
        Size middle = (start + end) / 2;

        if (value < predicate(*array[middle]))
        {
            end = middle - 1;
        }
        else
        {
            start = middle + 1;
        }
    }

    array.RemoveAt(start);
}

} // namespace Detail


ComponentCollection::ComponentCollection(Application& application) noexcept :
    _application(application) {}

void ComponentCollection::Append(System::SharedPointer<ApplicationComponent>&& component)
{
    try
    {
        // Adds the component to the collection.
        _components.Append(std::move(component));

        // Invokes on attach function
        component->OnAttach(_application);

        // Adds the component to the update and render lists.
        Detail::AppendSortedList(_componentsUpdateOrder, (ApplicationComponent*)component, Detail::UpdateOrderPredicate); // Update order
        Detail::AppendSortedList(_componentsDrawOrder, (ApplicationComponent*)component, Detail::RenderOrderPredicate);   // Render order
    }
    catch (...)
    {
        // Re-throws the exception.
        throw;
    }

    _componentAddedEvent.Invoke(*component);
}

void ComponentCollection::AppendHighest(System::SharedPointer<ApplicationComponent>&& component)
{
    Int32 updateOrder = component->GetUpdateOrder();
    Int32 renderOrder = component->GetRenderOrder();

    // Updates the update and render order of the component.
    component->SetUpdateOrder(_componentsUpdateOrder[_componentsUpdateOrder.GetLength() - 1]->GetUpdateOrder() + 1);
    component->SetRenderOrder(_componentsDrawOrder[_componentsDrawOrder.GetLength() - 1]->GetRenderOrder() + 1);

    // Adds the component to the collection.
    try
    {
        Append(std::move(component));
    }
    catch (...)
    {
        // Sets the update and render order back to the previous value.
        component->SetUpdateOrder(updateOrder);
        component->SetRenderOrder(renderOrder);

        // Re-throws the exception.
        throw;
    }
}

Bool ComponentCollection::Remove(const ApplicationComponent& component)
{
    Bool found = false;

    for (Size i = 0; i < _components.GetLength(); ++i)
    {
        if (_components[i].GetPointer() == &component)
        {
            auto componentPtr = std::move(_components[i]);

            // Removes the component from the collection.
            _components.RemoveAt(i);

            // Removes the component from the update and render lists.
            Detail::RemoveSortedList(_componentsUpdateOrder, componentPtr.GetPointer(), Detail::UpdateOrderPredicate); // Update order
            Detail::RemoveSortedList(_componentsDrawOrder, componentPtr.GetPointer(), Detail::RenderOrderPredicate);   // Render order

            // Invokes on detach function
            componentPtr->OnDetach(_application);

            _componentRemovedEvent.Invoke(*componentPtr);

            found = true;
        }
    }

    return found;
}

void ComponentCollection::UpdateAll(const System::TimePeriod& timeStep)
{
    // Updates all the components in the collection.
    for (Size i = 0; i < _componentsUpdateOrder.GetLength(); ++i)
    {
        // Checks if the component is enabled.
        if (_componentsUpdateOrder[i]->IsActive())
        {
            // Updates the component.
            _componentsUpdateOrder[i]->Update(timeStep);
        }
    }
}

void ComponentCollection::RenderAll(const System::TimePeriod& timeStep)
{
    // Draws all the components in the collection.
    for (Size i = 0; i < _componentsDrawOrder.GetLength(); ++i)
    {
        // Checks if the component is enabled.
        if (_componentsDrawOrder[i]->IsVisible())
        {
            // Draws the component.
            _componentsDrawOrder[i]->Render(timeStep);
        }
    }
}

} // namespace Core

} // namespace Axis