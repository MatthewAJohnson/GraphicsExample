#include "Weather.h"

Weather::Weather() : ParticleEmitter()
{

}

void Weather::Update(float msec)
{
	ParticleEmitter::Update(msec);
}

Particle* Weather::GetFreeParticle()
{
	Particle* p = NULL;

	
	if(freeList.size() > 0) {
		p = freeList.back();
		freeList.pop_back();
	}
	else{ 
		
		p = new Particle();
	}



	p->colour		= Vector4(1,1,1,1.0);
	p->direction	= Vector3(0.0f,-1.0f,0.0f);
	p->direction.x += (0.0f);
	p->direction.y += (-1.0f);
	p->direction.z += (0.0f);

	p->direction.Normalise();	
	//p->position.ToZero();
	int xMax = HEIGHTMAP_X;
	p->position.x = (int)rand() % (xMax * 16);
	p->position.y = 1500;
	int zMax = HEIGHTMAP_Z;
	p->position.z = (int)rand() % (zMax * 16);
	return p;	
}