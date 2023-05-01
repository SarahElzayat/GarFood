#version 330 core

in Varyings {       /// input from tinted vertex shader 
    vec4 color;
} fs_in;

out vec4 frag_color; /// the computed fragment color for each pixel

uniform vec4 tint;

void main() {
    //TODO: (Req 7) Modify the following line to compute the fragment color

    /// by multiplying the tint with the vertex color
    frag_color = tint * fs_in.color;
}