#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"

namespace our
{

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            // TODO: (Req 10) Pick the correct pipeline state to draw the sky
            //  Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = GL_TRUE;
            skyPipelineState.depthTesting.function = GL_LEQUAL;
            skyPipelineState.faceCulling.enabled = GL_TRUE;
            skyPipelineState.faceCulling.frontFace = GL_CW;
            skyPipelineState.faceCulling.culledFace = GL_BACK;
            // skyPipelineState.blending.enabled = GL_TRUE;
            // skyPipelineState.blending.equation = GL_FUNC_ADD;

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer

            // create a framebuffer using the postprocessFramebuffer object in this class
            glGenFramebuffers(1, &postprocessFrameBuffer);

            // bind the framebuffer just created to be drawn on
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);

            // TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).

            // create a color texture
            colorTarget = new Texture2D();

            // attach the color texture to the framebuffer
            colorTarget->bind();

            int width = windowSize.x, height = windowSize.y;

            // returns the bit mip levels to draw on -> simply returns the maximum of the width and height of the window size
            GLsizei levels = (GLsizei)glm::floor(glm::log2(glm::max<float>((float)width, (float)height))) + 1;

            // allocate memory for the color texture
            // the color format is 8 bits for each channel (red , green, blue, alpha) as stated above
            glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8, width, height);

            // attach the 2D texture to the currently bound frambuffer color attachment
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);

            // create a color texture
            depthTarget = new Texture2D();

            // attach the depth texture to the framebuffer
            depthTarget->bind();

            // allocate memory for the depth texture
            // the depth component is 24 bits as stated above
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height);

            // attach the 2D texture to the currently bound frambuffer depth attachment
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);

            // TODO: (Req 11) Unbind the framebuffer just to be safe
            // unbind the postprocess frambuffer after finishing to return to the default frambuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        lightings.clear();
        
        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
            // Add lights to a list
            if (auto light = entity->getComponent<LightComponent>(); light)
            {
                lightings.push_back(light);
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one

        /// for the camera to be pointing to forward direction, the transformation matrix of the camera is multiplied by
        /// a vector pointing towards that direction
        /// the forward direction is represented by 0 in x direction, 0 in y direction, -1 in z direction
        /// 0 is added to the vector to be used in the homogenous coordinates
        glm::vec4 foroward_direction = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
        glm::vec3 cameraForward = camera->getOwner()->getLocalToWorldMatrix() * foroward_direction;

        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
            //TODO: (Req 9) Finish this function
            // HINT: the following return should return true "first" should be drawn before "second". 

            /// the dot product of the command center and the camera forward will allow determining which has the largest depth value
            /// commands with larger depth values will be drawn first
            return glm::dot(cameraForward, first.center) > glm::dot(cameraForward, second.center); });

        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP

        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();

        // TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize

        /// starting from the bottom left corner 0,0 till window size in both directions
        glViewport(0, 0, windowSize.x, windowSize.y);

        // TODO: (Req 9) Set the clear color to black and the clear depth to 1
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);

        // TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            // TODO: (Req 11) bind the framebuffer
            // if the postprocess material is not null
            // bind the postprocess framebuffer ta draw on it
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }

        // TODO: (Req 9) Clear the color and depth buffers

        /// clear the color and depth buffers by setting their corresponding bits
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: (Req 9) Draw all the opaque commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (RenderCommand command : opaqueCommands)
        {
            /// to draw the command, first the material must be setup
            /// the shader transformation matrix must be set for each command
            /// the VP matrix is common for all since it's camera related, changing the camera view or/and position will change that
            /// each command has a local-to-world transformation matrix, multiplying that by the VP matix allows determining the final transformation matrix
            /// the last step is actually drawing the respective mesh of the commands
            command.material->setup();


             if (auto lightingMaterial = dynamic_cast<LightMaterial *>(command.material); lightingMaterial)
            {
                lightingMaterial->shader->set("VP", VP);
                lightingMaterial->shader->set("M", command.localToWorld);
                lightingMaterial->shader->set("M_IT", glm::transpose(glm::inverse(command.localToWorld)));
                command.material->shader->set("camera_position", glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0,1)));

                //Send the light and its data to the fragement shaders
                lightingMaterial->shader->set("light_count", (int)lightings.size());
                lightingMaterial->shader->set("sky.top", glm::vec3(0.7, 0.3, 0.8));
                lightingMaterial->shader->set("sky.middle", glm::vec3(0.7, 0.3, 0.8));
                lightingMaterial->shader->set("sky.bottom", glm::vec3(0.7, 0.3, 0.8));

                //loop on the lightings list and set each one of them sending its data to the fragement shader
                for (unsigned i = 0; i < lightings.size(); i++)
                {
                    glm::vec3 lightPosition = lightings[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                    glm::vec3 lightDirection = lightings[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, -1, 0);


                    std::string lightName = "lights[" + std::to_string(i) + "]";
                    lightingMaterial->shader->set(lightName + ".type", (GLint)lightings[i]->lightType);
                    lightingMaterial->shader->set(lightName + ".diffuse", lightings[i]->diffuse);
                    lightingMaterial->shader->set(lightName + ".specular", lightings[i]->specular);
                    lightingMaterial->shader->set(lightName + ".attenuation", lightings[i]->attenuation);

                    if (lightings[i]->lightType == LIGHT_TYPE::DIRECTIONAL)
                    {
                        lightingMaterial->shader->set(lightName + ".direction", lightDirection);
                    }
                    else if (lightings[i]->lightType == LIGHT_TYPE::POINT)
                    {
                        lightingMaterial->shader->set(lightName + ".position", lightPosition);
                    }
                    else if(lightings[i]->lightType == LIGHT_TYPE::SPOT)
                    {
                        lightingMaterial->shader->set(lightName + ".position", lightPosition);
                        lightingMaterial->shader->set(lightName + ".direction", lightDirection);
                        lightingMaterial->shader->set(lightName + ".cone_angles", lightings[i]->coneAngles);
                    }
                    
                }
            }
            else
            {
                command.material->shader->set("transform", VP * command.localToWorld);
            }
            command.mesh->draw();
        }

        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {

            // TODO: (Req 10) setup the sky material
            this->skyMaterial->setup();

            // TODO: (Req 10) Get the camera position
            glm::vec3 cameraPosition = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1.0);

            // TODO: (Req 10) Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)
            glm::mat4 identity(1.0f);
            glm::mat4 M = glm::translate(identity, cameraPosition); // translating shpere position to camera position

            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?

            /// same steps as the drawing transparent commands above, only the extra projection matrix to put the sky in the back
            /// is added, the values are of the matrix are put such that the x and y are components are preserved and the z component is set to 1
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f);

            // TODO: (Req 10) set the "transform" uniform
            glm::mat4 skyTransform = alwaysBehindTransform * VP * M; // trasforming sky to depth = 1
            this->skyMaterial->shader->set("transform", skyTransform);
            // TODO: (Req 10) draw the sky sphere
            this->skySphere->draw();
        }
        // TODO: (Req 9) Draw all the transparent commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (RenderCommand command : transparentCommands)
        {
            /// to draw the command, first the material must be setup
            /// the shader transformation matrix must be set for each command
            /// the VP matrix is common for all since it's camera related, changing the camera view or/and position will change that
            /// each command has a local-to-world transformation matrix, multiplying that by the VP matix allows determining the final transformation matrix
            /// the last step is actually drawing the respective mesh of the commands

            command.material->setup();

            if (auto lightingMaterial = dynamic_cast<LightMaterial *>(command.material); lightingMaterial)
            {
                lightingMaterial->shader->set("VP", VP);
                lightingMaterial->shader->set("M", command.localToWorld);
                lightingMaterial->shader->set("M_IT", glm::transpose(glm::inverse(command.localToWorld)));
                command.material->shader->set("camera_position", glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0,1)));
                 

                // send the lights count and other data (pos, direc , ..) to the fragement shader
                lightingMaterial->shader->set("light_count", (int)lightings.size());

                lightingMaterial->shader->set("sky.top", glm::vec3(0.7, 0.3, 0.8));
                lightingMaterial->shader->set("sky.middle", glm::vec3(0.7, 0.3, 0.8));
                lightingMaterial->shader->set("sky.bottom", glm::vec3(0.7, 0.3, 0.8));

                for (unsigned i = 0; i < lightings.size(); i++)
                {
                    glm::vec3 lightPosition = lightings[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                    glm::vec3 lightDirection = lightings[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, -1, 0);

                    std::string lightName = "lights[" + std::to_string(i) + "]";

                    lightingMaterial->shader->set(lightName + ".type", (GLint)lightings[i]->lightType);
                    lightingMaterial->shader->set(lightName + ".diffuse", lightings[i]->diffuse);
                    lightingMaterial->shader->set(lightName + ".specular", lightings[i]->specular);
                    lightingMaterial->shader->set(lightName + ".attenuation", lightings[i]->attenuation);


                     if (lightings[i]->lightType == LIGHT_TYPE::DIRECTIONAL)
                    {
                        lightingMaterial->shader->set(lightName + ".direction", lightDirection);
                    }
                    else if (lightings[i]->lightType == LIGHT_TYPE::POINT)
                    {
                        lightingMaterial->shader->set(lightName + ".position", lightPosition);
                    }
                    else if(lightings[i]->lightType == LIGHT_TYPE::SPOT)
                    {
                        lightingMaterial->shader->set(lightName + ".position", lightPosition);
                        lightingMaterial->shader->set(lightName + ".direction", lightDirection);
                        lightingMaterial->shader->set(lightName + ".cone_angles", lightings[i]->coneAngles);
                    }
                }
            }
            else
            {
                command.material->shader->set("transform", VP * command.localToWorld);
            }
            
            command.mesh->draw();
        }

        // If there is a postprocess material, apply postprocessing
        if (postprocessMaterial)
        {
            // TODO: (Req 11) Return to the default framebuffer
            // unbind the postprocess framebuffer to return to the default framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            // TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle

            // if postprocess material in not null
            // setup the postprocess material
            // this calls the setup of the textured material which sets uo the pipekine state, shader to use, tint,
            // and other shader parameters to be used in the material
            postprocessMaterial->setup();

            // now we draw the triangle using the vertices in the post process vertex array
            // first bind the post process vertex array to draw the traingle
            glBindVertexArray(postProcessVertexArray);

            // use glDrawArrays to draw the triangle
            // first by specifying the mode of what we're drawing which is GL_TRIANGLES
            // then by specifying the the fisrt vertex to be rendered in the bound vertex array
            // lastly by specifying the number of vertices to be rendered
            // this means that this function begins from the index 0 and draws using the first three vertices
            // which draws a single triangle
            glDrawArrays(GL_TRIANGLES, 0, 3);

            // unbind the vertex array after it is done rendering
            glBindVertexArray(0);
        }
    }

}