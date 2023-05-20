#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/collision.hpp>
#include <asset-loader.hpp>
#include <imgui.h>
#include <string>

// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State
{

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CollisionSystem collisionSystem;

    int score = 5;
    
    // variable to wait for a certain time if the object collided before disabling the postprocess effect
    int waitFor = 0;

    void onInitialize() override
    {
        // First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets"))
        {
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if (config.contains("world"))
        {
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        collisionSystem.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }

    void onDraw(double deltaTime) override
    {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);
        bool collided = collisionSystem.update(&world);

        // And finally we use the renderer system to draw the scene
        world.deleteMarkedEntities();

        if(collided == true)
        {
            renderer.postprocessEffect = true;
            collided = false;
            waitFor = 0;
        }

        if(waitFor == 100 && renderer.postprocessEffect == true)
        {
            renderer.postprocessEffect = false;
            waitFor = 0;
        }
        else
        {
            waitFor++; 
        }

        renderer.render(&world);

        // Get a reference to the keyboard object
        auto &keyboard = getApp()->getKeyboard();

        if (keyboard.justPressed(GLFW_KEY_ESCAPE))
        {
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
    }


    void onImmediateGui() override
    {

        // start gui
        ImGui::Begin("Score", (bool *)false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration);
        // set window position
        ImGui::SetWindowPos(ImVec2(0, 10));
        // set window size
        ImGui::SetWindowSize(ImVec2(600, 100));
        // set font
        ImGui::SetWindowFontScale(3.0f);
        
        // initialize score
        std::string score_screen = "Score: " + std::to_string(collisionSystem.get_score());
        std::string lives_screen = "Lives: " + std::to_string(collisionSystem.get_lives());
        // initialize color
        ImGui::TextColored(ImVec4(1.0f, 1.0, 1.0f, 1.0f), score_screen.c_str());
        ImGui::TextColored(ImVec4(1.0f, 1.0, 1.0f, 1.0f), lives_screen.c_str());
        // end gui
        ImGui::End();
    }

    void onDestroy() override
    {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }

        //     void imgui()
        // { //= [OLD]: Will be called every frame in application.cpp to provide a slider for camera position and rotation (use camera component to get its own entity = camera)
        //   //= A transparent ImGui window with no artifacts for score on top right.
        //     ImGuiWindowFlags window_flags = 0;
        //     window_flags |= ImGuiWindowFlags_NoBackground;
 

};