#include "camera.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace our
{
    // Reads camera parameters from the given json object
    void CameraComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        std::string cameraTypeStr = data.value("cameraType", "perspective");
        if (cameraTypeStr == "orthographic")
        {
            cameraType = CameraType::ORTHOGRAPHIC;
        }
        else
        {
            cameraType = CameraType::PERSPECTIVE;
        }
        near = data.value("near", 0.01f);
        far = data.value("far", 100.0f);
        fovY = data.value("fovY", 90.0f) * (glm::pi<float>() / 180);
        orthoHeight = data.value("orthoHeight", 1.0f);
    }

    // Creates and returns the camera view matrix
    glm::mat4 CameraComponent::getViewMatrix() const
    {
        /// first, we need to find the owner entity of the camera
        Entity *owner = getOwner();

        /// to determine the camera position, the owner's position must be specified
        /// so, we find the transformation matrix from the owner's local space to the world space
        glm::mat4 M = owner->getLocalToWorldMatrix();
        // TODO: (Req 8) Complete this function
        // HINT:
        //  In the camera space:
        //  - eye is the origin (0,0,0)
        //  - center is any point on the line of sight. So center can be any point (0,0,z) where z < 0. For simplicity, we let center be (0,0,-1)
        //  - up is the direction (0,1,0)
        //  but to use glm::lookAt, we need eye, center and up in the world state.
        //  Since M (see above) transforms from the camera to thw world space, you can use M to compute:
        //  - the eye position which is the point (0,0,0) but after being transformed by M
        //  - the center position which is the point (0,0,-1) but after being transformed by M
        //  - the up direction which is the vector (0,1,0) but after being transformed by M
        //  then you can use glm::lookAt

        /// transform each eye position, center position and upDirection to the the owner's world location
        /// by multiplying each with the owner's transformatioon matrix
        /// since the eye position and the center position are points, a 4th dimension is
        /// added and set 1 to represent them in the homogenuos coordinates
        /// as for the up direction, it's a vector. therefore it's set to 0
        glm::vec3 eyePosition = M * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 centerPosition = M * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
        glm::vec3 upDirection = M * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

        /// the lookAt function computes the view transformation matrix  to transform the vertices of
        /// 3D objects in the scene, so that they appear correctly in the camera view.
        return glm::lookAt(eyePosition, centerPosition, upDirection);
    }

    // Creates and returns the camera projection matrix
    // "viewportSize" is used to compute the aspect ratio
    glm::mat4 CameraComponent::getProjectionMatrix(glm::ivec2 viewportSize) const
    {
        // TODO: (Req 8) Wrtie this function
        //  NOTE: The function glm::ortho can be used to create the orthographic projection matrix
        //  It takes left, right, bottom, top. Bottom is -orthoHeight/2 and Top is orthoHeight/2.
        //  Left and Right are the same but after being multiplied by the aspect ratio
        //  For the perspective camera, you can use glm::perspective

        /// to calculate the projection matrix that represents the transformation from the camera space to the
        /// canonical view volume
        float aspectRatio = (float)viewportSize.x / viewportSize.y;
        float top = orthoHeight / 2.0f;
        float right = top * aspectRatio;

        if (cameraType == CameraType::ORTHOGRAPHIC)
            return glm::ortho(-right, right, -top, top, near, far);
        else
            /// fovY is the field of view angle of the camera if it is a perspective camera
            return glm::perspective(fovY, aspectRatio, near, far);
    }
}