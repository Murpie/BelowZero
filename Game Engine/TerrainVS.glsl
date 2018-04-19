#version 440 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 uv_coord;

uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 normal;
out vec2 uv;
out vec4 fragpos;


void main()
{
	fragpos = world_matrix * vec4(vertex_position, 1.0);


	normal = normalize(vec4(world_matrix * vec4(vertex_normal, 1.0))).xyz;
	normal = normalize(-vertex_normal);
	uv = uv_coord;

	
	gl_Position = projection_matrix * view_matrix * world_matrix * vec4(vertex_position, 1.0);
	//fragpos = gl_Position;
}