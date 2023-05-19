#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
// #include <systems/mesh>
#include <systems/movement.hpp>
#include <asset-loader.hpp>
// #include <systems/energy-controller.hpp>

// This state shows how to use the ECS framework and deserialization.
class LostState : public our::State
{

    our::World world;
    our::ForwardRenderer renderer;

    int final_score = 0;
    void onInitialize() override
    {
        std::string config_path = "config/lost.jsonc";

        // Open the config file and exit if failed
        std::ifstream file_in(config_path);
        if (!file_in)
        {
            std::cerr << "Couldn't open file: " << config_path << std::endl;
            return;
        }

        // Read the file into a json object then close the file
        nlohmann::json fileConfigs = nlohmann::json::parse(file_in, nullptr, true, true);
        file_in.close();

        // First of all, we get the scene configuration from the app config
        auto &config = fileConfigs["scene"];

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
        // We initialize the mesh renderer controller system since it needs a pointer to the app
        // meshRendererController.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }


    void setScore(int s){
        this->final_score = s;
    }
    void onDraw(double deltaTime) override
    {
        auto &keyboard = getApp()->getKeyboard();

        if (keyboard.justPressed(GLFW_KEY_ENTER))
        {
            // If the space key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
        // Here, we just run a bunch of systems to control the world logic
        // movementSystem.update(&world, (float)deltaTime);
        // meshRendererController.update(&world, (float)deltaTime);
        // energySystem.update(&world, (float)deltaTime);
        //  And finally we use the renderer system to draw the scene
        renderer.render(&world);
    }

    void onDestroy() override
    {
        // Don't forget to destroy the renderer
        renderer.destroy();
        world.clear();
        // On exit, we call exit for the meshRenderer controller system to make sure that the mouse is unlocked
        // meshRendererController.exit();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();

    }
};