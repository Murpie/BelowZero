#include "RenderManager.h"

RenderManager::RenderManager()
{
	createBuffers();
}

RenderManager::RenderManager(GameScene * otherGameScene, GLFWwindow* otherWindow, ShaderProgramLib* shaderProgram)
{
	gameScene = otherGameScene;
	window = otherWindow;

	this->geometryShaderProgram = shaderProgram->getShader<GeometryShaders>()->geometryShaderProgram;
	this->cubeMapShaderProgram = shaderProgram->getShader<CubeMapShaders>()->cubeMapShaderProgram;
	this->lightpassShaderProgram = shaderProgram->getShader<LightpassShaders>()->lightpassShaderProgram;
	this->skyboxShaderProgram = shaderProgram->getShader<SkyboxShaders>()->skyboxShaderProgram;
	//this->animationShaderProgram = shaderProgram->getShader<AnimationShaders>()->animationShaderProgram;
	this->shadowMapShaderProgram = shaderProgram->getShader<ShadowMapShader>()->ShadowMapShaderProgram;
	//this->pointLightShaderProgram = shaderProgram->getShader<PointLightShadowMapShaders>()->PointLightShaderProgram;
	this->UIShaderProgram = shaderProgram->getShader<UIShaders>()->UIShaderProgram;
	createBuffers();
	vao = 0;
	skyboxVAO = 0;
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

	//----------========== ShadowMap FBO POINT LIGHTS (CUBE MAP) ==========----------
	/*glGenFramebuffers(1, &cubeMapShadowFBO);
	glGenTextures(1, &cubeMapShadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapShadowMap);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, cubeMapShadowFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapShadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	// cube VAO
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	//... Create textures for Skybox Cube Map
	glGenFramebuffers(1, &skyFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, skyFBO);
	cubemapTexture = loadCubemap(faces);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cubemapTexture, 0);

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
	setupMatrices(shadowMapShaderProgram, gameScene->gameObjects[2].transform->position);
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

	////POINT LIGHT SHADOWMAP PASS----------------------------------------------------------------------------------------------------------------------------------------
	//glUseProgram(pointLightShaderProgram);
	//setupMatricesForCubeMapShadowMap(pointLightShaderProgram, gameScene->gameObjects[1].transform.position);
	//glViewport(0, 0, 1024, 1024);
	//glBindFramebuffer(GL_FRAMEBUFFER, cubeMapShadowFBO);
	//glClear(GL_DEPTH_BUFFER_BIT);

	//glUniformMatrix4fv(glGetUniformLocation(pointLightShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(world_matrix));
	//glUniform3fv(glGetUniformLocation(pointLightShaderProgram, "lightPos"), 1, glm::value_ptr(gameScene->gameObjects[1].transform.position));


	//for (unsigned int i = 0; i < gameObjectsToRender.size(); i++)
	//{
	//	gameObjectsToRender[i]->meshFilterComponent->bindVertexArray();
	//	glDrawElements(GL_TRIANGLES, gameObjectsToRender[i]->meshFilterComponent->vertexCount, GL_UNSIGNED_INT, 0);
	//}

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, display_w, display_h);

	
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	
	//... GEOMETRY PASS----------------------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(geometryShaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(glGetUniformLocation(geometryShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
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
		gameObjectsToRender[i]->meshFilterComponent->bindVertexArray();

		glDrawElements(GL_TRIANGLES, gameObjectsToRender[i]->meshFilterComponent->vertexCount, GL_UNSIGNED_INT, 0);
	}

	//------=====================Animation Pass=======================-------
	//glUseProgram(animationShaderProgram);

	////... CUBE MAP GEOMETREY PASS------------------------------------------------------------------------------------------------------------------------------
	//glBindVertexArray(cubeVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	//glUseProgram(cubeMapShaderProgram);

	//// Reflective Cube Map World Matrix
	//glm::mat4 cube_world_matrix = glm::mat4(1);
	//cube_world_matrix = glm::translate(cube_world_matrix, glm::vec3(2, 0.5, 0));
	//cube_world_matrix = glm::rotate(cube_world_matrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//glUniformMatrix4fv(glGetUniformLocation(cubeMapShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	//glUniformMatrix4fv(glGetUniformLocation(cubeMapShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	//glUniformMatrix4fv(glGetUniformLocation(cubeMapShaderProgram, "world_matrix"), 1, GL_FALSE, glm::value_ptr(cube_world_matrix));
	//glUniform3fv(glGetUniformLocation(cubeMapShaderProgram, "cameraPos"), 1, glm::value_ptr(gameScene->gameObjects[0].transform.position));

	//glStencilFunc(GL_ALWAYS, 2, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	//glDrawArrays(GL_TRIANGLES, 0, 36);

	////... CUBE MAP REFLECTION PASS TO FINAL IMAGE--------------------------------------------------------------------------------------------------------------
	//glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);

	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glDisable(GL_CULL_FACE);

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

	glUniform1i(glGetUniformLocation(lightpassShaderProgram, "cubeMapdepthMap"), 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapShadowMap);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilMask(0x00); // disable writing to the stencil buffer

	renderQuad();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//... SKYBOX PASS--------------------------------------------------------------------------------------------------------------------------------------------
	//glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
	//glUseProgram(skyboxShaderProgram);

	//view_matrix = glm::mat4(glm::mat3(view_matrix));
	//glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	//glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
	//glUniform1i(glGetUniformLocation(skyboxShaderProgram, "skybox"), 0);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_EQUAL, 0, 0xFF);
	//glStencilMask(0x00); // disable writing to the stencil buffer

	//renderSkyQuad();
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

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

void RenderManager::renderSkyQuad()
{
	if (skyboxVAO == 0)
	{
		glGenVertexArrays(1, &skyboxVAO);
		glBindVertexArray(skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxArray), skyboxArray, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		//ebo
		glGenBuffers(1, &skyboxEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);
	}
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
}

void RenderManager::setupMatrices(unsigned int shaderToUse, glm::vec3 lightPos)
{
	glUseProgram(shaderToUse);

	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 40.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glUniformMatrix4fv(glGetUniformLocation(shaderToUse, "LightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

void RenderManager::setupMatricesForCubeMapShadowMap(unsigned int shaderToUse, glm::vec3 lightPosition)
{
	glUseProgram(pointLightShaderProgram);

	float aspectRatio = 1024.0f / 1024.0f;
	float nearPlane = 1.0f;
	float farPlane = 25.0f;
	glm::mat4 cubeMapShadowProjection = glm::perspective(glm::radians(90.0f), aspectRatio, nearPlane, farPlane);

	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(cubeMapShadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))); //RIght
	shadowTransforms.push_back(cubeMapShadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0),glm::vec3(0.0, -1.0, 0.0))); //Left
	shadowTransforms.push_back(cubeMapShadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0))); //Top
	shadowTransforms.push_back(cubeMapShadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, -1.0, 0.0),glm::vec3(0.0, 0.0, -1.0))); //Bottom
	shadowTransforms.push_back(cubeMapShadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0))); //Near
	shadowTransforms.push_back(cubeMapShadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, -1.0),glm::vec3(0.0, -1.0, 0.0))); //Far
	
	std::string test = "";
	
	for (int i = 0; i < 6; i++)
	{
		test = "shadowMatrices[" + std::to_string(i) + "]";
		GLint shadowMatrices = glGetUniformLocation(shaderToUse, test.c_str());
		glUniformMatrix4fv(shadowMatrices, 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
	}
		glUniform1f(glGetUniformLocation(pointLightShaderProgram, "far_plane"), farPlane);
}

unsigned int RenderManager::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
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
