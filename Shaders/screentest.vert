#version 330
in vec2 position;//-1,1
out vec2 uv;

void main(void)
{
    gl_Position = vec4(position,0.0,1.0);
    uv = position*0.5+0.5;
}

