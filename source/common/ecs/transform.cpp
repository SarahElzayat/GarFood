#include "entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our
{

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const
    {
        // TODO: (Req 3) Write this function
        // create the translation matrix with the position to translate to
        // multiplies the identity matrix to the position given
        // returns a 4x4 matrix that represents the translation required of this transform
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), this->position);                            
        
        // create the rotation matrix with the euler angles to rotate pixel
        // converts from euler angles to yaw (rotation around y-axis), pitch (rotation around x-axis), roll (rotation around z-axis)
        // returns a 4x4 matrix that represents the required rotation of this transform
        glm::mat4 rotate = glm::yawPitchRoll((this->rotation).y, (this->rotation).x, (this->rotation).z);

        // create the scale matrix with the scale given in class
        // this represents the scaling part of this transform
        // apply scaling to the identity matrix passed to it using the scale given (data member) and returns the 4x4 scaling matrix
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), this->scale);
                                       
       // returns the multiplication of all the transformations in the correct order TRS
        return (translate * rotate * scale);
    }

    // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json &data)
    {
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale = data.value("scale", scale);
    }

}