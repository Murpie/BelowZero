#version 440 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
	Normal = vec3(world_matrix * vec4(aNormal, 0));
	Position = vec3(world_matrix * vec4(aPos, 1));
	gl_Position = projection_matrix * view_matrix * world_matrix * vec4(aPos, 1.0);
}