#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	triangle = Mesh::GenerateTriangle();

	currentShader = new Shader(SHADERDIR"basicVertex.glsl",
		SHADERDIR"colourFragment.glsl");

	if(!currentShader->LinkProgram()) {
		return;
	}

	init = true;
}
Renderer::~Renderer(void)	{
	delete triangle;
}

void Renderer::RenderScene()	{
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);	

	Vector4* vec = (Vector4*) triangle->mapBuffer(COLOUR_BUFFER);
	vec[0] = Vector4((rand() % 100) / 100.0f,(rand() % 100) / 100.0f,(rand() % 100) / 100.0f,1);
	vec[1] = Vector4((rand() % 100) / 100.0f,(rand() % 100) / 100.0f,(rand() % 100) / 100.0f,1);
	vec[2] = Vector4((rand() % 100) / 100.0f,(rand() % 100) / 100.0f,(rand() % 100) / 100.0f,1);

	triangle->unMapBuffer();
	glUseProgram(currentShader->GetProgram());
	triangle->Draw();
	glUseProgram(0);
	
	SwapBuffers();	
}

