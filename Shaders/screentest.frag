#version 330
in vec2 uv;

void main(void)
{
    gl_FragColor = vec4(uv,0.0,1.0);
}

