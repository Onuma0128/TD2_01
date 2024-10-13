#include "BaseEnemy.h"

#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Utility/SmartPointer.h>

#include "Game/GameScene/Player/PlayerBullet.h"
#include "Game/GameScene/Player/PlayerHPManager.h"
#include "Game/GameScene/BeatManager/BeatManager.h"

void BaseEnemy::initialize(const Vector3& translate, const Vector3& forward) {
	globalValues.add_value<int>("Heart", "AttackDamage", 30);

	globalValues.add_value<int>("Enemy", "HP", 100);
	globalValues.add_value<float>("Enemy", "BeatInterval", 1.0f);
	globalValues.add_value<int>("Enemy", "BeatingDamage", 20);
	globalValues.add_value<int>("Enemy", "BeatHitDamage", 5);

	globalValues.add_value<float>("Enemy", "NockbackMag", 3.0f);
	globalValues.add_value<float>("Enemy", "NockbackFriction", 0.8f);

	globalValues.add_value<float>("Enemy", "StartAttackDistance", 1.0f);
	globalValues.add_value<float>("Enemy", "ApproachSpeed", 3.0f);

	globalValues.add_value<float>("Enemy", "AbsorptionTime", 3.0f);
	globalValues.add_value<int>("Enemy", "AbsorptionAmount", 20);

	globalValues.add_value<float>("Enemy", "ToDeadDuration", 5.0f);

	markedCount = 0;
	maxHitpoint = globalValues.get_value<int>("Enemy", "HP");
	hitpoint = maxHitpoint;
	transform.set_translate(translate);
	transform.set_quaternion(Quaternion::LookForward(forward));
	// behavior初期化
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
		EnemyBehavior::DamagedHeart,
		std::bind(&BaseEnemy::damaged_heart_initialize, this),
		std::bind(&BaseEnemy::damaged_heart_update, this)
	);
	behavior.add_list(
		EnemyBehavior::DamagedBeat,
		std::bind(&BaseEnemy::damaged_beat_initialize, this),
		std::bind(&BaseEnemy::damaged_beat_update, this)
	);
	behavior.add_list(
		EnemyBehavior::Down,
		std::bind(&BaseEnemy::down_initialize, this),
		std::bind(&BaseEnemy::down_update, this)
	);
	behavior.add_list(
		EnemyBehavior::Revive,
		std::bind(&BaseEnemy::revive_initialize, this),
		std::bind(&BaseEnemy::revive_update, this)
	);
	behavior.add_list(
		EnemyBehavior::Erase,
		std::bind(&BaseEnemy::erase_initialize, this),
		std::bind(&BaseEnemy::erase_update, this)
	);

	ghostMesh = eps::CreateUnique<GameObject>("ghost_model.obj");
	ghostMesh->initialize();
	ghostMesh->set_parent(*this);
	ghostMesh->get_transform().set_translate({ 0,1,0 });

	hitCollider = eps::CreateShared<SphereCollider>();
	hitCollider->initialize();
	hitCollider->set_parent(*ghostMesh);
	hitCollider->set_radius(0.5);
	hitCollider->set_on_collision_enter(
		std::bind(&BaseEnemy::damaged_callback, this, std::placeholders::_1)
	);
	hitCollider->set_active(true);

	meleeCollider = eps::CreateShared<SphereCollider>();
	meleeCollider->initialize();
	meleeCollider->set_parent(*ghostMesh);
	meleeCollider->set_active(false);
	meleeCollider->set_radius(0.5);
	meleeCollider->set_on_collision_enter(
		std::bind(&BaseEnemy::attack_callback, this, std::placeholders::_1)
	);

	beatCollider = eps::CreateShared<SphereCollider>();
	beatCollider->initialize();
	beatCollider->set_parent(*this);
	beatCollider->set_active(false);
	beatCollider->set_radius(3.0f);
}

void BaseEnemy::update() {
	// 行動の更新
	behavior.update();

	// 付与状態かつビートでない場合
	if (markedCount && behavior.state() != EnemyBehavior::Beating) {
		markingTimer += WorldClock::DeltaSeconds();
	}

	// 付与状態でカウンタが達成した場合
	if (markingTimer >= globalValues.get_value<float>("Enemy", "AbsorptionTime")) {
		markingTimer = 0;
		beatManager->recovery(this);
		// 回復
		hitpoint += globalValues.get_value<int>("Enemy", "AbsorptionAmount") * markedCount;
		// 上限を超えないようにする
		hitpoint = std::min(maxHitpoint, hitpoint);
		markedCount = 0;
	}
}

void BaseEnemy::begin_rendering() {
	WorldInstance::update_matrix();
	ghostMesh->begin_rendering();
}

void BaseEnemy::draw() const {
	ghostMesh->draw();
}

// 被ダメ時コールバック
void BaseEnemy::damaged_callback(const BaseCollider* const other) {
	const std::string& group = other->group();
	// 心臓の場合
	if (group == "Heart") {
		// ダウン時は何もしない
		if (behavior.state() == EnemyBehavior::Down) {
			return;
		}
		hitpoint -= globalValues.get_value<int>("Heart", "AttackDamage");
		// マークされたのを記録
		++markedCount;
		// カウンタをリセット
		markingTimer = 0;
		// Parent情報から心臓を取得
		const PlayerBullet* playerBullet = dynamic_cast<const PlayerBullet*>(other->get_hierarchy().get_parent_address());
		// ノックバック方向の取得
		if (playerBullet) {
			Vector3 bulletVelocity = playerBullet->get_velocity();
			// 上下情報を切り捨て
			bulletVelocity.y = 0;
			velocity = bulletVelocity.normalize_safe() * globalValues.get_value<float>("Enemy", "NockbackMag");
		}
		else {
			velocity = CVector3::ZERO;
		}
		beatManager->set_next_enemy(this);
		// ビート状態の場合は遷移させない
		if (behavior.state() != EnemyBehavior::Beating) {
			// まだ生きてる場合
			if (hitpoint > 0) {
				behavior.request(EnemyBehavior::DamagedHeart);
			}
			// HPが0以下になった場合
			else {
				hitCollider->set_active(false);
				behavior.request(EnemyBehavior::Down);
			}
		}
	}
	else if (group == "Beat") {
		if (other == beatCollider.get()) {
			return;
		}
		// 復活処理
		if (behavior.state() == EnemyBehavior::Down) {
			behavior.request(EnemyBehavior::Revive);
			return;
		}
		hitpoint -= globalValues.get_value<int>("Enemy", "BeatHitDamage");
		// ビート状態だった場合はリアクションさせない
		if (behavior.state() != EnemyBehavior::Beating) {
			// まだ生きてる場合
			if (hitpoint > 0) {
				behavior.request(EnemyBehavior::DamagedBeat);
			}
			// HPが0以下になった場合
			else {
				hitCollider->set_active(false);
				behavior.request(EnemyBehavior::Down);
			}
		}
	}
}

// 攻撃時コールバック
void BaseEnemy::attack_callback(const BaseCollider* const other) {
	isAttakced = true;
	// コリジョンを無効化する
	meleeCollider->set_active(false);
}

void BaseEnemy::do_beat() {
	behavior.request(EnemyBehavior::Beating);
}

void BaseEnemy::pause_beat() {
	behavior.request(EnemyBehavior::Approach);
	beatCollider->set_active(false);
}

EnemyBehavior BaseEnemy::get_now_behavior() const {
	return behavior.state();
}

std::weak_ptr<SphereCollider> BaseEnemy::get_hit_collider() {
	return hitCollider;
}

std::weak_ptr<SphereCollider> BaseEnemy::get_beat_collider() {
	return beatCollider;
}

std::weak_ptr<SphereCollider> BaseEnemy::get_melee_collider() {
	return meleeCollider;
}

// ---------- スポーン処理 ----------
void BaseEnemy::spawn_initialize() {
	behaviorValue = SpwanBehaviorWork{
		{ [&] { behavior.request(EnemyBehavior::Approach); }, 3 }
	};
}

void BaseEnemy::spawn_update() {
	SpwanBehaviorWork& value = std::get<SpwanBehaviorWork>(behaviorValue);
	value.timedCall.update();
}

// ---------- 移動処理 ----------
void BaseEnemy::approach_initialize() {
	hitCollider->set_active(true);
	behaviorValue = ApproachBehaviorWork{
		globalValues.get_value<float>("Enemy", "StartAttackDistance"),
		globalValues.get_value<float>("Enemy", "ApproachSpeed")
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
		behavior.request(EnemyBehavior::Attack);
		return;
	}

	// velocity算出
	velocity = distance.normalize_safe() * value.speed;
	transform.plus_translate(velocity * WorldClock::DeltaSeconds());
	// player方向を向く
	look_at(*targetPlayer);
}

// ---------- 攻撃処理 ----------
void BaseEnemy::attack_initialize() {
	behaviorValue = AttackBehaviorWork{
	};
	isAttakced = false;
	meleeCollider->set_active(true);
}

void BaseEnemy::attack_update() {
	AttackBehaviorWork& value = std::get<AttackBehaviorWork>(behaviorValue);
	value.timer += WorldClock::DeltaSeconds();
	if (value.timer < 1.0f) {
		meleeCollider->set_active(false);
	}
	else if (value.timer < 1.5f) {
		meleeCollider->set_active(true);
	}
	else if (value.timer > 3.0f) {
		behavior.request(EnemyBehavior::Approach);
	}
}

// ---------- 鼓動同期時処理 ----------
void BaseEnemy::beating_initialize() {
	behaviorValue = BeatingBehaviorWork{
		0
	};
	beatCollider->set_active(true);
}

void BaseEnemy::beating_update() {
	BeatingBehaviorWork& value = std::get<BeatingBehaviorWork>(behaviorValue);
	value.timer += WorldClock::DeltaSeconds();
	value.timer = std::fmod(value.timer, globalValues.get_value<float>("Enemy", "BeatInterval"));
	if (value.timer < WorldClock::DeltaSeconds()) {
		hitpoint -= globalValues.get_value<int>("Enemy", "BeatingDamage");
		beatCollider->set_active(true);
	}
	else {
		beatCollider->set_active(false);
	}
	if (hitpoint <= 0) {
		behavior.request(EnemyBehavior::Down);
		beatCollider->set_active(false);
	}
}

// ---------- 被ハート時処理 ----------
void BaseEnemy::damaged_heart_initialize() {
	behaviorValue = DamagedBehaviorWork{
		{ [&] { behavior.request(EnemyBehavior::Approach); }, 1 }
	};
}

void BaseEnemy::damaged_heart_update() {
	DamagedBehaviorWork& value = std::get<DamagedBehaviorWork>(behaviorValue);
	value.damagedTimedCall.update();
	velocity *= globalValues.get_value<float>("Enemy", "NockbackFriction");
	transform.plus_translate(velocity * WorldClock::DeltaSeconds());
}

// ---------- 被ビート時処理 ----------
void BaseEnemy::damaged_beat_initialize() {
	behaviorValue = DamagedBehaviorWork{
		{ [&] { behavior.request(EnemyBehavior::Approach); }, 3 }
	};
}

void BaseEnemy::damaged_beat_update() {
	DamagedBehaviorWork& value = std::get<DamagedBehaviorWork>(behaviorValue);
	value.damagedTimedCall.update();
}

// ---------- ダウン時処理 ----------
void BaseEnemy::down_initialize() {
	if (markedCount) {
		// BeatManagerに通知
		beatManager->enemy_down(this);
	}
	// ダウンしたらマークを戻す
	markedCount = 0;
	behaviorValue = DownBehaviorWork{ 0 };
}

void BaseEnemy::down_update() {
	DownBehaviorWork& value = std::get<DownBehaviorWork>(behaviorValue);

	value.timer += WorldClock::DeltaSeconds();
	if (value.timer >= 0 && (value.timer - 1.0f) <= WorldClock::DeltaSeconds()) {
		hitCollider->set_active(true);
	}
	if (value.timer >= 6.0f) {
		behavior.request(EnemyBehavior::Erase);
	}
}

// ---------- 復活時処理 ----------
void BaseEnemy::revive_initialize() {
	hitCollider->set_active(false);
	behaviorValue = ReviveBehaviorWork{
		// 3秒後にApproachに戻す
		{ [&] { behavior.request(EnemyBehavior::Approach); }, 3 }
	};
}

void BaseEnemy::revive_update() {
	ReviveBehaviorWork& value = std::get<ReviveBehaviorWork>(behaviorValue);
	value.revicedCall.update();
}

// ---------- 死亡時処理 ----------
void BaseEnemy::erase_initialize() {
	hitCollider->set_active(false);
	behaviorValue = EraseBehaviorWork{
		{ [&] { isActive = false; }, 3 }
	};
}

void BaseEnemy::erase_update() {
	EraseBehaviorWork& value = std::get<EraseBehaviorWork >(behaviorValue);
	value.despawnTimedCall.update();
}
