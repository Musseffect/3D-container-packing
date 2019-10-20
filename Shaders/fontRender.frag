

uniform vec2 pos;
uniform vec2 size;

uniform sampler2D fontTex;
in vec2 out_uv;

void main()
{
	gl_FragColor=vec4(texture(fontTex,pos+out_uv*size));
}