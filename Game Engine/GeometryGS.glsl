#version 440 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

in VS_OUT{
	vec3 vertex_normal;
	vec2 uv_coord;
	vec3 tangent;
	vec3 bitangent;
	mat4 world_matrix;
	mat4 view_matrix;
	mat4 projection_matrix;
	int followCamera;
} gs_in[];

out GS_OUT{
	vec4 FragPos;
	vec2 TexCoords;
	mat3 TBN;
} gs_out;

void main() {

	vec4 pointA = gs_in[0].world_matrix * gl_in[0].gl_Position;
	vec4 pointB = gs_in[0].world_matrix * gl_in[1].gl_Position;
	vec4 pointC = gs_in[0].world_matrix * gl_in[2].gl_Position;

	vec4 ab = pointB - pointA;
	vec4 ac = pointC - pointA;
	vec3 normal = normalize(cross(ab.xyz, ac.xyz));

	vec4 camRay = (inverse(gs_in[0].view_matrix) * vec4(0.0, 0.0, 0.0, 1.0)) - pointA;

	float d = dot(-normal, camRay.xyz);
	if (d > 0.0 || gs_in[0].followCamera == 1)
	{
		for (int i = 0; i < gl_in.length(); i++)
		{
			if (gs_in[0].followCamera == 1)
				gl_Position = gs_in[i].projection_matrix * gl_in[i].gl_Position;
			else
				gl_Position = gs_in[i].projection_matrix * gs_in[i].view_matrix * gs_in[i].world_matrix * gl_in[i].gl_Position;
			gs_out.FragPos = gs_in[i].world_matrix * gl_in[i].gl_Position;
				
			gs_out.TexCoords = gs_in[i].uv_coord;

			mat3 worldSpace = mat3(gs_in[i].world_matrix);
			vec3 B = cross(gs_in[i].vertex_normal, gs_in[i].tangent);
			vec3 T = gs_in[i].tangent;
			if (dot(cross(gs_in[i].vertex_normal, T), B) < 0.0)
				T = T * -1.0;
			gs_out.TBN = worldSpace* mat3(T, B, gs_in[i].vertex_normal);

			EmitVertex();
		}

		EndPrimitive();
	}
}