#include "BeatManager.h"

#include "Game/GameScene/Player/PlayerBullet.h"
#include "Game/GameScene/Enemy/BaseEnemy.h"

#include <Engine/Utility/SmartPointer.h>
#include <Engine/Module/Collision/Collider/SphereCollider.h>
#include <Engine/Module/ParticleSystem/Particle/Particle.h>
#include <Engine/Module/Collision/CollisionManager.h>

#include "BeatParticleSystem/BeatEmitter.h"
#include "BeatParticleSystem/BeatParticleMvements.h"

#include <ranges>

void BeatManager::initalize() {
	BeatParticleMvements::beatManager = this;
}

void BeatManager::update() {
	for (ParticleSystemModel& particleSystem : particleSystems) {
		particleSystem.update();
	}

	std::erase_if(beatCollider,
		[](std::pair<const Particle* const, std::shared_ptr<SphereCollider>>& collider) {
		return !collider.second->is_active();
	}
	);

	std::erase_if(particleSystems,
		[&](ParticleSystemModel& ps) {
		if (ps.is_end_all()) {
			unregister_particle(ps);
			ps.finalize();
			return true;
		}
		return false;
	});
}

void BeatManager::begin_rendering() {
	for (ParticleSystemModel& particleSystem : particleSystems) {
		particleSystem.begin_rendering();
	}
}

void BeatManager::draw() const {
	for (const ParticleSystemModel& particleSystem : particleSystems) {
		particleSystem.draw();
	}
}

void BeatManager::set_next_enemy(BaseEnemy* enemy) {
	nextPair.enemy = enemy;
	check_make_pair();
}

void BeatManager::set_next_heart(PlayerBullet* heart) {
	nextPair.heart = heart;
	check_make_pair();
}

bool BeatManager::empty_pair() {
	return enemyBeatPair.empty();
}

void BeatManager::start_beat() {
	for (auto& pair : enemyBeatPair) {
		pair.first->start_beat();
		pair.second->BeatAttack();
	}
}

void BeatManager::beating() {
	BaseEnemy* prev = nullptr;
	for (BaseEnemy* enemy : enemyBeatPair | std::views::elements<0>) {
		// 重複してびーとさせないようにする
		if (enemy == prev) {
			continue;
		}
		enemy->beating();
		prev = enemy;

		// エミッター初期化
		auto&& emitter = eps::CreateUnique<BeatEmitter>();
		emitter->get_transform().set_translate(enemy->world_position() + Vector3{ 0,1.0f,0.0f });
		emitter->update_matrix();
		// Mvements初期化
		auto&& movement = eps::CreateUnique<BeatParticleMvements>();
		movement->set_emit_enemy(enemy);
		// パーティクル追加
		ParticleSystemModel& newParticleSystem = particleSystems.emplace_back();
		newParticleSystem.initialize(32);
		newParticleSystem.set_mesh("beat-particle.obj");
		newParticleSystem.set_emitter(std::move(emitter));
		newParticleSystem.set_particle_movements(std::move(movement));
	}
}

void BeatManager::pause_beat() {
	for (auto& pair : enemyBeatPair) {
		pair.first->pause_beat();
		pair.second->StopBeat();
	}
}

// 敵が気絶した場合に呼び出す
void BeatManager::enemy_down(BaseEnemy* enemy) {
	if (enemyBeatPair.contains(enemy)) {
		// 削除するポインタの取得
		auto eraseHearts = enemyBeatPair.equal_range(enemy);
		for (auto& itr = eraseHearts.first; itr != eraseHearts.second; ++itr) {
			PlayerBullet*& eraseHeart = itr->second;
			// 止める
			eraseHeart->ComeBack();
			// 登録解除
			heartBeatPair.erase(eraseHeart);
		}
		enemyBeatPair.erase(enemy);
	}
}

// 保持されて10秒経過したら呼び出す
void BeatManager::recovery(BaseEnemy* enemy) {
	if (enemyBeatPair.contains(enemy)) {
		// 削除するポインタの取得
		auto eraseHearts = enemyBeatPair.equal_range(enemy);
		for (auto& itr = eraseHearts.first; itr != eraseHearts.second; ++itr) {
			PlayerBullet*& eraseHeart = itr->second;
			// 削除
			eraseHeart->lost();
			// 登録解除
			heartBeatPair.erase(eraseHeart);
		}
		enemyBeatPair.erase(enemy);
	}
}

bool BeatManager::is_self_particle(BaseEnemy* const enemy, const BaseCollider* const other) {
	bool result = false;
	if (involeder.contains(other)) {
		result = involeder.at(other).emitEnemy == enemy;
	}
	return result;
}

void BeatManager::unregister_particle(const ParticleSystemModel& particleSystem) {
	auto& particles = particleSystem.get_particles();
	for (auto& particle : particles) {
		auto& collider = beatCollider.at(&particle);
		involeder.erase(collider.get());
		beatCollider.erase(&particle);
	}
}

void BeatManager::make_collider(Particle* const particle, BaseEnemy* const enemy, BeatParticleMvements* const movements_) {
	auto&& newCollider = eps::CreateShared<SphereCollider>();
	auto&& newInvoleder = involeder.emplace(newCollider.get(), BeatColliderInvolved{ enemy, particle , movements_ });
	newCollider->set_parent(*particle);
	newCollider->set_on_collision_enter(
		std::bind(
			&BeatManager::particle_on_collision,
			this,
			std::placeholders::_1,
			newCollider.get(),
			enemy->get_hit_collider().lock().get()
		)
	);
	newCollider->set_radius(0.5f);
	collisionManager->register_collider("BeatParticle", newCollider);

	beatCollider.emplace(particle, newCollider);
}

void BeatManager::destroy_collider(const Particle* particle) {
	auto&& destroyCollider = beatCollider.at(particle);
	involeder.erase(destroyCollider.get());
	beatCollider.erase(particle);
}

void BeatManager::check_make_pair() {
	if (nextPair.enemy && nextPair.heart) {
		enemyBeatPair.emplace(nextPair.enemy, nextPair.heart);
		heartBeatPair.emplace(nextPair.heart, nextPair.enemy);

		nextPair.enemy = nullptr;
		nextPair.heart = nullptr;
	}
}

void BeatManager::particle_on_collision(const BaseCollider* const other, SphereCollider* thisCollider, const BaseCollider* const enemyCollider) {
	if (other == enemyCollider) {
		return;
	}
	thisCollider->set_active(false);
	involeder.at(thisCollider).movements->set_next_destroy();
}
