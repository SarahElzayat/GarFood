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
        Application *app; // The application in which the state runs
        FreeCameraControllerSystem *fccs;
        int score = 0;
        int lives = 3;

    public:
        void enter(Application *app)
        {
            this->app = app;
            score = 0;
            lives = 3;
        }

        int get_score()
        {
            return score;
        }
        int get_lives()
        {
            return lives;
        }

        // This should be called every frame to update all entities containing a CollisionComponent.
        bool update(World *world)
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
                    // std::cout << collision->getOwner()->name << '\n';
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

                // std::cout << "Collider Type: " << colliderType << '\n';

                if (colliderType == "meshmesh")
                {
                    colliderPosition = collider->getOwner()->getLocalToWorldMatrix() * glm::vec4(colliderPosition, 1.0f);
                }

                for (auto obstacle : collisionComponents)
                {
                    std::string obstacleType;
                    glm::vec3 obstaclePosition;
                    glm::vec3 obstacleScale;

                    obstacleType = obstacle->getOwner()->name;
                    obstaclePosition = obstacle->getOwner()->localTransform.position;
                    obstacleScale = obstacle->getOwner()->localTransform.scale;

                    // std::cout << "Obstacle Type: " << obstacleType << '\n';
                    if (colliderType != obstacleType)
                    {

                        if (glm::length(colliderPosition.z - obstaclePosition.z) < 5 && colliderType == "meshmesh" && obstacleType == "finish_line")
                        {
                            app->changeState("end");
                            return false;
                        }

                        else if (glm::length(colliderPosition - obstaclePosition) < 1 && colliderType == "meshmesh" && obstacleType == "fish")
                        {
                            world->markForRemoval(obstacle->getOwner());
                            score += 20;
                            return false;
                        }
                        else if ((glm::length(colliderPosition.x - obstaclePosition.x) < 0.05f || glm::length(colliderPosition.z - obstaclePosition.z) < 0.05f) && colliderType == "meshmesh" && obstacleType == "lamp")
                        {
                            return true;
                        }
                        else if (glm::length(colliderPosition.x - obstaclePosition.x) < 1   && glm::length(colliderPosition.z - obstaclePosition.z) < 1.2 && colliderType == "meshmesh" && obstacleType == "fekry")
                        {
                            world->markForRemoval(obstacle->getOwner());

                            score -= 10;
                            lives--;
                            if (lives == 0){
                                 app->changeState("lost");
                            }
                            return true;
                        }
                    }
                }
            }
            return false;
        };
    };
}
