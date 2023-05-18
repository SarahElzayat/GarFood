#pragma once

#include "../ecs/world.hpp"
#include "../components/score.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>


namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/movement.hpp"
    enum class ScoreChange
    {
        INCREASE,
        DECREASE
    };
    class ScoreSystem {  
    public:

        // This should be called every frame to update all entities containing a MovementComponent. 
        void update(World* world, float deltaTime) {

            // For each entity in the world
            for(auto entity : world->getEntities()){
                // Get the score component if it exists
                ScoreComponent* scoreComponent = entity->getComponent<ScoreComponent>();
                // If the score component exists
                if(scoreComponent){
                    // DO SOMETHING
                }
            }
        }

        void destroy(){

        };

    };

}
