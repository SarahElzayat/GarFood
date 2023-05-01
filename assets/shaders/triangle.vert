#version 330

// This vertex shader should be used to render a triangle whose normalized device coordinates are:
// (-0.5, -0.5, 0.0), ( 0.5, -0.5, 0.0), ( 0.0,  0.5, 0.0)
// And it also should send the vertex color as a varying to the fragment shader where the colors are (in order):
// (1.0, 0.0, 0.0), (0.0, 1.0, 0.0), (0.0, 0.0, 1.0)

out Varyings {
    vec3 color;
} vs_out;

// Currently, the triangle is always in the same position, but we don't want that.
// So two uniforms should be added: translation (vec2) and scale (vec2).
// Each vertex "v" should be transformed to be "scale * v + translation".
// The default value for "translation" is (0.0, 0.0) and for "scale" is (1.0, 1.0).

uniform vec2 scale = vec2(1.0, 1.0);
uniform vec2 translation = vec2(0.0, 0.0);

//TODO: (Req 1) Finish this shader

void main() {

    // Array of positions of triangle vertices
    // the positions are normalized, so that the center of the screen is (0.0, 0.0, 0.0)
    // and top right corner is (1.0, 1.0, 0.0)
    const vec3 positions[3] = vec3[3](
        vec3(-0.5, -0.5, 0.0), 
        vec3(0.5, -0.5, 0.0), 
        vec3(0.0, 0.5, 0.0));

    // Array of colors
    // Each color is assigned to the corresponding vertex in the "positions" vector
    const vec3 colors[3] = vec3[3](
        vec3(1.0, 0.0, 0.0), 
        vec3(0.0, 1.0, 0.0), 
        vec3(0.0, 0.0, 1.0));

    // The gl_VertexID is a variable that contains the index of the current vertex
    // We'll use it to pick a position and send it to the gl_Position variable
    gl_Position = vec4(positions[gl_VertexID], 1.0);

    // Scale the posision of the vertex by multiplying the xy components
    gl_Position.xy *= scale;

    // Translate the vertex position by adding the translation vector to the xy components
    gl_Position.xy += translation;

    // The color of the current vertex is assigned as the corresponding one from the colors array
    vs_out.color = colors[gl_VertexID];

}