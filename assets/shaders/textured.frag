#version 330 core

/// input from textured.vert

in Varyings {
    vec4 color;
    vec2 tex_coord;
} fs_in;

/// the computed fragment color for each pixel
out vec4 frag_color;

uniform vec4 tint;
uniform sampler2D tex;

void main() {
    //TODO: (Req 7) Modify the following line to compute the fragment color
    /// "tint" stores the color that will be applied as a tint to the texture.
    /// "tex" stores the texture that will be tinted
    /// texture is sampled using its coordinates stored in fs_in.tex_coord 
    /// each fragment is assigned a color based on the texture information at the corresponding texture coordinate.
    /// the frag_color is computed by multiplying the sampled texture color with the input vertex color (fs_in.color) and the tint color (tint). 
    frag_color = texture(tex, fs_in.tex_coord) * fs_in.color * tint;
}