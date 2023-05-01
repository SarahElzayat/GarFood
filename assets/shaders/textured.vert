#version 330 core

/// input to textured vertex shader from c++ code
/// locations are set manually so we can directly send the data to loc 0,1... etc without finding the variable location  
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;

/// output to fragment shader
out Varyings {
    vec4 color;
    vec2 tex_coord;
} vs_out;

/// uniform transformation matrix for all vertices
uniform mat4 transform;

void main() {
    //TODO: (Req 7) Change the next line to apply the transformation matrix
   /// to transform each vertex, it's potision is multiplied by the transformation matrix
    /// the result is then stored in gl_Position.
    /// since the vertex is a point, so to be represted in 
    /// homogenous coodinates, an extra dimension is added and set to 1
    gl_Position = transform * vec4(position, 1.0);

    /// pass the color and texture coordinate to the fragment shader
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
}