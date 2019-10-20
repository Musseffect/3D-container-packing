#version 330
in vec3 direction;

void main(void)
{
    vec3 rd=normalize(direction);
    gl_FragColor=vec4(1.0,1.0,1.0,1.0);
    //gl_FragColor = vec4(max(rd,vec3(0.0)),1.0);
}

