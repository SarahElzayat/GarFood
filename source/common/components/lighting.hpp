#pragma once

#include "../ecs/component.hpp"
#include "../shader/shader.hpp"
#include <glm/glm.hpp>

namespace our
{
    enum class LIGHT_TYPE
   {
        //Here we define types of lights which are 3
        DIRECTIONAL, POINT, SPOT
   };

   class LightComponent : public Component {
    public:
        
        LIGHT_TYPE lightType;

        glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f); 
        glm::vec3 attenuation = glm::vec3(0.0f, 0.0f, 0.0f); 
        glm::vec3 diffuse = glm::vec3(0, 0, 0); 
        glm::vec3 specular = glm::vec3(0, 0, 0);
        glm::vec2 coneAngles = glm::vec2(0.0f, 0.0f);  
        
        
        static std::string getID() { return "Light"; }

        // Reads Light data from the given json object
        void deserialize(const nlohmann::json& data) override;
    };
}

