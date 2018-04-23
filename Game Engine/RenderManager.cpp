#include "RenderManager.h"
#include "VFX.h"

RenderManager::RenderManager()
{
	createBuffers();
}

RenderManager::RenderManager(GameScene * otherGameScene, GLFWwindow* otherWindow, ShaderProgramLib* shaderProgram)
{
	gameScene = otherGameScene;
	window = otherWindow;

	this->geometryShaderProgram = shaderProgram->getShader<GeometryShaders>()->geometryShaderProgram;
	this->lightpassShaderProgram = shaderProgram->getShader<LightpassShaders>()->lightpassShaderProgram;
	//this->animationShaderProgram = shaderProgram->getShader<AnimationShaders>()->animationShaderProgram;
	this->shadowMapShaderProgram = shaderProgram->getShader<ShadowMapShader>()->ShadowMapShaderProgram;
	this->UIShaderProgram = shaderProgram->getShader<UIShaders>()->UIShaderProgram;
	createBuffers();
	vao = 0;
}

RenderManager::~RenderManager()
{
}

void RenderManager::FindObjectsToRender() {
	for (unsigned int i = 0; i < gameScene->gameObjects.size(); i++) {
	/*	glm::vec3 vectorToObject = gameScene->gameObjects[0].transform.position - gameScene->gameObjects[i].transform.position;

		float distance = length(vectorToObject);*/

		if (gameScene->gameObjects[i].getIsRenderable() == true) {
			gameObjectsToRender.push_back(&gameScene->gameObjects[i]);
		}

		if (gameScene->gameObjects[i].hasLight == true) {
			lightsToRender.push_back(gameScene->gameObjects[i].lightComponent);
			//rework this
		}
	}
}

void RenderManager::createBuffers()
{
	//screen size
	glfwGetFramebufferSize(window, &display_w, &display_h);

	//----------========== ShadowMap FBO DIRECTIONAL LIGHTS ==========----------
	glGenFramebuffers(1, &shadowFBO);
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, HIGH_SHADOW, HIGH_SHADOW, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//... VFX

	static const GLfloat g_vertex_buffer_data[] = {
		0.0f, 4.5f, 0.5f,
		0.0f, 4.5f, -0.5f,
		0.0f,  5.5f, 0.5f,
		0.0f,  5.5f, -0.5f,
	};
	glGenVertexArrays(1, &billboardVAO);
	glBindVertexArray(billboardVAO);
	glGenBuffers(1, &billboardVBO);
	glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &particlePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLfloat) * 4, particlePositionData);

	glGenBuffers(1, &particleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, 5 * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(GLfloat) * 4, particleColorData);

	//... Create G-buffers
	//framebufferobject
	glGenFramebuffers(1, &gbo);
	glBindFramebuffer(GL_FRAMEBUFFER, gbo);
	//g-buffer position
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	//g-buffer normal
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	//g-buffer albedo
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	//g-buffer specular
	glGenTextures(1, &gSpecular);
	glBindTexture(GL_TEXTURE_2D, gSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gSpecular, 0);

	//g-buffer metallic
	glGenTextures(1, &gMetallic);
	glBindTexture(GL_TEXTURE_2D, gMetallic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gMetallic, 0);

	//g-buffer AO
	glGenTextures(1, &gAO);
	glBindTexture(GL_TEXTURE_2D, gAO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gAO, 0);

	glDrawBuffers(6, attachments);

	//... Create and attach depth buffer
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, display_w, display_h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	//... Create Colorbuffer
	glGenFramebuffers(1, &finalFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
	glGenTextures(1, &finalColorBuffer);
	glBindTexture(GL_TEXTURE_2D, finalColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, display_w, display_h, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;

	//... Create and attach depth and stencil buffer
	glGenRenderbuffers(1, &finalDepthStensil);
	glBindRenderbuffer(GL_RENDERBUFFER, finalDepthStensil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, display_w, display_h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, finalDepthStensil);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, finalDepthStensil);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	//.. Create UI Frame Buffer with UI Texture
	int width, height, nrOfChannels;
	unsigned char * data = stbi_load("uiTextureFlipped.png", &width, &height, &nrOfChannels, 0);

	glGenFramebuffers(1, &UIFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, UIFBO);

	glGenTextures(1, &UITexture);
	glBindTexture(GL_TEXTURE_2D, UITexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load UI Texture from path" << std::endl;
	}

	stbi_image_free(data);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, UITexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "UI Framebuffer not complete!" << std::endl;
}

void RenderManager::Render() {
	FindObjectsToRender();

	//... Set view and projection matrix
	view_matrix = glm::lookAt(gameScene->gameObjects[0].transform->position,
		gameScene->gameObjects[0].transform->position + gameScene->gameObjects[0].transform->forward,
		gameScene->gameObjects[0].transform->up);

	projection_matrix = glm::perspective(glm::radians(60.0f), float(display_w) / float(display_h), 0.1f, 100.0f);

	glm::mat4 world_matrix = glm::mat4(1);
	world_matrix = glm::translate(world_matrix, glm::vec3(10.0f, -5.0f, 0.0f));
	world_matrix = glm::rotate(world_matrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//... Clear Back Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.749, 0.843, 0.823, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//... Clear finalFBO
	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//DIRECTIONAL LIGHT SHADOWMAP PASS----------------------------------------------------------------------------------------------------------------------------------------
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glUseProgram(shadowMapShaderProgram);
	setupMatrices(shadowMapShaderProgram, gameScene->gameObjects[1].transform->position);
	glViewport(0, 0, HIGH_SHADOW, HIGH_SHADOW);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(glGetUniformLocation(shadowMapShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));

	for (unsigned int i = 0; i < gameObjectsToRender.size(); i++)
	{
		gameObjectsToRender[i]->meshFilterComponent->bindVertexArray();

		glDrawElements(GL_TRIANGLES, gameObjectsToRender[i]->meshFilterComponent->vertexCount, GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, display_w, display_h);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	//... GEOMETRY PASS----------------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(geometryShaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF); // enable writing to the stencil buffer

	gameObjectsToRender[0]->materialComponent->bindTextures();
	gameObjectsToRender[0]->materialComponent->bindFoundTextures();

	for (unsigned int i = 0; i < gameObjectsToRender.size(); i++)
	{
		if (i < 2)
			glUniform1i(glGetUniformLocation(geometryShaderProgram, "followCamera"), 1);
		else
			glUniform1i(glGetUniformLocation(geometryShaderProgram, "followCamera"), 0);

		gameObjectsToRender[i]->meshFilterComponent->bindVertexArray();

		glDrawElements(GL_TRIANGLES, gameObjectsToRender[i]->meshFilterComponent->vertexCount, GL_UNSIGNED_INT, 0);
	}

	//... VFX

	glUseProgram(VFXShaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glUniform1i(glGetUniformLocation(VFXShaderProgram, "myTextureSampler"), 0);

	glUniform3f(glGetUniformLocation(VFXShaderProgram, "cameraRight_worldspace"), view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
	glUniform3f(glGetUniformLocation(VFXShaderProgram, "cameraUp_worldspace"), view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);

	glUniform3f(glGetUniformLocation(VFXShaderProgram, "billboardPos"), 0.0f, 5.5f, 0.0f);
	glUniform2f(glGetUniformLocation(VFXShaderProgram, "billboardSize"), 1.0f, 0.125f);

	float lifeLevel = sin(deltaTime)* 0.1f + 0.7f;

	glUniform1f(glGetUniformLocation(VFXShaderProgram, "lifeLevel"), lifeLevel);
	glUniformMatrix4fv(glGetUniformLocation(VFXShaderProgram, "vp"), 1, GL_FALSE, &projection_matrix[0][0]);

	glBindVertexArray(billboardVAO);
	glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);

	//renderQuad();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//------=====================Animation Pass=======================-------
	//glUseProgram(animationShaderProgram);

	//... Copy Stencil Buffer from gbo to finalFBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalFBO);
	glBlitFramebuffer(0, 0, display_w, display_h, 0, 0, display_w, display_h, GL_STENCIL_BUFFER_BIT, GL_NEAREST);

	//... LIGHTING PASS----------------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
	glUseProgram(lightpassShaderProgram);
	setupMatrices(lightpassShaderProgram, gameScene->gameObjects[1].transform->position);

	//CAM pos
	glUniform3fv(glGetUniformLocation(lightpassShaderProgram, "view_position"), 1, glm::value_ptr(gameScene->gameObjects[0].transform->position));

	//Lights
	for (unsigned int i = 0; i < lightsToRender.size(); i++)
	{
		//position
		std::string lightUniform = "lights[" + std::to_string(i) + "].Position";
		glUniform3fv(glGetUniformLocation(lightpassShaderProgram, lightUniform.c_str()), 1, glm::value_ptr(lightsToRender.at(i)->transform.position));

		//Color
		lightUniform = "lights[" + std::to_string(i) + "].Color";
		glUniform3fv(glGetUniformLocation(lightpassShaderProgram, lightUniform.c_str()), 1, glm::value_ptr(lightsToRender.at(i)->color));

		//Attenuation linear
		lightUniform = "lights[" + std::to_string(i) + "].Linear";
		glUniform1f(glGetUniformLocation(lightpassShaderProgram, lightUniform.c_str()), lightsToRender.at(i)->Linear);

		//Attenuation quadratic
		lightUniform = "lights[" + std::to_string(i) + "].Quadratic";
		glUniform1f(glGetUniformLocation(lightpassShaderProgram, lightUniform.c_str()), lightsToRender.at(i)->Quadratic);
	}
	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "gPosition"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "gNormal"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "gAlbedo"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "gSpecular"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gSpecular);

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "gMetallic"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gMetallic);

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "gAO"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, gAO);

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "depthMap"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilMask(0x00); // disable writing to the stencil buffer

	renderQuad();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	glDisable(GL_STENCIL_TEST);

	//... UI -----------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(UIShaderProgram);

	glUniform1i(glGetUniformLocation(UIShaderProgram, "theTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, UITexture);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "equipedTexture"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->equipedTexture);

	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture1"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[0]);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture2"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[1]);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture3"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[2]);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture4"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[3]);
	glUniform1i(glGetUniformLocation(UIShaderProgram, "inventoryTexture5"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, gameScene->gameObjects[0].getPlayer()->inventoryTexture[4]);
	
	glUniform1i(glGetUniformLocation(UIShaderProgram, "SceneTexture"), 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, finalColorBuffer);

	glUniform1f(glGetUniformLocation(UIShaderProgram, "hp"), gameScene->gameObjects[0].getPlayer()->hp);
	glUniform1f(glGetUniformLocation(UIShaderProgram, "cold"), gameScene->gameObjects[0].getPlayer()->cold);
	glUniform1f(glGetUniformLocation(UIShaderProgram, "water"), gameScene->gameObjects[0].getPlayer()->water);
	glUniform1f(glGetUniformLocation(UIShaderProgram, "food"), gameScene->gameObjects[0].getPlayer()->food);
	glUniform1f(glGetUniformLocation(UIShaderProgram, "fade"), gameScene->gameObjects[0].getPlayer()->fade);

	glBindTexture(GL_TEXTURE_2D, finalColorBuffer);

	renderQuad();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	gameObjectsToRender.clear();
	lightsToRender.clear();
	Update();
}

void RenderManager::renderQuad()
{
	if (vao == 0)
	{
		unsigned int vertexPos;
		unsigned int uvPos;

		vertexPos = glGetAttribLocation(lightpassShaderProgram, "aPos");
		uvPos = glGetAttribLocation(lightpassShaderProgram, "aTexCoords");

		//create vertices
		QuadVertex vertices[] = {
			// pos and normal and uv for each vertex
			{ 1,  1, 1.0f, 1.0f },
			{ 1, -1, 1.0f, 0.0f },
			{ -1, -1, 0.0f, 0.0f },
			{ -1,  1, 0.0f, 1.0f },
		};

		unsigned int indices[] = {
			0,1,3,
			1,2,3,
		};

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);

		if (vertexPos == -1) {
			OutputDebugStringA("Error, can't find aPos attribute in vertex shader\n");
			return;
		}

		glVertexAttribPointer(
			0,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(QuadVertex),
			BUFFER_OFFSET(0)
		);

		glEnableVertexAttribArray(1);

		if (uvPos == -1) {
			OutputDebugStringA("Error, cannt find aTexCoords attribute in vertex shader\n");
			return;
		}

		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(QuadVertex),
			BUFFER_OFFSET(sizeof(float) * 2)
		);

		//ebo
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void RenderManager::setupMatrices(unsigned int shaderToUse, glm::vec3 lightPos)
{
	glUseProgram(shaderToUse);

	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 40.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glUniformMatrix4fv(glGetUniformLocation(shaderToUse, "LightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

int RenderManager::FindUnusedParticle()
{
	for (int i = 0; i < maxParticles; i++)
	{
		if (particleContainer[i].life < 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++)
	{
		if (particleContainer[i].life < 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	return 0;
}

void RenderManager::Update()
{
}

void RenderManager::setDeltaTime(float deltaTime)
{
	this->deltaTime = deltaTime;
}

void RenderManager::setSeconds(float seconds)
{
	this->seconds = seconds;
}
