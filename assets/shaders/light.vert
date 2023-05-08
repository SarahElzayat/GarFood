#version 330

//Attributes are used to pass per-vertex data from the CPU to the GPU.
//Uniform variables are used to pass data that is constant across all vertices in a draw call.


//Attributes: variables that are used to store information about each vertex. 
//vertex shader uses these attributes to transform the vertices into their final positions on the screen, and to apply lighting and other effects to the model
//passed from the application to the vertex shader
//layout(location = x) ==> states where each vertex attribute is stored in the Vertex buffer object
//ensure that the vertex data is properly mapped to the correct attributes in the shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;


//uniforms are variables that are sent from the CPU to the GPU

// 1- Object to World.
//The model matrix
//to transform the vertices of a 3D object from its local coordinate system to the world coordinate system.
uniform mat4 M;
//Inverse transpose of model matrix
//is used for normal transformation
//because it removes any scaling and rotation applied to the model matrix and ensures that the resulting normal vector is perpendicular to the surface it represents.
uniform mat4 M_IT;
//(View-Projection) Matrix: Camera View Matrix*Projection Matrix.
//It transforms objects from world space into screen space.
// 2- World to Homogenous Clipspace.
uniform mat4 VP;
// The camera position matrix is used to translate objects in camera space based on the position of the camera.
//used in frag shader to compute specular
uniform vec3 camera_position;


//Varyings are "out" from a shader and "in" to another shader
//Send vertex information to the fragment shader
out Varyings {
    vec4 color;
    //(texture image: uv space)
    vec2 tex_coord;
    vec3 normal;
    //vertex to eye vector in the world space
    vec3 view;
    //verteex in world space
    vec3 world;
} vs_out;

void main() {
    //transform vertex to world coordinates
    vec3 world = (M * vec4(position, 1.0)).xyz;
    //vertex's position in the homogenous clipSpace
    gl_Position = VP * vec4(world, 1.0);
    //Send varyings from vertex shader to fragment shader
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
    vs_out.world = world;
    //transform normal to world coordinates
    //==>w=0 as it is a vector
    vs_out.normal = normalize((M_IT * vec4(normal, 0.0)).xyz);
    //vertex to eye vector in the world space
    //used in frag shader to compute specular
    vs_out.view = camera_position - world;
}