#version 330

#define DIRECTIONAL 0
#define POINT       1
#define SPOT        2
#define MAX_LIGHTS 8

//Varyings are "out" from vertex shader and "in" to fragment shader
//Recieve vertex information from the vertex shader to the fragment shader
in Varyings {
    vec3 normal;
    vec4 color;
    //(texture image: uv space)
    vec2 tex_coord;
    //vertex to eye vector in the world space
    vec3 view;
    //verteex in world space
    vec3 world;
} fs_in;

//final color of the vertex that will be drawn on the screen
out vec4 frag_color;

//uniforms are variables that are sent from the CPU to the GPU
//Uniform variables are used to pass data that is constant across all vertices in a draw call.
uniform Light lights[MAX_LIGHTS];
uniform int light_count;
uniform Sky sky;
uniform Material material;

struct Light {
    //directional, point or spot light
    int type;
    //for spot light and point light
    vec3 position;
    //for spot light and directional light
    vec3 direction;
    vec3 diffuse;
    vec3 specular; 
    //the reduction in light's intensity as it travels
    vec3 attenuation; // x*d^2 + y*d + z
    //inner and outer angles
    //inside the inner angle ==> light intensity is maximum
    //outside the outer angle ==> light intensity is 0
    //in between ==> light intensity is interpolated
    vec2 cone_angles; 
};

struct Material {
    //measure of ability to reflect light
    sampler2D albedo;
    //reflection of light from a surface in a specific direction.
    sampler2D specular;
    sampler2D roughness;
    //to create more realistic shadows 
    sampler2D ambient_occlusion;
    //materials or objects that emit their own light. 
    sampler2D emissive;
};

//to give the vertex ambient lighting according to its normal
//if its normal is up ==> it takes top light (sky)
//if its normal is down ==> it takes bottom light (ground)
//if its normal is in beween ==> it mix light colors beween (top, horizon) or (bottom, horizon)
struct Sky {
    vec3 top, horizon, bottom;
};

vec3 compute_sky_light(vec3 normal){
    vec3 extreme = normal.y > 0 ? sky.top : sky.bottom;
    return mix(sky.horizon, extreme, normal.y * normal.y);
}

//for diffuse
float lambert(vec3 normal, vec3 world_to_light_direction) {
    return max(0.0, dot(normal, world_to_light_direction));
}

//for specular
float phong(vec3 reflected, vec3 view, float shininess) {
    return pow(max(0.0, dot(reflected, view)), shininess);
}

void main() {
    //normal and view of each vertex
    vec3 normal = normalize(fs_in.normal);
    vec3 view = normalize(fs_in.view);

    //get diffuse, specular, roughness, ambient, emissive of the material from their sent textures
    vec3 diffuse = texture(material.albedo, fs_in.tex_coord).rgb;
    vec3 specular = texture(material.specular, fs_in.tex_coord).rgb;
    float roughness = texture(material.roughness, fs_in.tex_coord).r;
    vec3 ambient = diffuse * texture(material.ambient_occlusion, fs_in.tex_coord).r;
    vec3 emissive = texture(material.emissive, fs_in.tex_coord).rgb;
    //compute the shininess of the material
    //This function clamps the value of "roughness" between 0.001 and 0.999. If the value of "roughness" is less than 0.001, it will be set to 0.001, and if it is greater than 0.999, it will be set to 0.999.
    //to avoid 0 and infinity
    float shininess = 2.0 / pow(clamp(roughness, 0.001, 0.999), 4.0) - 2.0;
    
    //compute ambient light 
    vec3 ambient_light = compute_sky_light(normal);
    vec3 color = emissive + ambient_light * ambient

    vec3 world_to_light_dir;
    //for each light source:
        //compute diffuse and specular amount and add them to the lighting effect of the vertex
    for(int light_idx = 0; light_idx < min(MAX_LIGHTS, light_count); light_idx++){
        Light light = lights[light_idx];
        float attenuation = 1.0;
        if(light.type == DIRECTIONAL){ //no atttenuation for directional light
            world_to_light_dir = -light.direction; 
        } else {
            world_to_light_dir = normalize(light.position - fs_in.world);

            float d = distance(light.position, fs_in.world);
            //world_to_light_dir /= d;
            //for point light ==> attenuation depend on the distance only
            // x*d^2 + y*d + z
            attenuation = 1.0 / dot(light.attenuation, vec3(d*d, d, 1.0));

            if(light.type == SPOT){
                //for spot light ==> attenuation depend on the distance and the angle
                float angle = acos(dot(light.direction, -world_to_light_dir));
                //inside the inner cone angle ==> light intensity is maximum
                //outside the outer cone angle ==> light intensity is 0
                //in between ==> light intensity is interpolated
                attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle);
            }
        }
        
        vec3 computed_diffuse = light.diffuse * diffuse * lambert(normal, world_to_light_dir);

        vec3 reflected = normalize(reflect(-world_to_light_dir, normal));
        
        vec3 computed_specular = light.specular * specular * phong(reflected, view, shininess);

        //color = emissive (if the material is lighting by itself) + ambient effect + diffuse effect+ specular effect
        color += (computed_diffuse + computed_specular) * attenuation;
    }
    //final color of the vertex that will be drawn on the screen
    frag_color = vec4(color, 1.0);
}