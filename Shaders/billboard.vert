#version 330

in vec3 position;
in vec2 in_uv;
uniform mat4 mvp;
out vec2 uv;

void main(void)
{
    gl_Position = mvp * vec4(position,1.0);
    uv=in_uv;
}

