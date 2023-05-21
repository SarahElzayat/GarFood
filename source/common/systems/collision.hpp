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

    // The collision system is responsible for checking every entity which contains a CollisionComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic.
    // For more information, see "common/components/collision.hpp"
    class CollisionSystem
    {
        Application *app; // The application in which the state runs
        FreeCameraControllerSystem *fccs;
        // The player's score
        int score = 0;

        // The player's lives
        int lives = 3;

    public:
        void enter(Application *app)
        {
            this->app = app;
            score = 0;
            lives = 3;
        }

        // This function returns the score to be updated every time meshmesh collides
        int get_score()
        {
            return score;
        }

        // This function returns the score to be updated every time meshmesh collides with a dog or a lamp
        int get_lives()
        {
            return lives;
        }

        // This should be called every frame to update all entities containing a CollisionComponent.
        // The function returns a boolean to indicate the type of entity meshmesh collided with 
        // If meshmesh collided with a dog or a lamp the function returns true 
        // Otherwise it returns false
        bool update(World *world)
        {

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
                }
            }

            // Traverse all the objects that contain a collision component
            for (auto collider : collisionComponents)
            {
                // First we get the position, type and scale of the collider
                std::string colliderType;
                glm::vec3 colliderPosition;
                glm::vec3 colliderScale;

                colliderType = collider->getOwner()->name;
                colliderPosition = collider->getOwner()->localTransform.position;
                colliderScale = collider->getOwner()->localTransform.scale;

                // Check if the collider is meshmesh then we check for the obstacle type
                if (colliderType == "meshmesh")
                {
                    colliderPosition = collider->getOwner()->getLocalToWorldMatrix() * glm::vec4(colliderPosition, 1.0f);
                }

                // Travers the objects for the collision components once more to check for the obstacle
                for (auto obstacle : collisionComponents)
                {
                    // Get the type, position and scale of the obstacle
                    std::string obstacleType;
                    glm::vec3 obstaclePosition;
                    glm::vec3 obstacleScale;

                    obstacleType = obstacle->getOwner()->name;
                    obstaclePosition = obstacle->getOwner()->localTransform.position;
                    obstacleScale = obstacle->getOwner()->localTransform.scale;

                    // If the two objects are not the same type chaeck for collision
                    if (colliderType != obstacleType)
                    {

                        // Check if any of the two objects is in the vacinity of the other then collision occurred
                        // Meshmesh reached the finidh line -> the player won the game
                        if (glm::length(colliderPosition.z - obstaclePosition.z) < 5 && colliderType == "meshmesh" && obstacleType == "finish_line")
                        {
                            app->changeState("end");
                            return false;
                        }

                        // Meshmesh collided with a fish -> the player gains 20 points
                        else if (glm::length(colliderPosition - obstaclePosition) < 1 && colliderType == "meshmesh" && obstacleType == "fish")
                        {
                            world->markForRemoval(obstacle->getOwner());    // mark the fish for removal so that it can be deleted when rendering the next frame
                            score += 20;    // Increase the player's score by 20 points
                            return false;
                        }

                        // Meshmesh collided with a lamp -> results in a 10-point reduction in their score
                        else if ((glm::length(colliderPosition.x - obstaclePosition.x) < 0.25 && glm::length(colliderPosition.z - obstaclePosition.z) < 0.1) && colliderType == "meshmesh" && obstacleType == "lamp")
                        {
                            if (score >= 10)
                            {
                                score -= 10;
                            }

                            return true;
                        }

                        // Meshmesh collided with a dog -> the player loses a life + a 10-point reduction in their score
                        else if (glm::length(colliderPosition.x - obstaclePosition.x) < 1 && glm::length(colliderPosition.z - obstaclePosition.z) < 2 && colliderType == "meshmesh" && obstacleType == "fekry")
                        {
                            // remove the dog
                            world->markForRemoval(obstacle->getOwner());

                            if (score >= 10)
                            {
                                score -= 10;
                            }
                            lives--;
                            if (lives == 0)
                            {
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
