#include "BaseEnemy.h"

#include <Engine/Application/GameTimer/GameTimer.h>

void BaseEnemy::initialize() {
	hitpoint = 10;
	behaviorRequest = EnemyBehavior::Spawn;
	behaviorInitFunc = {
		std::bind(&BaseEnemy::spawn_initialize, this),
		std::bind(&BaseEnemy::approach_initialize, this),
		std::bind(&BaseEnemy::attack_initialize, this),
		std::bind(&BaseEnemy::beating_initialize, this),
		std::bind(&BaseEnemy::damaged_initialize, this),
		std::bind(&BaseEnemy::despawn_initialize, this),
	};
	behaviorUpdateFunc = {
		std::bind(&BaseEnemy::spawn_update, this),
		std::bind(&BaseEnemy::approach_update, this),
		std::bind(&BaseEnemy::attack_update, this),
		std::bind(&BaseEnemy::beating_update, this),
		std::bind(&BaseEnemy::damaged_update, this),
		std::bind(&BaseEnemy::despawn_update, this),
	};
	reset_object("Sphere.obj");
}

void BaseEnemy::update() {
	// リクエストが値があるなら遷移させる
	if (behaviorRequest.has_value()) {
		behavior = std::move(behaviorRequest.value());
		// 初期化処理
		behaviorInitFunc[static_cast<int32_t>(behavior)]();
		behaviorRequest = std::nullopt;
	}

	// 更新処理
	behaviorUpdateFunc[static_cast<int32_t>(behavior)]();
}

void BaseEnemy::do_beat() {
	behaviorRequest = EnemyBehavior::Beating;
}

// ---------- スポーン処理 ----------
void BaseEnemy::spawn_initialize() {
	behaviorValue = SpwanBehaviorWork{
		{ [&] { behaviorRequest = EnemyBehavior::Approach; }, 3 }
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
	//Vector3 distance = targetPlayer->world_position() - world_position();
	Vector3 distance = CVector3::BASIS_Z * 3;

	// distanceより近かったら攻撃に移行
	if (distance.length() <= value.attackDistance) {
		behaviorRequest = EnemyBehavior::Attack;
		return;
	}

	// velocity算出
	Vector3 velocity = distance.normalize_safe() * value.speed;
	transform->plus_translate(velocity * GameTimer::DeltaTime());
	// player方向を向く
	//look_at(*targetPlayer);
}

// ---------- 攻撃処理 ----------
void BaseEnemy::attack_initialize() {
	auto&& collider = std::make_shared<SphereCollider>();
	collider->get_hierarchy().set_parent(*hierarchy);
	behaviorValue = AttackBehaviorWork{
		collider,
		{ [&] { behaviorRequest = EnemyBehavior::Approach; }, 3 }
	};
}

void BaseEnemy::attack_update() {
	AttackBehaviorWork& value = std::get<AttackBehaviorWork>(behaviorValue);
	value.attackTimedCall.update();
}

// ---------- 鼓動同期時処理 ----------
void BaseEnemy::beating_initialize() {
	auto&& collider = std::make_shared<SphereCollider>();
	collider->get_hierarchy().set_parent(*hierarchy);
	behaviorValue = BeatingBehaviorWork{
		collider
	};
}

void BaseEnemy::beating_update() {
	BeatingBehaviorWork& value = std::get<BeatingBehaviorWork>(behaviorValue);

	if (hitpoint <= 0) {
		behaviorRequest = EnemyBehavior::Despawn;
	}
}

// ---------- 被ダメージ時処理 ----------
void BaseEnemy::damaged_initialize() {
	behaviorValue = DamagedBehaviorWork{
		{ [&] { behaviorRequest = EnemyBehavior::Approach; }, 3 }
	};

}

void BaseEnemy::damaged_update() {
	DamagedBehaviorWork& value = std::get<DamagedBehaviorWork>(behaviorValue);
	value.damagedTimedCall.update();

	if (hitpoint <= 0) {
		behaviorRequest = EnemyBehavior::Despawn;
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
