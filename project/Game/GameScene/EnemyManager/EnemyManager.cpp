#include "EnemyManager.h"

#include <Engine/Module/Collision/CollisionManager.h>

void EnemyManager::begin() {
	for (BaseEnemy& enemy : enemies) {
		enemy.begin();
	}
}

void EnemyManager::update() {
	for (BaseEnemy& enemy : enemies) {
		enemy.update();
	}

	enemies.remove_if([&](const BaseEnemy& enemy) {return !enemy.is_active(); });
}

void EnemyManager::begin_rendering() {
	for (BaseEnemy& enemy : enemies) {
		enemy.begin_rendering();
	}
}

void EnemyManager::draw() const {
	for (const BaseEnemy& enemy : enemies) {
		enemy.draw();
	}
}

void EnemyManager::draw_marker() const {
	for (const BaseEnemy& enemy : enemies) {
		enemy.draw_marker();
	}
}

const std::list<BaseEnemy>& EnemyManager::get_enemies() {
	return enemies;
}

void EnemyManager::set_collision_manager(CollisionManager* collisionManager_) {
	collisionManager = collisionManager_;
}

void EnemyManager::create_enemy(const Vector3& position, const Vector3& forward, BaseEnemy::Type type) {
	auto& newEnemy = enemies.emplace_back();
	newEnemy.initialize(position, forward, type);
	collisionManager->register_collider("EnemyHit", newEnemy.get_hit_collider());
	collisionManager->register_collider("Beat", newEnemy.get_beat_collider());
	collisionManager->register_collider("EnemyMelee", newEnemy.get_melee_collider());
}

#ifdef _DEBUG
void EnemyManager::clear() {
	enemies.clear();
}

#include <imgui.h>

void EnemyManager::debug_gui() {
	ImGui::Begin("EnemyManager");
	for (int i = 0; BaseEnemy & enemy : enemies) {
		ImGui::Text("[%02d] HP : %d, State : %d", i, enemy.get_hp(), enemy.get_state());
		++i;
	}
	ImGui::End();
}
#endif // _DEBUG
