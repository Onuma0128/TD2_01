#include "BaseEnemy.h"

#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Utility/SmartPointer.h>
#include "Engine/DirectX/DirectXCommand/DirectXCommand.h"

#include "Game/GameScene/Player/PlayerBullet.h"
#include "Game/GameScene/Player/PlayerHPManager.h"
#include "Game/GameScene/BeatManager/BeatManager.h"

void BaseEnemy::initialize(const Vector3& translate, const Vector3& forward) {
	globalValues.add_value<int>("Heart", "AttackDamage", 30);

	globalValues.add_value<int>("Enemy", "HP", 100);
	globalValues.add_value<int>("Enemy", "BeatingDamage", 20);
	globalValues.add_value<int>("Enemy", "BeatHitDamage", 5);
	globalValues.add_value<int>("Enemy", "RevivedHitpoint", 30);

	globalValues.add_value<float>("Enemy", "NockbackMag", 3.0f);
	globalValues.add_value<float>("Enemy", "NockbackFriction", 0.8f);

	globalValues.add_value<float>("Enemy", "StartAttackDistance", 1.0f);

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

	hitMarkerMesh = eps::CreateUnique<GameObject>("HitMarker.obj");
	hitMarkerMesh->initialize();
	hitMarkerMesh->set_parent(*this);

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

void BaseEnemy::begin() {
	beatCollider->set_active(false);
}

void BaseEnemy::update() {
	// 行動の更新
	behavior.update();

	float markingTime = globalValues.get_value<float>("Enemy", "AbsorptionTime");
	// 付与状態かつビートでない場合
	if (markedCount && behavior.state() != EnemyBehavior::Beating) {
		markingTimer += WorldClock::DeltaSeconds();
		*percentage.get_data() = 1 - markingTimer / markingTime;
	}

	// 付与状態でカウンタが達成した場合
	if (markingTimer >= markingTime) {
		markingTimer = 0;
		beatManager->recovery(this);
		// 回復
		hitpoint += globalValues.get_value<int>("Enemy", "AbsorptionAmount") * markedCount;
		// 上限を超えないようにする
		hitpoint = std::min(maxHitpoint, hitpoint);
		markedCount = 0;
	}
	if (hitpoint <= 0) {
		if (behavior.state() != EnemyBehavior::Down &&
			behavior.state() != EnemyBehavior::Erase &&
			behavior.state() != EnemyBehavior::Revive)
			behavior.request(EnemyBehavior::Down);
	}
}

void BaseEnemy::begin_rendering() {
	WorldInstance::update_matrix();
	ghostMesh->begin_rendering();
	hitMarkerMesh->begin_rendering();
}

void BaseEnemy::draw() const {
	ghostMesh->draw();
}

void BaseEnemy::draw_marker() const {
	if (markedCount) {
		DirectXCommand::GetCommandList()->SetGraphicsRootConstantBufferView(
			3, percentage.get_resource()->GetGPUVirtualAddress()
		);
		hitMarkerMesh->draw();
	}
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

void BaseEnemy::start_beat() {
	behavior.request(EnemyBehavior::Beating);
}

void BaseEnemy::beating() {
	hitpoint -= globalValues.get_value<int>("Enemy", "BeatingDamage");

	beatCollider->set_active(true);
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
	behaviorTimer = 0;
}

void BaseEnemy::spawn_update() {
	behaviorTimer += WorldClock::DeltaSeconds();
	if (behaviorTimer >= 3) {
		behavior.request(EnemyBehavior::Approach);
	}
}

// ---------- 移動処理 ----------
void BaseEnemy::approach_initialize() {
	hitCollider->set_active(true);
}

void BaseEnemy::approach_update() {
	if (!targetPlayer) {
		return;
	}
	// プレイヤーとの距離を算出
	Vector3 distance = targetPlayer->world_position() - world_position();

	// distanceより近かったら攻撃に移行
	float attackDistance = globalValues.get_value<float>("Enemy", "StartAttackDistance");
	if (distance.length() <= attackDistance) {
		behavior.request(EnemyBehavior::Attack);
		return;
	}

	// velocity算出
	velocity = distance.normalize_safe() * approachSpeed;
	transform.plus_translate(velocity * WorldClock::DeltaSeconds());
	// player方向を向く
	look_at(*targetPlayer);
}

// ---------- 攻撃処理 ----------
void BaseEnemy::attack_initialize() {
	behaviorTimer = 0;
	isAttakced = false;
	meleeCollider->set_active(true);
}

void BaseEnemy::attack_update() {
	behaviorTimer += WorldClock::DeltaSeconds();
	if (behaviorTimer < 1.0f) {
		meleeCollider->set_active(false);
	}
	else if (behaviorTimer < 1.5f) {
		meleeCollider->set_active(true);
	}
	else if (behaviorTimer > 3.0f) {
		meleeCollider->set_active(false);
		behavior.request(EnemyBehavior::Approach);
	}
}

// ---------- 鼓動同期時処理 ----------
void BaseEnemy::beating_initialize() {
}

void BaseEnemy::beating_update() {
}

// ---------- 被ハート時処理 ----------
void BaseEnemy::damaged_heart_initialize() {
	behaviorTimer = 0;
}

void BaseEnemy::damaged_heart_update() {
	behaviorTimer += WorldClock::DeltaSeconds();
	if (behaviorTimer >= 1.0f) {
		behavior.request(EnemyBehavior::Approach);
	}
	velocity *= globalValues.get_value<float>("Enemy", "NockbackFriction");
	transform.plus_translate(velocity * WorldClock::DeltaSeconds());
}

// ---------- 被ビート時処理 ----------
void BaseEnemy::damaged_beat_initialize() {
	behaviorTimer = 0;
}

void BaseEnemy::damaged_beat_update() {
	behaviorTimer += WorldClock::DeltaSeconds();
	if (behaviorTimer >= 1.0f) {
		behavior.request(EnemyBehavior::Approach);
	}
}

// ---------- ダウン時処理 ----------
void BaseEnemy::down_initialize() {
	if (markedCount) {
		// BeatManagerに通知
		beatManager->enemy_down(this);
	}
	// ダウンしたらマークを戻す
	markedCount = 0;
	behaviorTimer = 0;
	// コリジョン無効化
	beatCollider->set_active(false);
	meleeCollider->set_active(false);
	// hitの有効化
	hitCollider->set_active(true);
}

void BaseEnemy::down_update() {
	behaviorTimer += WorldClock::DeltaSeconds();
	if (behaviorTimer >= 3.0f) {
		behavior.request(EnemyBehavior::Erase);
	}
}

// ---------- 復活時処理 ----------
void BaseEnemy::revive_initialize() {
	hitCollider->set_active(false);
	behaviorTimer = 0;
	// HP強制回復
	hitpoint = globalValues.get_value<int>("Enemy", "RevivedHitpoint");
}

void BaseEnemy::revive_update() {
	behaviorTimer += WorldClock::DeltaSeconds();
	if (behaviorTimer >= 3.0f) {
		behavior.request(EnemyBehavior::Approach);
	}
}

// ---------- 死亡時処理 ----------
void BaseEnemy::erase_initialize() {
	hitCollider->set_active(false);
	behaviorTimer = 0;
}

void BaseEnemy::erase_update() {
	behaviorTimer += WorldClock::DeltaSeconds();
	if (behaviorTimer >= 3.0f) {
		isActive = false;
	}
}
