#version 440 core
layout(location = 0) in vec3 vertex_position;

uniform mat4 world_matrix;

void main()
{
	gl_Position = world_matrix * vec4(vertex_position, 1.0);
}