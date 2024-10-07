#include "PlayerBullet.h"

#include "Engine/Math/Definition.h"
#include "Engine/Application/WorldClock/WorldClock.h"

#include "Game/WorldSystemValue.h"

void PlayerBullet::initialize(const WorldInstance& parent) {
	reset_object("Sphere.obj");
	// 脈拍の速さ
	hartbeatTimer = 0.0f;
	// スケールの振幅
	heartbeatAmplitude_ = 0.05f;
	// 基本のスケール値
	baseScale_ = 0.2f;

	this->set_parent(parent.get_hierarchy());
}

void PlayerBullet::update() {
	constexpr float hartbeatCycle = 0.5f;

	// 脈拍のようなスケール変化
	hartbeatTimer += WorldClock::DeltaSeconds();
	hartbeatTimer = std::fmod(hartbeatTimer, hartbeatCycle);
	float parametric = hartbeatTimer / hartbeatCycle;
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
	state = State::Attacking;
}
