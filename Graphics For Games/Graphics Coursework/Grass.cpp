#include "Grass.h"

Grass::Grass(Mesh* m, float x, float y, float z, int totalGrass)
{
	root = new SceneNode(m, Vector4(0,1,0,1));

	root->SetMesh(m);
	root->SetModelScale(Vector3(1,1,1));
	root->SetTransform(Matrix4::Translation(Vector3(x,y,z)));
	AddChild(root);
	AddGrass(m, x,y,z, 10, root);
	 currentScale = 0;
	Update(0);
}

Grass::~Grass()
{
	delete root;
}

void Grass::Update(float msec)
{
	float actualSize;
	float target = 100; 
	if(currentScale < target)
	{
		currentScale += msec * 0.001;
	}
	else 
	{
		currentScale = target;
	}
	actualSize = max(currentScale, 0);
	SetModelScale(Vector3(actualSize,actualSize,actualSize));
	SceneNode::Update(msec);
}

void Grass::AddGrass(Mesh* m, float x, float y, float z, int totalGrass, SceneNode* parent)
{
	float distFromOrigin = rand() % 20;

	//Grass* newGrass = new Grass(m,x,y,z,totalGrass-1);
	SceneNode* newGrass = new SceneNode(m, Vector4(1,1,1,1));
	if((int)totalGrass%2 == 0)
	{
		newGrass->SetTransform(Matrix4::Translation(Vector3(x, y, z + distFromOrigin)));
	}
	else
	{
		newGrass->SetTransform(Matrix4::Translation(Vector3(x + distFromOrigin, y, z )));
	}
	parent->AddChild(newGrass);

	--totalGrass;
	if(totalGrass >= 0)
	{
		AddGrass(m,x,y,z,totalGrass,parent);
		//return;
	}


}