#version 440 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 uv_coord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out VS_OUT {
	vec3 vertex_normal;
	vec2 uv_coord;
	vec3 tangent;
	vec3 bitangent;
	mat4 world_matrix;
	mat4 view_matrix;
	mat4 projection_matrix;
} vs_out;

void main() {
	gl_Position = vec4(vertex_position, 1.0);
	vs_out.vertex_normal = vertex_normal;
	vs_out.uv_coord = uv_coord;
	vs_out.tangent = tangent;
	vs_out.bitangent = bitangent;
	vs_out.world_matrix = world_matrix;
	vs_out.view_matrix = view_matrix;
	vs_out.projection_matrix = projection_matrix;
}