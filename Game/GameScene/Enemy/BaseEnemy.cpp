#include "BaseEnemy.h"

#include <Engine/Application/WorldClock/WorldClock.h>

void BaseEnemy::initialize() {
	hitpoint = 10;
	behavior.initalize(EnemyBehavior::Spawn);
	behavior.add_list(
		EnemyBehavior::Spawn,
		std::bind(&BaseEnemy::spawn_initialize, this),
		std::bind(&BaseEnemy::spawn_update, this)
	);
	behavior.add_list(
		EnemyBehavior::Approach,
		std::bind(&BaseEnemy::approach_initialize, this),
		std::bind(&BaseEnemy::approach_update, this)
	);
	behavior.add_list(
		EnemyBehavior::Attack,
		std::bind(&BaseEnemy::attack_initialize, this),
		std::bind(&BaseEnemy::attack_update, this)
		);
	behavior.add_list(
		EnemyBehavior::Beating,
		std::bind(&BaseEnemy::beating_initialize, this),
		std::bind(&BaseEnemy::beating_update, this)
	);
	behavior.add_list(
		EnemyBehavior::Damaged,
		std::bind(&BaseEnemy::damaged_initialize, this),
		std::bind(&BaseEnemy::damaged_update, this)
	);
	behavior.add_list(
		EnemyBehavior::Despawn,
		std::bind(&BaseEnemy::despawn_initialize, this),
		std::bind(&BaseEnemy::despawn_update, this)
	);
	reset_object("Sphere.obj");
}

void BaseEnemy::update() {
	behavior.update();
}

void BaseEnemy::do_beat() {
	behavior.behavior_request(EnemyBehavior::Beating);
}

// ---------- スポーン処理 ----------
void BaseEnemy::spawn_initialize() {
	behaviorValue = SpwanBehaviorWork{
		{ [&] { behavior.behavior_request(EnemyBehavior::Approach); }, 3 }
	};
}

void BaseEnemy::spawn_update() {
	SpwanBehaviorWork& value = std::get<SpwanBehaviorWork>(behaviorValue);
	value.timedCall.update();
}

// ---------- 移動処理 ----------
void BaseEnemy::approach_initialize() {
	behaviorValue = ApproachBehaviorWork{
		1.0f, 3.0f
	};
}

void BaseEnemy::approach_update() {
	if (!targetPlayer) {
		return;
	}
	ApproachBehaviorWork& value = std::get<ApproachBehaviorWork>(behaviorValue);
	// プレイヤーとの距離を算出
	Vector3 distance = targetPlayer->world_position() - world_position();

	// distanceより近かったら攻撃に移行
	if (distance.length() <= value.attackDistance) {
		behavior.behavior_request(EnemyBehavior::Attack);
		return;
	}

	// velocity算出
	Vector3 velocity = distance.normalize_safe() * value.speed;
	transform.plus_translate(velocity * WorldClock::DeltaSeconds());
	// player方向を向く
	//look_at(*targetPlayer);
}

// ---------- 攻撃処理 ----------
void BaseEnemy::attack_initialize() {
	auto&& collider = std::make_shared<SphereCollider>();
	collider->get_hierarchy().set_parent(hierarchy);
	behaviorValue = AttackBehaviorWork{
		collider,
		{ [&] { behavior.behavior_request(EnemyBehavior::Approach); }, 3 }
	};
}

void BaseEnemy::attack_update() {
	AttackBehaviorWork& value = std::get<AttackBehaviorWork>(behaviorValue);
	value.attackTimedCall.update();
}

// ---------- 鼓動同期時処理 ----------
void BaseEnemy::beating_initialize() {
	auto&& collider = std::make_shared<SphereCollider>();
	collider->get_hierarchy().set_parent(hierarchy);
	behaviorValue = BeatingBehaviorWork{
		collider
	};
}

void BaseEnemy::beating_update() {
	BeatingBehaviorWork& value = std::get<BeatingBehaviorWork>(behaviorValue);

	if (hitpoint <= 0) {
		behavior.behavior_request(EnemyBehavior::Despawn);
	}
}

// ---------- 被ダメージ時処理 ----------
void BaseEnemy::damaged_initialize() {
	behaviorValue = DamagedBehaviorWork{
		{ [&] { behavior.behavior_request(EnemyBehavior::Approach); }, 3 }
	};

}

void BaseEnemy::damaged_update() {
	DamagedBehaviorWork& value = std::get<DamagedBehaviorWork>(behaviorValue);
	value.damagedTimedCall.update();

	if (hitpoint <= 0) {
		behavior.behavior_request(EnemyBehavior::Despawn);
	}
}

// ---------- 死亡時処理 ----------
void BaseEnemy::despawn_initialize() {
	behaviorValue = DespawnBehaviorWork{
		{ [&] { isDead = true; }, 3 }
	};
}

void BaseEnemy::despawn_update() {
	DespawnBehaviorWork& value = std::get<DespawnBehaviorWork>(behaviorValue);
	value.despawnTimedCall.update();
}
