#pragma once

#include "Engine/Module/ParticleSystem/Particle/Movements/BaseParticleMovements.h"

class BaseEnemy;
class BeatEmitter;
class BeatManager;

class BeatParticleMvements : public BaseParticleMovements {
public: // Member function
	std::unique_ptr<BaseParticleMovements> clone() override;
	void initialize(Particle* const particle) override;
	void move(Particle* const particle) override;

public:
	void set_next_destroy() { isDestroy = true; };
	void set_emit_enemy(BaseEnemy* enemy) { emitEnemy = enemy; };

private:
	bool isDestroy;
	float timer;
	Vector3 velocity;
	float lifeTime;

	BaseEnemy* emitEnemy;

public:
	inline static BeatManager* beatManager = nullptr;
};
