#version 440 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 uv_coord;

uniform mat4 world_matrix;
uniform mat4 LightSpaceMatrix;

void main()
{
	gl_Position = LightSpaceMatrix * world_matrix * vec4(vertex_position, 1.0);
}