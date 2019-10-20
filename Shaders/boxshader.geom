#version 330


layout(points) in;
layout(triangle_strip, max_vertices = 14) out;


in vec3 size[];
in vec3 color[];
out vec2 uv[];
out vec3 out_color;

uniform mat4 mvp;


void main()
{
    vec4 center = mvp*gl_in[0].gl_Position;
    vec3 sz=size[0];
    vec4 dx = mvp*vec4(sz.x,0.0,0.0,0.0);
    vec4 dy = mvp*vec4(0.0,sz.y,0.0,0.0);
    vec4 dz = mvp*vec4(0.0,0.0,sz.z,0.0);
    vec4 p1 = center-(dx+dy+dz)*0.5;
    vec4 p2 = p1 + dx;
    vec4 p3 = p1 + dy;
    vec4 p4 = p2 + dy;
    vec4 p5 = p1 + dz;
    vec4 p6 = p2 + dz;
    vec4 p7 = p3 + dz;
    vec4 p8 = p4 + dz;


    gl_Position = p7;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p8;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p5;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p6;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p2;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p8;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p4;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p7;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p3;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p5;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p1;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p2;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p3;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();

    gl_Position = p4;
    gl_PrimitiveID = gl_PrimitiveIDIn;
    out_color=color[0];
    EmitVertex();
    EndPrimitive();
}
