#pragma once

#include <unordered_map>
#include <memory>
#include <list>

#include <Engine/Module/ParticleSystem/ParticleSystemModel.h>

class BaseEnemy;
class PlayerBullet;
class BaseCollider;
class Particle;
class SphereCollider;
class CollisionManager;
class BeatParticleMvements;

class BeatManager {
private:
	struct BeatPair {
		BaseEnemy* enemy;
		PlayerBullet* heart;
	};

	struct BeatColliderInvolved {
		BaseEnemy* emitEnemy;
		Particle* parentParticle;
		BeatParticleMvements* movements;
	};

public:
	void initalize();
	void update();
	void begin_rendering();
	void draw() const;

public:
	void set_next_enemy(BaseEnemy* enemy);
	void set_next_heart(PlayerBullet* heart);
	bool empty_pair();

	void start_beat();
	void beating();
	void pause_beat();
	void enemy_down(BaseEnemy* enemy);
	void recovery(BaseEnemy* enemy);

public:
	bool is_self_particle(BaseEnemy* const enemy, const BaseCollider* const other);
	void unregister_particle(const ParticleSystemModel& particleSystem);
	void make_collider(Particle* const particle, BaseEnemy* const enemy, BeatParticleMvements* const movements_);
	void destroy_collider(const Particle* particle);

private:
	void check_make_pair();

private:
	void particle_on_collision(
		const BaseCollider* const other, 
		SphereCollider* thisCollider,
		const BaseCollider* const enemyCollider
	);

private:
	BeatPair nextPair;

	std::unordered_multimap<BaseEnemy*, PlayerBullet*> enemyBeatPair;
	std::unordered_map<PlayerBullet*, BaseEnemy*> heartBeatPair;

	std::list<ParticleSystemModel> particleSystems;

	std::unordered_map<const Particle*, std::shared_ptr<SphereCollider>> beatCollider;
	std::unordered_map<const BaseCollider*, BeatColliderInvolved> involeder;

public:
	inline static CollisionManager* collisionManager = nullptr;
};
