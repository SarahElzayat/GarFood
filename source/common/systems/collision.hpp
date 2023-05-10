#pragma once

#include "../ecs/world.hpp"
#include "../components/collision.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>
#include <iostream>

namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic.
    // For more information, see "common/components/collision.hpp"
    class CollisionSystem
    {
    public:
        // This should be called every frame to update all entities containing a CollisionComponent.
        void update(World *world)
        {

            // printf("UPDATE COLLISION COMPONENT");

            std::vector<CollisionComponent *> collisionComponents;
            // For each entity in the world
            for (auto entity : world->getEntities())
            {
                // Get the movement component if it exists
                CollisionComponent *collision = entity->getComponent<CollisionComponent>();
                // If the movement component exists
                if (collision)
                {
                    collisionComponents.emplace_back(collision);
                    std::cout << collision->getOwner()->name << '\n';
                }
            }

            for (auto collider : collisionComponents)
            {

                std::string colliderType;
                glm::vec3 colliderPosition;
                glm::vec3 colliderScale;

                colliderType = collider->getOwner()->name;
                colliderPosition = collider->getOwner()->localTransform.position;
                colliderScale = collider->getOwner()->localTransform.scale;

                std::cout << "Collider Type: " << colliderType << '\n';

                if (colliderType == "meshmesh")
                {
                    colliderPosition = collider->getOwner()->getLocalToWorldMatrix() * glm::vec4(colliderPosition, 1.0f);
                }

                glm::vec3 maxColliderReach = colliderPosition + colliderScale;
                glm::vec3 minColliderReach = colliderPosition - colliderScale;

                for (auto obstacle : collisionComponents)
                {
                    std::string obstacleType;
                    glm::vec3 obstaclePosition;
                    glm::vec3 obstacleScale;

                    obstacleType = obstacle->getOwner()->name;
                    obstaclePosition = obstacle->getOwner()->localTransform.position;
                    obstacleScale = obstacle->getOwner()->localTransform.scale;

                    std::cout << "Obstacle Type: " << obstacleType << '\n';

                    glm::vec3 maxObstacleReach = obstaclePosition + obstacleScale;
                    glm::vec3 minObstacleReach = obstaclePosition - obstacleScale;

                    if (colliderType != obstacleType)
                    {

                        // std::cout << "Different Colliders \n";
                        // std::cout << "Max Collider Reach X: " << maxColliderReach.x << '\n';
                        // std::cout << "Max Collider Reach y: " << maxColliderReach.y << '\n';
                        // std::cout << "Max Collider Reach Z: " << maxColliderReach.z << '\n';
                        // std::cout << "Max Obstacle Reach X: " << maxObstacleReach.x << '\n';
                        // std::cout << "Max Obstacle Reach y: " << maxObstacleReach.y << '\n';
                        // std::cout << "Max Obstacle Reach Z: " << maxObstacleReach.z << '\n';
                        // std::cout << "Min Collider Reach X: " << minColliderReach.x << '\n';
                        // std::cout << "Min Collider Reach y: " << minColliderReach.y << '\n';
                        // std::cout << "Min Collider Reach Z: " << minColliderReach.z << '\n';
                        // std::cout << "Min Obstacle Reach X: " << minObstacleReach.x << '\n';
                        // std::cout << "Min Obstacle Reach y: " << minObstacleReach.y << '\n';
                        // std::cout << "Min Obstacle Reach Z: " << minObstacleReach.z << '\n';

                        // if (maxColliderReach.x >= minObstacleReach.x && minColliderReach.x <= maxObstacleReach.x &&
                        //     maxColliderReach.y >= minObstacleReach.y && minColliderReach.y <= maxObstacleReach.y &&
                        // maxColliderReach.z >= minObstacleReach.z && minColliderReach.z <= maxObstacleReach.z)

                        if (glm::length(colliderPosition - obstaclePosition) < 2)
                        {

                            std::cout << "Dakhalna f ba3d\n";

                            // if (colliderType == "meshmesh" && obstacleType == "fekry"
                            if (colliderType == "meshmesh" && obstacleType == "fish")
                            {
                                // collider->getOwner()->localTransform.scale += glm::vec3(0.02, 0, 0);
                                world->markForRemoval(obstacle->getOwner());
                                return;
                            }
                        }
                    }
                }
            }
        };
    };
}
