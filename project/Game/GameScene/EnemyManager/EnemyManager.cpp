#include "EnemyManager.h"

#include <Engine/Module/Collision/CollisionManager.h>
#include <Engine/Utility/SmartPointer.h>

#include "../Enemy/ReviveParticle/ReviveEmitter.h"
#include "../Enemy/ReviveParticle/ReviveParticleMvements.h"

void EnemyManager::initialize() {
	static GlobalValues& globalValues = GlobalValues::GetInstance();
	globalValues.add_value<int>("ReviveEmitter", "EmitsNum", 2);
	globalValues.add_value<float>("ReviveEmitter", "EmitterDuration", 0.3f);
	globalValues.add_value<float>("ReviveEmitter", "EmitTime", 0.15f);
	globalValues.add_value<float>("ReviveEmitter", "EmitsRadius", 1.0f);
	BaseEnemy::enemyManager = this;
}

void EnemyManager::begin() {
	for (BaseEnemy& enemy : enemies) {
		enemy.begin();
	}
}

void EnemyManager::update() {
	for (BaseEnemy& enemy : enemies) {
		enemy.update();
	}

	for (ParticleSystemModel& particleSystem : reviveParticleSystems) {
		particleSystem.update();
	}

	enemies.remove_if([&](const BaseEnemy& enemy) {return !enemy.is_active(); });

	std::erase_if(reviveParticleSystems,
		[](ParticleSystemModel& particleSystem) {
		return particleSystem.is_end_all();
	});
}

void EnemyManager::begin_rendering() {
	for (BaseEnemy& enemy : enemies) {
		enemy.begin_rendering();
	}

	for (ParticleSystemModel& particleSystem : reviveParticleSystems) {
		particleSystem.begin_rendering();
	}
}

void EnemyManager::draw() const {
	for (const BaseEnemy& enemy : enemies) {
		enemy.draw();
	}
}

void EnemyManager::draw_particle() const {
	for (const ParticleSystemModel& particleSystem : reviveParticleSystems) {
		particleSystem.draw();
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

void EnemyManager::create_revive_effect(const BaseEnemy* enemy) {
	auto& newParticleSystem = reviveParticleSystems.emplace_back();
	newParticleSystem.initialize(16);

	auto&& emitter = eps::CreateUnique<ReviveEmitter>();
	if (enemy) {
		emitter->get_transform().set_translate(enemy->world_position());
	}

	auto&& movements = eps::CreateUnique<ReviveParticleMvements>();

	newParticleSystem.set_mesh("EnemyRevive.obj");
	newParticleSystem.set_emitter(std::move(emitter));
	newParticleSystem.set_particle_movements(std::move(movements));
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

	if (ImGui::Button("Emit")) {
		create_revive_effect(nullptr);
	}
	ImGui::End();
}
#endif // _DEBUG
