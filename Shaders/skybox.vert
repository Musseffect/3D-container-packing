#version 330
in vec3 vertex;
uniform mat4 mvp;
out vec3 direction;

void main(void)
{
    vec4 v = mvp * vec4(vertex,1.0);
    direction=vertex;
    gl_Position= v.xyww;
}

