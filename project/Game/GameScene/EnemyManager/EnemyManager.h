#pragma once

#include <list>

#include "Game/GameScene/Enemy/BaseEnemy.h"

class CollisionManager;

class EnemyManager {
public:
	EnemyManager() = default;
	~EnemyManager() = default;

	EnemyManager(const EnemyManager&) = delete;
	EnemyManager& operator=(const EnemyManager&) = delete;

public:
	void update();
	void begin_rendering();
	void draw() const;

public:
	const std::list<BaseEnemy>& get_enemies();
	void set_collision_manager(CollisionManager* collisionManager_);
	void create_enemy(const Vector3& position, const Vector3& forward);

private:
	std::list<BaseEnemy> enemies;
	CollisionManager* collisionManager;
};
