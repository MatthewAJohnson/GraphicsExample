#include "Renderer.h"


Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	camera = new Camera();
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	
	quad = Mesh::GenerateQuad();
	postProcessQuad = Mesh::GenerateQuad();
	tree = Mesh::GenerateCircle(50, 150);
	leaf = Mesh::GenerateTriangle();
	emitter = new ParticleEmitter();
	rain = new Weather();
	grassQuad = Mesh::GenerateQuad();
	
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxFragment.glsl");
	sceneShader = new Shader(SHADERDIR"shadowSceneVert.glsl", SHADERDIR"shadowSceneFrag.glsl");
	particleShader = new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl", SHADERDIR"geometry.glsl");
	processShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"processfrag.glsl");

	root = new SceneNode();
	root->AddChild(new Tree(tree, leaf,emitter,particleShader, RAW_HEIGHT*HEIGHTMAP_X /2.0f, 135, RAW_WIDTH*HEIGHTMAP_X / 2.0f));

	grassNode = new SceneNode();
	grassNode->AddChild(new Grass(grassQuad,RAW_HEIGHT*HEIGHTMAP_X /2.0f, 550.0f, RAW_WIDTH*HEIGHTMAP_X/2.0f, 10));
	grassNode->SetMesh(grassQuad);

	if(!reflectShader->LinkProgram()  || !skyboxShader->LinkProgram() || !sceneShader->LinkProgram() || !textShader->LinkProgram()
		|| !particleShader->LinkProgram() || !processShader->LinkProgram())
	{
		return;
	}

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"waterBump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	tree->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"tree.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	leaf->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"leaf.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	emitter->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"particle.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT));
	rain->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"particle.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT));
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"waterBump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg",TEXTUREDIR"rusted_east.jpg", 
									TEXTUREDIR"rusted_up.jpg",TEXTUREDIR"rusted_down.jpg",
									TEXTUREDIR"rusted_south.jpg",TEXTUREDIR"rusted_north.jpg",
									SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	grassQuad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"grassTex.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if(!cubeMap || !quad->GetTexture() || !heightMap->GetBumpMap() || !heightMap->GetTexture() || !tree->GetTexture() || !leaf->GetTexture()
		|| !grassQuad->GetTexture())
	{
		return;
	}
	
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);
	//SetUpPostProcess();
	/**Shadow Stuff**/
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	/*End Shadow stuff*/




	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(quad->GetBumpMap(),true);
	
	SetTextureRepeating(tree->GetTexture(), true);
	SetTextureRepeating(tree->GetBumpMap(),true);
	SetTextureRepeating(leaf->GetTexture(), true);
	
	SetTextureRepeating(heightMap->GetTexture(),true);
	SetTextureRepeating(heightMap->GetBumpMap(),true);

	waterRotate = 0.0f;

	//light = new Light(Vector3(RAW_HEIGHT*HEIGHTMAP_X /2.0f,500.0f,RAW_HEIGHT*HEIGHTMAP_X /2.0f), Vector4(1,1,1,1), ((RAW_WIDTH * HEIGHTMAP_X) / 2.0f) *3);
	light = new Light(Vector3(0,1000.0f,0), Vector4(1,1,1,1), ((RAW_WIDTH * HEIGHTMAP_X) / 2.0f) *6);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	camera->SetPosition(Vector3(RAW_HEIGHT*HEIGHTMAP_X /2.0f - 100, 150.0f, RAW_WIDTH*HEIGHTMAP_X-100));
		
	
	glEnable(GL_DEPTH_TEST); 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);



	fpsTimer = 0;
	fps = 0;
	frameCounter = 0;
	memCount = 0;
	init = true;

}

Renderer::~Renderer()
{
	delete camera;
	
	delete light;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete particleShader;
	delete processShader;

	delete leaf;
	delete postProcessQuad;
	delete root;
	delete grassNode;
	delete grassQuad;

	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteTextures(2,bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	delete sceneShader;
	delete emitter;
	delete rain;
	currentShader = 0;
}

void Renderer::RenderScene()
{
//	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
		
	DrawShadowScene();
		
	DrawCombinedScene();
	//DrawPostProcess();
	
//	PresentScene();
	
	//DrawParticleEmitter(rain); //

	//glDisable(GL_DEPTH_TEST);
	DrawText("Current Frame Rate: ", Vector3(0,0,0), 16.0f, fps + 0.5f); //+0.5 to round up
	DrawText("Current Tree Mem Use: ", Vector3(1,20,1), 16.0f, memCount);
	//glEnable(GL_DEPTH_TEST);
	

	
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	//DrawParticleEmitter(rain);
	glUseProgram(0);


	SwapBuffers();
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
	root->Update(msec);
	grassNode->Update(msec);
	emitter->Update(msec);
	rain->Update(msec);
	fpsTimer += msec;
	frameCounter++;
	if(fpsTimer >= 1000)
	{
		fps = (float)frameCounter / (fpsTimer * 0.001f);
		memCount = GetTreeMemSize(root);
		frameCounter = 0;
		fpsTimer = 0;
	}

	//rain->SetDirection(Vector3(msec, 0.0f, msec));
	
	//light->SetPosition(Vector3(light->GetPosition().x + (msec / 10.0f), 650, light->GetPosition().z + (msec / 20.0f)));
	
}

void Renderer::DrawHeightMap()
{
	

	SetCurrentShader(sceneShader);
	SetShaderLight(*light);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),  "diffuseTex"), 0);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos" ), 1,
				(float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),  "bumpTex"   ), 1);
	
	Matrix4 tempMatrix = shadowMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1,
						false, *&tempMatrix.values);

	modelMatrix.ToIdentity();
	//textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	heightMap->Draw();

	glDisable(GL_CULL_FACE);

	glUseProgram(0);
}

void Renderer::DrawSkybox()
{
	
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	quad->Draw();
	glDisable(GL_CULL_FACE);
	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawWater()
{
	
	SetCurrentShader(reflectShader);//reflectShader
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos" ), 1, 
				(float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),  "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),  "cubeTex"   ), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),  "bumpTex"   ), 1);

	Matrix4 tempMatrix = shadowMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1,
						false, *&tempMatrix.values);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 3.0f;
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix = 
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) * 
		Matrix4::Scale(Vector3(heightX, heightY, heightZ)) * 
		Matrix4:: Rotation(90, Vector3(1.0f,0.0f,0.0f));

	textureMatrix = 
		Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();
	quad->Draw();
	textureMatrix = Matrix4();
		
	glUseProgram(0);
}

//more shadow stuff

void Renderer::DrawShadowScene()
{
		
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glViewport(0,0,SHADOWSIZE, SHADOWSIZE);
	
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	
	SetCurrentShader(sceneShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)));
	projMatrix = Matrix4::Perspective(100.0f, 15000.0f, (float)width / (float)height, 45.0f);
	shadowMatrix = biasMatrix*(projMatrix*viewMatrix);

	UpdateShaderMatrices();
	//DrawWater();
	DrawHeightMap();
	DrawTree(root);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0,0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene()
{
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1,
				(float*)&camera->GetPosition());
	SetShaderLight(*light);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	

	viewMatrix = camera->BuildViewMatrix();
	//viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f))); //rich did this
	UpdateShaderMatrices();
	
	DrawSkybox();
	DrawHeightMap();
	DrawWater();
	DrawTree(root);
	DrawGrass(grassNode);
	//DrawParticleEmitter(rain);
	
	glUseProgram(0);

}


void Renderer::DrawTree(SceneNode* n)
{
	if(n->GetMesh())
	{
		if(n->GetFlag() == true)
		{
			SetCurrentShader(particleShader);
			DrawParticleEmitter(emitter);
		}else
		{
			SetCurrentShader(sceneShader);
		}
		SetShaderLight(*light);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),  "diffuseTex"), 0);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos" ), 1,
					(float*)&camera->GetPosition());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),  "bumpTex"   ), 1);
	
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		

		Matrix4 tempMatrix = shadowMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1,
						false, *&tempMatrix.values);

		//modelMatrix.ToIdentity();
		
		UpdateShaderMatrices();

		n->Draw(*this);
	}
	
	for(vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i)
	{
		DrawTree(*i);
	}

	glUseProgram(0);
	
	
}

void Renderer::DrawGrass(SceneNode* n)
{
	if(n->GetMesh())
	{
		SetCurrentShader(sceneShader);
		SetShaderLight(*light);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),  "diffuseTex"), 0);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos" ), 1,
					(float*)&camera->GetPosition());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),  "bumpTex"   ), 1);
	
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		

		Matrix4 tempMatrix = shadowMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1,
						false, *&tempMatrix.values);
		n->Draw(*this);
	}
	
	for(vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i)
	{
		DrawTree(*i);
	}

	glUseProgram(0);
	
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, float val)
{
	SetCurrentShader(textShader);
	string s = text + to_string((int)val);
	TextMesh* m = new TextMesh(s, *basicFont);

	modelMatrix = Matrix4::Translation(Vector3(position.x, height-position.y,position.z)) * Matrix4::Scale(Vector3(size,size,1));
	viewMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	textureMatrix.ToIdentity();



	UpdateShaderMatrices();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	m->Draw();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glDisable(GL_BLEND);
	delete m;

}

int Renderer::GetTreeMemSize(SceneNode * node)
{

	float size = 0;
	
	size += sizeof(Tree);
	size += sizeof(tree);
	size += sizeof(leaf);
	size += sizeof(tree->numVertices) * sizeof(Vector3);
	size += sizeof(tree->numVertices) * sizeof(Vector2);
	size += sizeof(tree->numVertices) * sizeof(Vector4);
	size += sizeof(tree->numIndices) * sizeof(tree->indices);
	size += sizeof(tree->GetTexture());

	for(vector<SceneNode*>::const_iterator i = node->GetChildIteratorStart(); i != node->GetChildIteratorEnd(); ++i)
	{
		size += (int)sizeof*(*i);
		size += sizeof(BranchNode);
		size += GetTreeMemSize((*i));

	}
	return size;

}

void Renderer::ResetScene()
{
	delete root;
	root = new SceneNode();
	root->AddChild(new Tree(tree, leaf,emitter,particleShader, RAW_HEIGHT*HEIGHTMAP_X /2.0f, 135, RAW_WIDTH*HEIGHTMAP_X / 2.0f));
}


void Renderer::DrawParticleEmitter(ParticleEmitter* p)
{
//	return;
		//glDisable(GL_BLEND);
	SetCurrentShader(particleShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	//modelMatrix.ToIdentity();
	//viewMatrix.ToIdentity();
	SetShaderParticleSize(p->GetParticleSize());
	p->SetParticleSize(8.0f);
	p->SetParticleVariance(1.0f);
	p->SetLaunchParticles(16.0f);
	p->SetParticleLifetime(2000.0f);
	p->SetParticleSpeed(0.1f);
	UpdateShaderMatrices();

	p->Draw();
	
	glUseProgram(0);
	//SetCurrentShader(sceneShader);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::SetShaderParticleSize(float f)
{
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}

void Renderer::SetUpPostProcess()
{
	//generate scene depth texture
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);


	//colour texture
	for(int i = 0; i < 2; ++i)
	{
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO); // render scene into this.
	glGenFramebuffers(1, &processFBO); // post process into this

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	//check FBO attachment success
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0])
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1,1,1,-1,-1,1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	for(int i = 0; i < POST_PASSES; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		postProcessQuad->SetTexture(bufferColourTex[0]);

		postProcessQuad->Draw();
	
		//swap colour buffers, and second blur pass
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		postProcessQuad->SetTexture(bufferColourTex[1]);
		
		postProcessQuad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Orthographic(-1,1,1,-1,-1,1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	postProcessQuad->SetTexture(bufferColourTex[0]);
	postProcessQuad->Draw();
	glUseProgram(0);
}