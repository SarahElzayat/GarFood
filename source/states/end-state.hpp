#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>

class EndState : public our::State
{
    // A meterial holding the end state shader and the end texture to draw
    our::TexturedMaterial *endMaterial;
    // A rectangle mesh on which the menu material will be drawn

    our::Mesh *rectangle;
    // A variable to record the time since the state is entered (it will be used for the fading effect).

    float time;

    int final_score = 0;
    void onInitialize() override
    {
        // First, we create a material for the end state's background

        endMaterial = new our::TexturedMaterial();
        // Here, we load the shader that will be used to draw the background
        endMaterial->shader = new our::ShaderProgram();
        endMaterial->shader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
        endMaterial->shader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
        endMaterial->shader->link();
        // Then we load the menu texture
        endMaterial->texture = our::texture_utils::loadImage("assets/textures/king.png");
        // Initially, the menu material will be black, then it will fade in
        endMaterial->tint = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

        // Then we create a rectangle whose top-left corner is at the origin and its size is 1x1.
        // Note that the texture coordinates at the origin is (0.0, 1.0) since we will use the
        // projection matrix to make the origin at the the top-left corner of the screen.
        rectangle = new our::Mesh({
                                      {{0.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                                      {{1.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                                      {{1.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                                      {{0.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                                  },
                                  {
                                      0,
                                      1,
                                      2,
                                      2,
                                      3,
                                      0,
                                  });
        // Reset the time elapsed since the state is entered.
        time = 0;
    }

    void setScore(int s)
    {
        this->final_score = s;
    }
    void onDraw(double deltaTime) override
    {
        // Get a reference to the keyboard object

        auto &keyboard = getApp()->getKeyboard();

        if (keyboard.justPressed(GLFW_KEY_ENTER))
        {
            // If the space key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
        // Get the framebuffer size to set the viewport and the create the projection matrix.
        glm::ivec2 size = getApp()->getFrameBufferSize();
        // Make sure the viewport covers the whole size of the framebuffer.
        glViewport(0, 0, size.x, size.y);

        // The view matrix is an identity (there is no camera that moves around).
        // The projection matrix applys an orthographic projection whose size is the framebuffer size in pixels
        // so that the we can define our object locations and sizes in pixels.
        // Note that the top is at 0.0 and the bottom is at the framebuffer height. This allows us to consider the top-left
        // corner of the window to be the origin which makes dealing with the mouse input easier.
        glm::mat4 VP = glm::ortho(0.0f, (float)size.x, (float)size.y, 0.0f, 1.0f, -1.0f);
        // The local to world (model) matrix of the background which is just a scaling matrix to make the menu cover the whole
        // window. Note that we defind the scale in pixels.
        glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

        // First, we apply the fading effect.
        time += (float)deltaTime;
        endMaterial->tint = glm::vec4(glm::smoothstep(0.00f, 2.00f, time));
        // Then we render the menu background
        // Notice that I don't clear the screen first, since I assume that the menu rectangle will draw over the whole
        // window anyway.
        endMaterial->setup();
        endMaterial->shader->set("transform", VP * M);
        rectangle->draw();
    }

    void onDestroy() override
    {
        // Delete all the allocated resources
        delete rectangle;
        delete endMaterial->texture;
        delete endMaterial->shader;
        delete endMaterial;
        our::clearAllAssets();

    }
};