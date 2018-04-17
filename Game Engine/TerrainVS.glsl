#version 440 core
layout(location = 0) in vec3 vertex_position;
layout(location = 0) in vec3 vertex_normal;
layout(location = 0) in vec2 uv_coord;

uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 normal;
out vec2 uv;
out vec3 fragpos;


void main()
{
	fragpos = vec4(world_matrix * vec4(vertex_position, 1.0)).xyz;
	gl_Position = projection_matrix * view_matrix * world_matrix * vec4(vertex_position, 1.0);
	normal = vertex_normal;
	uv = uv_coord;

}