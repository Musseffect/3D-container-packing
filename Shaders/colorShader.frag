#version 330

in vec3 out_normal;
uniform vec3 color;
uniform float alpha;

void main()
{
   vec3 normal=normalize(out_normal);
   gl_FragColor=vec4(color,alpha)*abs(normal.z);
}
