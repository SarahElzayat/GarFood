#pragma once

#include "component.hpp"
#include "transform.hpp"
#include <list>
#include <iterator>
#include <string>
#include <glm/glm.hpp>

namespace our
{

    class World; // A forward declaration of the World Class

    class Entity
    {
        World *world;                      // This defines what world own this entity
        std::list<Component *> components; // A list of components that are owned by this entity

        friend World;       // The world is a friend since it is the only class that is allowed to instantiate an entity
        Entity() = default; // The entity constructor is private since only the world is allowed to instantiate an entity
    public:
        std::string name;         // The name of the entity. It could be useful to refer to an entity by its name
        Entity *parent;           // The parent of the entity. The transform of the entity is relative to its parent.
                                  // If parent is null, the entity is a root entity (has no parent).
        Transform localTransform; // The transform of this entity relative to its parent.

        World *getWorld() const { return world; } // Returns the world to which this entity belongs

        glm::mat4 getLocalToWorldMatrix() const;  // Computes and returns the transformation from the entities local space to the world space
        void deserialize(const nlohmann::json &); // Deserializes the entity data and components from a json object

        // This template method create a component of type T,
        // adds it to the components map and returns a pointer to it
        template <typename T>
        T *addComponent()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
            // TODO: (Req 8) Create an component of type T, set its "owner" to be this entity, then push it into the component's list

            // create a new component of generic type T as T inherits from component 
            T *component = new T();

            // set the owner of the created component to be this entity
            component->owner = this;

            // add the sreated new component to the list of components of this entity
            components.push_back(component);

            // Don't forget to return a pointer to the new component
            return component;
        }

        // This template method searhes for a component of type T and returns a pointer to it
        // If no component of type T was found, it returns a nullptr
        template <typename T>
        T *getComponent()
        {
            // TODO: (Req 8) Go through the components list and find the first component that can be dynamically cast to "T*".

            // loops on all the components in the components list of this entity
            for (auto &this_component : components)
            {
                // checks if the component can be dynamically cast to T*
                if (dynamic_cast<T *>(this_component) != nullptr)
                {
                    // returns the first component that meets this condition
                    return dynamic_cast<T *>(this_component);
                }
            }
            // returns a nullptr if no component satisfies the condition
            return nullptr;
        }

        // This template method dynami and returns a pointer to it
        // If no component of type T was found, it returns a nullptr
        template <typename T>
        T *getComponent(size_t index)
        {
            auto it = components.begin();
            std::advance(it, index);
            if (it != components.end())
                return dynamic_cast<T *>(*it);
            return nullptr;
        }

        // This template method searhes for a component of type T and deletes it
        template <typename T>
        void deleteComponent()
        {
            // TODO: (Req 8) Go through the components list and find the first component that can be dynamically cast to "T*".
            //  If found, delete the found component and remove it from the components list

            // loops on all the components in the components list of this entity
            for (auto component = components.begin(); component != components.end(); component++)
            {
                // checks if the component can be dynamically cast to T*
                if (dynamic_cast<T *>(*component) != nullptr)
                {
                    // deletes the first component that meets this condition
                    delete *component;

                    // erase the component from the entity's list
                    components.erase(component);
                    break;
                }
            }
        }

        // This template method searhes for a component of type T and deletes it
        void deleteComponent(size_t index)
        {
            auto it = components.begin();
            std::advance(it, index);
            if (it != components.end())
            {
                delete *it;
                components.erase(it);
            }
        }

        // This template method searhes for the given component and deletes it
        template <typename T>
        void deleteComponent(T const *component)
        {
            // TODO: (Req 8) Go through the components list and find the given component "component".
            //  If found, delete the found component and remove it from the components list

            // loops on all the components in the components list of this entity
            for (auto this_component = components.begin(); this_component != components.end(); this_component++)
            {
                // checks if the given compenent is equal to the current component in the list
                if (*this_component == component)
                {
                    // delete the required component pointer
                    delete *this_component;

                    // and erase it from the entity's list
                    components.erase(this_component);
                    break;
                }
            }
        }

        // Since the entity owns its components, they should be deleted alongside the entity
        ~Entity()
        {
            // TODO: (Req 8) Delete all the components in "components".

            // traverse the list of components of this entity and delete each component
            for (auto &component : components)
            {
                delete component;
            }
        }

        // Entities should not be copyable
        Entity(const Entity &) = delete;
        Entity &operator=(Entity const &) = delete;
    };

}