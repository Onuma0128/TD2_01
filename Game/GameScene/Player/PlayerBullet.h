#pragma once

#include "Engine/Module/GameObject/GameObject.h"
#include <Engine/Module/Collision/Collider/SphereCollider.h>

#include "Game/GlobalValues/GlobalValues.h"

class Player;

class PlayerBullet : public GameObject {
public:
	enum class State {
		Follow,
		Attacking,
		OnGround,
		Attach,
		BeatAttack,
		Comeback,
		Lost,
	};

public:
	void initialize(const WorldInstance& parent);

	void update() override;

	void move();

	void attack(const Vector3& worldPosition, const Vector3& velocityDirection);

	void beatAttack();

	/// <summary>
	/// HPが減る
	/// </summary>
	void lost();

	std::weak_ptr<SphereCollider> get_collider() const;

private:
	void on_collision_enter(const BaseCollider* const other);

public:
	void set_angle_offset(float offset) { angleOffset = offset; };
	const State& get_state() const { return state; }
	void set_play_translate(const Vector3& translate) { playerPos = translate; }
	float get_destructionCount()const { return destructionCount; }
	const Vector3& get_velocity() const;

private:
	Vector3 playerPos;
	Vector3 velocity;

	// 脈拍用タイマー
	float hartbeatTimer;
	// スケールの振幅
	float heartbeatAmplitude_;
	// 基本のスケール値
	float baseScale_;
	// プレイヤーの周りを回転
	float angleTimer= 0;
	float angle= 0;
	float parametric;

	float angleOffset;
	Vector3 distanceOffset{ 0,0,1.5f };

	float onGroundTimer = 0;

	// 弾が敵に当たっている時間
	float destructionCount = 0;

	// 攻撃したか
	State state;

	std::shared_ptr<SphereCollider> collider;

	GlobalValues& globalValues = GlobalValues::GetInstance();
};

