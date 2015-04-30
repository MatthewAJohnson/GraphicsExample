#pragma once
#include "../../nclgl/ParticleEmitter.h"
#include "../../nclgl/HeightMap.h"
class Weather : public ParticleEmitter
{
public:
	Weather();
	virtual	~Weather(){};

	virtual void Update(float msec);

	Particle* GetFreeParticle();
};