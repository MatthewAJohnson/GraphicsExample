#pragma once

#include "../../nclgl/SceneNode.h"

class Grass : public SceneNode
{
public:
	Grass(Mesh* m, float x, float y, float z, int totalGrass);
	virtual  ~Grass();

	virtual void Update(float msec);

	void AddGrass(Mesh* m, float x, float y, float z, int totalGrass, SceneNode* parent);
protected:
	SceneNode* root;
	float currentScale;
};