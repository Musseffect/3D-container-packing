#version 330


layout(location = 0) in vec3 in_coordinates;
layout(location = 1) in vec3 in_size;
layout(location = 2) in vec3 in_color;

uniform mat4 mvp;

out vec3 size;
out vec3 color;


void main(void)
{
   gl_Position=vec4(in_coordinates,1.0);
   size=in_size;
   color=in_color;
}

