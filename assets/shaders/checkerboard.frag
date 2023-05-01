#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the bottom-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//TODO: (Req 1) Finish this shader.

uniform int size = 32;
uniform vec3 colors[2];

void main(){
    //Deviding FragCoord/size gives us the tile Coordinate[tile order on the grid](1st tile, 1nd tile ..etc)
    //As we start bottom-left most with be "colors[0]" ==>
    //Tiles that has colors[0] are (0,0),(1,1),(2,2),(0,2).....etc
    //Tiles that has colors[1] are (0,1),(1,0),(2,3),(1,2)....etc
    //So Tiles that have colors[0] are tiles with x&y tile coordinates are both even or both odd
    // Tiles that have colors[1] are tiles one of tile coordinates is even and the other is odd 
    int color=(int(gl_FragCoord.x/size)%2==int(gl_FragCoord.y/size)%2)?0:1;
    frag_color = vec4(colors[color], 1.0);

}