#pragma once

#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace our {
    // There are some options in the render pipeline that we cannot control via shaders
    // such as blending, depth testing and so on
    // Since each material could require different options (e.g. transparent materials usually use blending),
    // we will encapsulate all these options into a single structure that will also be responsible for configuring OpenGL's pipeline
    struct PipelineState {
        // This set of pipeline options specifies whether face culling will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum culledFace = GL_BACK;
            GLenum frontFace = GL_CCW;
        } faceCulling;

        // This set of pipeline options specifies whether depth testing will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum function = GL_LEQUAL;
        } depthTesting;

        // This set of pipeline options specifies whether blending will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum equation = GL_FUNC_ADD;
            GLenum sourceFactor = GL_SRC_ALPHA;
            GLenum destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
            glm::vec4 constantColor = {0, 0, 0, 0};
        } blending;

        // These options specify the color and depth mask which can be used to
        // prevent the rendering/clearing from modifying certain channels of certain targets in the framebuffer
        glm::bvec4 colorMask = {true, true, true, true}; // To know how to use it, check glColorMask
        bool depthMask = true; // To know how to use it, check glDepthMask


        // This function should set the OpenGL options to the values specified by this structure
        // For example, if faceCulling.enabled is true, you should call glEnable(GL_CULL_FACE), otherwise, you should call glDisable(GL_CULL_FACE)
        void setup() const {
            //TODO: (Req 4) Write this function

            //1-FACE CULLING
            //Check if we want it enabled or not
            
            //If YES then we enable culling at first then
            //call glCullFace() to specify that the back-facing facets are culled
            //& glFrontFace() to set orientation of front-facing polygons initially set to "counter-clockwise""

            //If NO then we call glDisable() to disable culling
            if (faceCulling.enabled == true)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(faceCulling.culledFace);
                glFrontFace(faceCulling.frontFace);
            }
            else
            {
                glDisable(GL_CULL_FACE);
            }

            //2-DEPTH
            //Check if we want it enabled or not
            //If YES then we enable Depth at first then
            //call glDepthFunc() to set the depth comparison function initially set to "GL_LEQUAL" 
            //Passes if  incoming depth value is <= to stored value.

            //If NO then we call glDisable() to disable culling

            if (depthTesting.enabled == true)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(depthTesting.function);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }
            
            //3-BLENDING
            //Check if we want it enabled or not

            //If YES then we enable Depth at first then
            //call glBlendEquation() to set how source and destination colors are combined set initially to "GL_FUNC_ADD" used for antialiasing and transparency,.
            //& glBlendFunc() to define the operation of blending for all draw buffers  set initially to "src = GL_SRC_ALPHA" & "dst = GL_ONE_MINUS_SRC_ALPHA"
            // meaning that the Factor is equal to the alpha component of the source color vector and dst factor is equal to 1−alpha of the source color vector
            //& glBlendColor() to set  the source and destination blending factors ranging from [0,1]


            //If NO then we call glDisable() to disable culling


            if (blending.enabled == true)
            {
                glEnable(GL_BLEND);
                glBlendEquation(blending.equation);
                glBlendFunc(blending.sourceFactor, blending.destinationFactor);
                glBlendColor(blending.constantColor.r, blending.constantColor.g, blending.constantColor.b, blending.constantColor.a);
            }
            else
            {
                glDisable(GL_BLEND);
            }
            
            //Here to just call glColorMask() & glDepthMask() to
            glColorMask(colorMask.r, colorMask.g, colorMask.b, colorMask.a);
            glDepthMask(depthMask);
        }

        // Given a json object, this function deserializes a PipelineState structure
        void deserialize(const nlohmann::json& data);
    };

}