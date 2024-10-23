#pragma once

#include "Engine/Module/ParticleSystem/Particle/Movements/BaseParticleMovements.h"

class ReviveParticleMvements : public BaseParticleMovements {
public: // Member function
	std::unique_ptr<BaseParticleMovements> clone() override;
	void initialize(Particle* const particle) override;
	void move(Particle* const particle) override;

public:

private:
	float timer;
	float lifeTime;

	Vector3 velocity;
};
