#pragma once

#include <list>

#include <Engine/Module/ParticleSystem/ParticleSystemModel.h>

#include "Game/GameScene/Enemy/BaseEnemy.h"

class CollisionManager;

class EnemyManager {
public:
	EnemyManager() = default;
	~EnemyManager() = default;

	EnemyManager(const EnemyManager&) = delete;
	EnemyManager& operator=(const EnemyManager&) = delete;

public:
	void initialize();

	void begin();
	void update();
	void begin_rendering();
	void draw() const;
	void draw_particle() const;
	void draw_marker() const;

public:
	const std::list<BaseEnemy>& get_enemies();
	void set_collision_manager(CollisionManager* collisionManager_);
	void create_enemy(const Vector3& position, const Vector3& forward, BaseEnemy::Type type);
	void create_revive_effect(const BaseEnemy* enemy);

#ifdef _DEBUG
public:
	void clear();
	void debug_gui();
#endif // _DEBUG

private:
	std::list<BaseEnemy> enemies;

	std::list<ParticleSystemModel> reviveParticleSystems;
	CollisionManager* collisionManager;
};
