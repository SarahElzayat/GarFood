#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

in vec2 tex_coord;
out vec4 frag_color;

// This shader will produce a fisheye effect centered at (0.5, 0.5) with a specific radius and distortion scale.
void main() {
    vec2 center = vec2(0.5f, 0.5f); // center of the fisheye circle
    float radius = 20.0f;  // radius of the fisheye
    float scale = -0.3f;   // controls the amount of distortion applied to the texture.

    vec2 tex_coord_to_use = tex_coord;
    float dist = distance(center, tex_coord);
    tex_coord_to_use -= center;    // get realtive position to the fisheye center by subtracting position from the fisheye center

     // apply the fisheye efect to the pixels inside the fisheye circle
     // the distance between the pixel and the center must be less than the radius
    if(dist < radius) {
        float percent = 1.0 + ((0.5 - dist) / 0.5) * scale; // calculate the percentage of the distortion based on the pixel's position from the center

        tex_coord_to_use = tex_coord_to_use * percent;
    }
    tex_coord_to_use += center;    // translate back to original position by reversing the above subtraction

    frag_color = texture(tex, tex_coord_to_use);  // return the final texture with the new tex_coord

}