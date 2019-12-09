#version 330
in vec2 uv;
uniform sampler2D charAtlas;
uniform vec2 charID;
uniform vec3 color;

void main(void)
{
    vec2 texCoord=uv*vec2(1.0/16.0)+vec2(charID)/16.0;

    float alpha=1.0-smoothstep(0.475,0.525,texture2D(charAtlas, texCoord).a);
    if(alpha==0.0)
        discard;
    gl_FragColor = vec4(mix(vec3(1.),vec3(color),alpha),alpha);
    //gl_FragColor=vec4(smoothstep(-0.05,0.05,texture2D(charAtlas,texCoord).a),0.0,0.0,1.0);
}

