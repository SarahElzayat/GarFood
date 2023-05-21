#pragma once

#include "../ecs/component.hpp"
#include "../shader/shader.hpp"
#include <glm/glm.hpp>

namespace our
{
    //Definig the 3 types of lights we have
    enum class LIGHT_TYPE
   {
        DIRECTIONAL, POINT, SPOT
   };

   class LightComponent : public Component {
    public:
        
        LIGHT_TYPE lightType;

        //Define the properties of light
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f); //Direction of light
        glm::vec3 attenuation = glm::vec3(0.0f, 0.0f, 0.0f); //Intensity of light
        glm::vec3 diffuse = glm::vec3(0, 0, 0); //Light reflection that comes from a particular point source 
        glm::vec3 specular = glm::vec3(0, 0, 0); //Shininess of the light (bright part)
        glm::vec2 coneAngles = glm::vec2(0.0f, 0.0f); //Defined for Spot light type 
        
        
        static std::string getID() { return "Light"; }

        //Read Light data from the given json object
        void deserialize(const nlohmann::json& data) override;
    };
}

