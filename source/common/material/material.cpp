#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our
{

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const
    {
        // TODO: (Req 7) Write this function
        // setup the pipeline state
        // this function enables the required parameters to be true
        pipelineState.setup();

        // set the shader program to be used
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        if (data.contains("pipelineState"))
        {
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint
    void TintedMaterial::setup() const
    {
        // TODO: (Req 7) Write this function

        // call the setup of the parent Material;
        // which sets up the pipeline state 
        // and sets the shader program to be used
        Material::setup(); 

        // if the material is TintedMaterial in addition to setting the shader to be used we need to set the tint variable 
        // set the uniform "tint" in (shader) to the variable tint
        shader->set("tint", tint); 
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex"
    void TexturedMaterial::setup() const
    {
        // TODO: (Req 7) Write this function
        // call setup of parent TintedMaterial
        // to setup the pipeline, the shader to use, and the tint
        // as the textured material also requies the tint to be set
        TintedMaterial::setup(); 

        // sets the alpha threshold
        // set the uniform "alphaThreshold" in (shader) to the variable alphaTreshold
        this->shader->set("alphaThreshold", alphaThreshold); 
        
        glActiveTexture(GL_TEXTURE0);
        // binds the texture to unit 0
        this->texture->bind();      
        
        // binds sampler to same texture unit 0 if it exists
        if(sampler != nullptr)
            this->sampler->bind(0);                              
        
        // send unit number to shader with uniform variable "tex"
        this->shader->set("tex", 0);                         
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json &data)
    {
        TintedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    // setup of the lightMaterial to create the needed textures based on the type
    void LightMaterial::setup() const
    {
        // call the setup of the parent Material;
        // which sets up the pipeline state 
        // and sets the shader program to be used
        Material::setup();

        if (albedo != nullptr)
        {
            //select an active texture unit -> 0
            glActiveTexture(GL_TEXTURE0);
            //bind the texture to unit 0
            albedo->bind();
            //bind the sampler to unit 0
            sampler->bind(0);
            // send unit number to shader with uniform variable "albedo"
            shader->set("material.albedo", 0);
        }
        if (specular != nullptr)
        {
            //select an active texture unit -> 1
            glActiveTexture(GL_TEXTURE1);
            //bind the texture to unit 1
            specular->bind();
            //bind the sampler to unit 1
            sampler->bind(1);
            // send unit number to shader with uniform variable "specular"
            shader->set("material.specular", 1);
        }

        if (emissive != nullptr)
        {
            //select an active texture unit -> 2
            glActiveTexture(GL_TEXTURE2);
            //bind the texture to unit 2
            emissive->bind();
            //bind the sampler to unit 2
            sampler->bind(2);
            // send unit number to shader with uniform variable "emissive"
            shader->set("material.emissive", 2);
        }

        if (roughness != nullptr)
        {
            //select an active texture unit -> 3
            glActiveTexture(GL_TEXTURE3);
            //bind the texture to unit 3
            //texture->bind();
            roughness->bind();
            //bind the sampler to unit 3
            sampler->bind(3);
            // send unit number to shader with uniform variable "roughness"
            shader->set("material.roughness", 3);
        }

        if (ambient_occlusion != nullptr)
        {
            //select an active texture unit -> 4
            glActiveTexture(GL_TEXTURE4);
            //bind the texture to unit 4
            ambient_occlusion->bind();
            //bind the sampler to unit 4
            sampler->bind(4);
            // send unit number to shader with uniform variable "ambient_occlusion"
            shader->set("material.ambient_occlusion", 4);
        }
        
    }

    //Deserialize LightMaterial data from the json file
    void LightMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
        albedo = AssetLoader<Texture2D>::get(data.value("albedo", ""));
        specular = AssetLoader<Texture2D>::get(data.value("specular", ""));
        emissive = AssetLoader<Texture2D>::get(data.value("emissive", ""));
        roughness = AssetLoader<Texture2D>::get(data.value("roughness", ""));
        ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion", ""));
    }

}