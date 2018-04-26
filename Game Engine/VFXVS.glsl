#version 440 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec2 squareUVs;

// Output data ; will be interpolated for each fragment.
out vec2 uv;

// Values that stay constant for the whole mesh.
uniform vec3 view_position;
uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 cameraRight_worldspace;
uniform vec3 cameraUp_worldspace;
uniform mat4 vp; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
uniform vec3 billboardPos; // Position of the center of the billboard
uniform vec2 billboardSize; // Size of the billboard, in world units (probably meters)

void main()
{
	//vec3 particleCenter_worldspace = billboardPos;
	//vec3 camToParticle = particleCenter_worldspace - view_position;
	
	//The Tutorial
	//vec3 vertexPosition_worldspace = particleCenter_worldspace 
	//	+ cameraRight_worldspace * squareVertices.x * billboardSize.x 
	//	+ cameraUp_worldspace    * squareVertices.y * billboardSize.y;

	// Output position of the vertex
	gl_Position = projection_matrix * view_matrix * world_matrix * vec4(squareVertices, 1.0);
	uv = squareUVs;

	// Or, if BillboardSize is in percentage of the screen size (1,1 for fullscreen) :
	//vertexPosition_worldspace = particleCenter_wordspace;
	//gl_Position = vp * vec4(vertexPosition_worldspace, 1.0f); // Get the screen-space position of the particle's center
	//gl_Position /= gl_Position.w; // Here we have to do the perspective division ourselves.
	//gl_Position.xy += squareVertices.xy * vec2(0.2, 0.05); // Move the vertex in directly screen space. No need for CameraUp/Right_worlspace here.
}