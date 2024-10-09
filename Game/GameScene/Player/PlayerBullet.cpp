#include "PlayerBullet.h"

#include "Engine/Math/Definition.h"
#include "Engine/Application/WorldClock/WorldClock.h"
#include <Engine/Utility/SmartPointer.h>

#include "Game/WorldSystemValue.h"
#include "Game/GameScene/Enemy/BaseEnemy.h"

void PlayerBullet::initialize(const WorldInstance& parent) {
	reset_object("Sphere.obj");
	// 脈拍のタイマー
	hartbeatTimer = 0.0f;
	// スケールの振幅
	heartbeatAmplitude_ = 0.05f;
	// 基本のスケール値
	baseScale_ = 0.2f;

	this->set_parent(parent.get_hierarchy());

	collider = eps::CreateShared<SphereCollider>();
	collider->initialize();
	//collider->set_active(false);
	collider->set_on_collision_enter(
		std::bind(&PlayerBullet::on_collision_enter, this, std::placeholders::_1)
	);
}

void PlayerBullet::update() {
	float HartbeatCycle = 0.5f;

	// 脈拍のようなスケール変化
	hartbeatTimer += WorldClock::DeltaSeconds();
	hartbeatTimer = std::fmod(hartbeatTimer, HartbeatCycle);
	float parametric = hartbeatTimer / HartbeatCycle;
	// 時間に基づいてスケールが脈打つように変化する
	float scaleValue = baseScale_ + heartbeatAmplitude_ * (parametric - std::floor(parametric));
	Vector3 scale = { scaleValue, scaleValue, scaleValue };
	get_transform().set_scale(scale);

	// 攻撃処理が始まってなければプレイヤーの周りを回転
	constexpr float angleLapCycle = 6.0f;
	angleTimer += WorldClock::DeltaSeconds();
	angleTimer = std::fmod(angleTimer, angleLapCycle);
	parametric = angleTimer / angleLapCycle;
	switch (state) {
	case PlayerBullet::State::Follow:
	{
		angle = parametric * PI2;
		Vector3 translate = distanceOffset * Quaternion::AngleAxis(CVector3::BASIS_Y, angle + angleOffset);
		get_transform().set_translate(translate);
		break;
	}
	case PlayerBullet::State::Attacking:
		velocity += GRAVITY * WorldClock::DeltaSeconds();
		transform.plus_translate(velocity * WorldClock::DeltaSeconds());
		if (transform.get_translate().y <= 0) {
			state = State::OnGround;
			transform.set_translate_y(0);
		}
		break;
	case PlayerBullet::State::OnGround:
		// どうしようね～～～
		break;
	case PlayerBullet::State::Attach:

		break;
	case PlayerBullet::State::Comeback:
		break;
	case PlayerBullet::State::Lost:
		break;
	default:
		break;
	}
}

void PlayerBullet::attack(const Vector3& worldPosition, const Vector3& velocityDirection) {
	hierarchy.reset_parent();
	float StartOffset = 1.5f;
	float Speed = 6.0f;
	transform.set_translate(worldPosition + velocityDirection * StartOffset + Vector3{ 0,0.5f,0 });
	velocity = velocityDirection * Speed;
	velocity.y = 1.0f;
	//collider->set_active(true);
	state = State::Attacking;
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
	if (group == "Enemy") {
		// 
		//const WorldInstance* enemy = other->get_parent_address(); 
		const WorldInstance* enemy = nullptr; // 未実装のため一旦nullptr
		if (enemy) {
			Vector3 worldPosition = world_position();
			Vector3 translate = Transform3D::Homogeneous(worldPosition, enemy->world_matrix());
			set_parent(enemy->get_hierarchy());
			transform.set_translate(translate);
		}
	}
}
