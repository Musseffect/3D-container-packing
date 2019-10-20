in vec2 localPosition;
in vec2 uv;

uniform cameraMatrix;

uniform vec2 size;
uniform vec3 center;
out vec2 out_uv;

void main()
{
	vec3 right=vec3(cameraMatrix[0][0],cameraMatrix[1][0],cameraMatrix[2][0]);
	vec3 up=vec3(cameraMatrix[0][1],cameraMatrix[1][1],cameraMatrix[2][1]);
	gl_Position=vec4(center+size.x*localPosition.x*right+
	size.y*localPosition.y*up,1.0);
	out_uv=uv;
}