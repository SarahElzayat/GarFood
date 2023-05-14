
#include "lighting.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include "../deserialize-utils.hpp"

namespace our
{

    // Reads light from a json object that is given from the user so we need to deserialize it
    void LightComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
        {
            return;
        }
        // read the type of the light and set the default to be directional.
        std::string type = data.value("lightType", "Directional");  

        if (type == "Directional")
        {
            lightType = LIGHT_TYPE::DIRECTIONAL;
        }
        else if (type == "Point")
        {
            lightType = LIGHT_TYPE::POINT;
        }
        else if (type == "Spot")
        {
            lightType = LIGHT_TYPE::SPOT;
        }
        
        //gets the lighting with the normal of the surface
        diffuse = glm::vec3(data.value("diffuse", glm::vec3(1, 0.9, 0.8)));

        //The shineness of the point which is basically the reflection angle of the light fallen on surface
        specular = glm::vec3(data.value("specular", glm::vec3(1, 0.9, 0.8)));

        //Calculates a percentage of the original light that is used to color a pixel
        attenuation = glm::vec3(data.value("attenuation", glm::vec3(1, 0, 0)));

        //Direction for Spot & Directional
        direction =  glm::normalize(glm::vec3(data.value("direction", glm::vec3(0, -1, 0))));

        //Calculate angles of cone in case of spot light type
        coneAngles.x = glm::radians((float)data.value("cone_angles.inner",90));
        coneAngles.y = glm::radians((float)data.value("cone_angles.outer",120));
    }

}