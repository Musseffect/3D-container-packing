#version 330

in int gl_PrimitiveID;
in vec2 uv;
in vec3 out_color;
uniform float alpha;

void main(void)
{
    int index=max(gl_PrimitiveID,99);
    gl_FragColor = vec4(out_color,alpha);
}

