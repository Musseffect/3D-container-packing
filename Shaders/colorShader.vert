#version 330

in vec3 position;
in vec3 normal;

uniform mat4 mvp;
uniform mat3 normalMatrix;

out vec3 out_normal;

void main()
{
    gl_Position=mvp*vec4(position,1.0);
    out_normal=normalize(normalMatrix*normal);
}
