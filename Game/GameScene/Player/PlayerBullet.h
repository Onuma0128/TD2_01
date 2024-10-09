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
		Comeback,
		Lost,
	};

public:
	void initialize(const WorldInstance& parent);

	void update() override;

	void attack(const Vector3& worldPosition, const Vector3& velocityDirection);

	/// <summary>
	/// HPが減る
	/// </summary>
	void lost();

	std::weak_ptr<SphereCollider> get_collider() const;

private:
	void on_collision_enter(const BaseCollider* const other);

public:
	void set_angle_offset(float offset) { angleOffset = offset; };
	State get_state() const { return state; };
	const Vector3& get_velocity() const;

private:
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

	float angleOffset;
	Vector3 distanceOffset{ 0,0,1.5f };

	// 攻撃したか
	State state;

	std::shared_ptr<SphereCollider> collider;

	GlobalValues& globalValues = GlobalValues::GetInstance();
};

