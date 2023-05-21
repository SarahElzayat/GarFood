#version 330

#define MAX_LIGHTS 16
#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2

//Varyings are "out" from vertex shader and "in" to fragment shader
//Recieve vertex information from the vertex shader to the fragment shader

in Varyings {
    vec4 color;
    //(texture image: uv space)
    vec2 tex_coord;
    vec3 normal;
    //vertex to eye vector in the world space
    vec3 view;
    //verteex in world space
    vec3 world;
} fs_in;

//final color of the vertex that will be drawn on the screen
out vec4 frag_color;

struct Material {
    //measure of ability to reflect light
    sampler2D albedo;
    //reflection of light from a surface in a specific direction.
    sampler2D specular;
    //to create more realistic shadows 
    sampler2D ambient_occlusion;
    sampler2D roughness;
    //materials or objects that emit their own light. 
    sampler2D emissive;
};


struct Light {
    //directional, point or spot light
    int type;
    //for spot light and point light
    vec3 position;
    //for spot light and directional light
    vec3 direction;
    //the part of light color that affects the diffuse of the material
    vec3 diffuse;
    //the part of light that affects the specular of the material
    vec3 specular;
    //the reduction in light's intensity as it travels
    vec3 attenuation; // x*d^2 + y*d + z
    //inner and outer angles
    //inside the inner angle ==> light intensity is maximum
    //outside the outer angle ==> light intensity is 0
    //in between ==> light intensity is interpolated
    vec2 cone_angles; // x: inner_angle, y: outer_angle
};

//to give the vertex ambient lighting according to its normal
//if its normal is up ==> it takes top light (sky)
//if its normal is down ==> it takes bottom light (ground)
//if its normal is in beween ==> it mix light colors beween (top, horizon) or (bottom, horizon)
struct Sky {
    vec3 top, horizon, bottom;
};


//uniforms are variables that are sent from the CPU to the GPU
//Uniform variables are used to pass data that is constant across all vertices in a draw call.
uniform Light lights[MAX_LIGHTS];
//number of light sources in the scene
uniform int light_count;
//ambient color struct
uniform Sky sky;
//The lit material
uniform Material material;

void main(){
    //normal and view of each vertex
    vec3 view = normalize(fs_in.view);
    vec3 normal = normalize(fs_in.normal);

    //get diffuse, specular, roughness, ambient, emissive of the material from their sent textures
    vec3 material_diffuse = texture(material.albedo, fs_in.tex_coord).rgb;
    vec3 material_specular = texture(material.specular, fs_in.tex_coord).rgb;
    vec3 material_ambient = material_diffuse * texture(material.ambient_occlusion, fs_in.tex_coord).r;
    vec3 material_emissive = texture(material.emissive, fs_in.tex_coord).rgb;

    float material_roughness = texture(material.roughness, fs_in.tex_coord).r;
    //compute the shininess of the material
    //This function clamps the value of "roughness" between 0.001 and 0.999. If the value of "roughness" is less than 0.001, it will be set to 0.001, and if it is greater than 0.999, it will be set to 0.999.
    //to avoid 0 and infinity
    float material_shininess = 2.0 / pow(clamp(material_roughness, 0.001, 0.999), 4.0) - 2.0;

    //compute ambient light
    vec3 ambient_light = (normal.y > 0) ?
        mix(sky.horizon, sky.top, normal.y * normal.y) :
        mix(sky.horizon, sky.bottom, normal.y * normal.y);
    //calculate the color of the vertex without taking light sources into consideration
    //it adds the ambient light effect and the emissive of the vertex itself
    frag_color = vec4(material_emissive + material_ambient * ambient_light, 1.0);
    
    //if light count > specified MAX_LIGHTS ==> clamp it to MAX_LIGHTS number
    int clamped_light_count = min(MAX_LIGHTS, light_count);
    //for each light source:
        //compute diffuse and specular amount and add them to the lighting effect of the vertex
    for(int light_idx = 0; light_idx < clamped_light_count; light_idx++){
        Light light = lights[light_idx];

        vec3 world_to_light_dir;

        float attenuation = 1;
        //world_to_light_dir in directional light is the negative of the light direction (it doesn't have position)
        if(light.type == DIRECTIONAL){
            world_to_light_dir = -light.direction;
        } else {
            //in spot and point light ==> the direction is the vector from the vertx position to the light position
            world_to_light_dir = light.position - fs_in.world;
            //get distance between light position and vertex to calculate attenuation
            float d = distance(light.position, fs_in.world);

            world_to_light_dir /= d;
            //for point light ==> attenuation depend on the distance only
            // x*d^2 + y*d + z
            attenuation /= dot(light.attenuation, vec3(d*d, d, 1));
            
            if(light.type == SPOT){
                //for spot light ==> attenuation depend on the distance and the angle
                float angle = acos(dot(-world_to_light_dir, light.direction));
                //inside the inner cone angle ==> light intensity is maximum
                //outside the outer cone angle ==> light intensity is 0
                //in between ==> light intensity is interpolated
                attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle);
            }
        }
        //caculate vertex color effects due to this light component:

        //Material diffuse refers to the way a material scatters light in all directions
        //if cos of the angle between the normal is less than 0 ==> it means there is no effect from the light to the vertex ==> consider it 0 not negative
        vec3 diffuse = light.diffuse * material_diffuse * max(0, dot(normal, world_to_light_dir));
        //the reflection of the light direction vector with respect to the surface normal vector using the "reflect" function
        vec3 reflected = reflect(-world_to_light_dir, normal);
        //Material specular is the way a material reflects light in a specific direction, causing it to appear shiny 
        //simulate shiny surfaces
        //phong 
        vec3 specular = light.specular * material_specular * pow(max(0, dot(view, reflected)), material_shininess);

        //add effect of light sources to the vertex color
        //color = emissive (if the material is lighting by itself) + ambient effect + diffuse effect+ specular effect
        frag_color.rgb += (diffuse + specular) * attenuation;
    }
}