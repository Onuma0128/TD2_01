#include "PlayerBullet.h"

#include "Engine/Math/Definition.h"
#include "Engine/Application/WorldClock/WorldClock.h"
#include <Engine/Utility/SmartPointer.h>
#include "Game/WorldSystemValue.h"

#include "Game/GameScene/Enemy/BaseEnemy.h"
#include "Game/GameScene/BeatManager/BeatManager.h"

void PlayerBullet::initialize(const WorldInstance& parent) {
	reset_object("Sphere.obj");

	globalValues.add_value<float>("Heart", "AngleLapCycle", 6.0f);
	globalValues.add_value<float>("Heart", "CamebackTime", 3.0f);
	globalValues.add_value<float>("Heart", "CamebackSpeed", 5.0f);
	globalValues.add_value<float>("Heart", "ToFollowDistance", 1.0f);
	globalValues.add_value<float>("Heart", "HeartbeatCycle", 0.5f);
	globalValues.add_value<float>("Heart", "StartOffset", 1.5f);
	globalValues.add_value<float>("Heart", "AttackSpeed", 6.0f);
	globalValues.add_value<float>("Heart", "HeartBeatAmplitude", 0.05f);
	globalValues.add_value<Vector3>("Heart", "DistanceOffset", { 0,0,1.5f });

	// 脈拍のタイマー
	heartbeatTimer = 0.0f;
	// スケールの振幅
	heartbeatAmplitude_ = 0.05f;
	// 基本のスケール値
	baseScale_ = 0.2f;
	// 地面に着いてからのタイマー
	onGroundTimer = 0.0f;

	globalValues.add_value<float>("Heart", "HeartBaseScale", 0.2f);

	this->set_parent(parent);

	collider = eps::CreateShared<SphereCollider>();
	collider->initialize();
	collider->set_parent(*this);
	collider->set_radius(0.25f);
	// コリジョンは非活性状態
	collider->set_active(false);
	collider->set_on_collision_enter(
		std::bind(&PlayerBullet::on_collision_enter, this, std::placeholders::_1)
	);
}

void PlayerBullet::update() {
	// 非アクティブ状態なら何もしない
	if (!isActive) {
		return;
	}

	// 弾の動き
	BeatNormal();

	switch (state) {
	case PlayerBullet::State::Follow:
	{
		// 攻撃処理が始まってなければプレイヤーの周りを回転
		float angleLapCycle = globalValues.get_value<float>("Heart", "AngleLapCycle");
		angleTimer += WorldClock::DeltaSeconds();
		angleTimer = std::fmod(angleTimer, angleLapCycle);
		parametric = angleTimer / angleLapCycle;

		float angle = parametric * PI2;
		distanceOffset = globalValues.get_value<Vector3>("Heart", "DistanceOffset");
		Vector3 translate = distanceOffset * Quaternion::AngleAxis(CVector3::BASIS_Y, angle + angleOffset);
		get_transform().set_translate(translate);
		break;
	}
	case PlayerBullet::State::Attacking:
		// 移動処理
		velocity += GRAVITY * WorldClock::DeltaSeconds();
		transform.plus_translate(velocity * WorldClock::DeltaSeconds());
		// 地面に落下した時OnGroundにする
		if (transform.get_translate().y <= 0) {
			state = State::OnGround;
			transform.set_translate_y(0);
			collider->set_active(false);
		}
		break;
	case PlayerBullet::State::OnGround:

		// 地面に着いてからのタイマーを増加
		onGroundTimer += WorldClock::DeltaSeconds();
		if (onGroundTimer >= globalValues.get_value<float>("Heart", "CamebackTime")) {
			// 3秒経ったらComeback状態に移行
			state = State::Comeback;
		}
		break;

	case PlayerBullet::State::Attach:
		// 10秒カウントして10秒立ったら弾が消滅
		break;
	
	case PlayerBullet::State::BeatAttack:
	{
		// ビート攻撃をする
		// 弾を揺らしたり動かしたりもろもろ

		// ビート攻撃を止めたらAttachに戻る

		// if (敵が死んだら弾を戻す){
		//     state = State::Comeback;
		// }
	}
	break;
	case PlayerBullet::State::Comeback:
	{
		Vector3 distanceToPlayer = player->world_position() - transform.get_translate();
		// プレイヤーの位置に向かって移動する処理
		Vector3 directionToPlayer = distanceToPlayer.normalize_safe();

		// プレイヤーに戻る速度
		transform.plus_translate(directionToPlayer * globalValues.get_value<float>("Heart", "CamebackSpeed") * WorldClock::DeltaSeconds());

		// プレイヤーに近づいたらFollow状態に戻る
		if (distanceToPlayer.length() < globalValues.get_value<float>("Heart", "ToFollowDistance")) {
			state = State::Follow;
			onGroundTimer = 0.0f;
			destructionCount = 0.0f;
			transform.set_translate(Transform3D::Homogeneous(transform.get_translate(), player->world_matrix().inverse()));
			set_parent(*player);
		}
	}
	break;
	case PlayerBullet::State::Lost:
		isActive = false;
		break;
	default:
		break;
	}
}

void PlayerBullet::BeatNormal() {
	float heartbeatCycle = globalValues.get_value<float>("Heart", "HeartbeatCycle");

	// 脈拍のようなスケール変化
	heartbeatTimer += WorldClock::DeltaSeconds();
	heartbeatTimer = std::fmod(heartbeatTimer, heartbeatCycle);
	parametric = heartbeatTimer / heartbeatCycle;
	// 時間に基づいてスケールが脈打つように変化する
	baseScale_ = globalValues.get_value<float>("Heart", "HeartBaseScale");
	heartbeatAmplitude_ = globalValues.get_value<float>("Heart", "HeartBeatAmplitude");
	float scaleValue = baseScale_ + heartbeatAmplitude_ * (parametric - std::floor(parametric));
	Vector3 scale = { scaleValue, scaleValue, scaleValue };
	get_transform().set_scale(scale);
}

void PlayerBullet::Throw(const Vector3& worldPosition, const Vector3& velocityDirection) {
	hierarchy.reset_parent();
	float StartOffset = globalValues.get_value<float>("Heart", "StartOffset");
	float Speed = globalValues.get_value<float>("Heart", "AttackSpeed");
	transform.set_translate(worldPosition + velocityDirection * StartOffset + Vector3{ 0,0.5f,0 });
	velocity = velocityDirection * Speed;
	velocity.y = 1.0f;
	collider->set_active(true);
	state = State::Attacking;
}

void PlayerBullet::BeatAttack() {
	state = State::BeatAttack;
}

void PlayerBullet::StopBeat() {
	state = State::Attach;
}

void PlayerBullet::ComeBack() {
	state = State::Comeback;
	transform.set_translate(world_position());
	hierarchy.reset_parent();
}

const Vector3& PlayerBullet::get_velocity() const {
	return velocity;
}

void PlayerBullet::lost() {
	state = State::Lost;
}

std::weak_ptr<SphereCollider> PlayerBullet::get_collider() const {
	return collider;
}

void PlayerBullet::on_collision_enter(const BaseCollider* const other) {
	const std::string& group = other->group();
	if (group == "EnemyHit") {
		const BaseEnemy* beatEnemy = dynamic_cast<const BaseEnemy*>(other->get_hierarchy().get_parent_address());
		if (beatEnemy && beatEnemy->get_now_behavior() != EnemyBehavior::Down) {
			Vector3 worldPosition = world_position();
			Vector3 translate = Transform3D::Homogeneous(worldPosition, beatEnemy->world_matrix().inverse());
			set_parent(*beatEnemy);
			transform.set_translate(translate);
			state = State::Attach;
			collider->set_active(false);
			beatManager->set_next_heart(this);
		}
	}
}
