#include "PlayerBullet.h"

#include "Engine/Math/Definition.h"
#include "Engine/Application/Input/Input.h"
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
	// 地面に着いてからのタイマー
	onGroundTimer = 0.0f;

	this->set_parent(parent.get_hierarchy());
}

void PlayerBullet::update() {
	// 弾の動き
	move();

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

		// if (当たり判定が起きたら状態遷移){
		//     state = State::Attach;
		// }

		break;
	case PlayerBullet::State::OnGround:
		
		// 地面に着いてからのタイマーを増加
		onGroundTimer += WorldClock::DeltaSeconds();
		if (onGroundTimer >= 3.0f) {
			// 3秒経ったらComeback状態に移行
			state = State::Comeback;
		}

		break;
	case PlayerBullet::State::Attach:

		// 10秒カウントして10秒立ったら弾が消滅
		destructionCount += WorldClock::DeltaSeconds();


		break;
	case PlayerBullet::State::BeatAttack:

		// ビート攻撃をする
		// 弾を揺らしたり動かしたりもろもろ

		// ビート攻撃を止めたらAttachに戻る
		if (Input::IsReleasePad(PadID::A)) {
		    state = State::Attach;
		}

		// if (敵が死んだら弾を戻す){
		//     state = State::Comeback;
		// }

		break;
	case PlayerBullet::State::Comeback:
		
		// プレイヤーの位置に向かって移動する処理
		Vector3 directionToPlayer = playerPos - transform.get_translate();
		directionToPlayer.normalize();

		// プレイヤーに戻る速度
		constexpr float comebackSpeed = 5.0f;
		transform.plus_translate(directionToPlayer * comebackSpeed * WorldClock::DeltaSeconds());

		// プレイヤーに近づいたらFollow状態に戻る
		if ((playerPos - transform.get_translate()).length() < 1.0f) {
			state = State::Follow;
			onGroundTimer = 0.0f;
			destructionCount = 0.0f;
		}
		 
		break;
	}
}

void PlayerBullet::move()
{
	constexpr float hartbeatCycle = 0.5f;

	// 脈拍のようなスケール変化
	hartbeatTimer += WorldClock::DeltaSeconds();
	hartbeatTimer = std::fmod(hartbeatTimer, hartbeatCycle);
	parametric = hartbeatTimer / hartbeatCycle;
	// 時間に基づいてスケールが脈打つように変化する
	float scaleValue = baseScale_ + heartbeatAmplitude_ * (parametric - std::floor(parametric));
	Vector3 scale = { scaleValue, scaleValue, scaleValue };
	if (state == State::Follow) {
		get_transform().set_scale(scale);
	}

	// 攻撃処理が始まってなければプレイヤーの周りを回転
	constexpr float angleLapCycle = 6.0f;
	angleTimer += WorldClock::DeltaSeconds();
	angleTimer = std::fmod(angleTimer, angleLapCycle);
	parametric = angleTimer / angleLapCycle;
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

void PlayerBullet::beatAttack()
{
	state = State::BeatAttack;
}
