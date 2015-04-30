#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/TextMesh.h"
#include "../../nclgl/ParticleEmitter.h"
#include "Tree.h"
#include "Weather.h"
#include "Grass.h"
#define SHADOWSIZE 8192
#define POST_PASSES 5

class Renderer : public OGLRenderer
{
public:
	Renderer(Window &parent);
	virtual ~Renderer();

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void ResetScene();

protected:
	void DrawHeightMap();
	void DrawWater();
	void DrawSkybox();
	
	void DrawTree(SceneNode* n);
	void DrawParticleEmitter(ParticleEmitter* p);
	void SetShaderParticleSize(float f);
	void DrawGrass(SceneNode*n);

	int GetTreeMemSize(SceneNode * node);

	void DrawShadowScene();
	void DrawCombinedScene();
	void DrawPostProcess();
	void PresentScene();

	void SetUpPostProcess();
		
	void DrawText(const std::string &text, const Vector3 &position, const float size, float val);

	Camera* camera;
	HeightMap* heightMap;
	
	Light* light;
		
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* sceneShader;
	Shader* textShader;
	Shader* particleShader;
	Shader* processShader;

	GLuint cubeMap;
	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;


	float waterRotate;
	Mesh* quad;
	Mesh* tree;
	Mesh* leaf;
	Mesh* postProcessQuad;
	Mesh* grassQuad;
	SceneNode* root;
	SceneNode* grassNode;
	Font* basicFont;

	float fpsTimer;
	float fps;
	int frameCounter;
	float memCount;

	ParticleEmitter* emitter;
	Weather* rain;


};