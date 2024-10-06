#pragma once

#include "Engine/Module/GameObject/GameObject.h"

class Player;

class PlayerBullet : public GameObject {
public:
	enum class State {
		Follow,
		Attacking,
		OnGround,
		Attach,
		Comeback,
	};

public:
	void initialize(const WorldInstance& parent);

	void update() override;

	void attack(const Vector3& worldPosition, const Vector3& velocityDirection);

public:
	void set_angle_offset(float offset) { angleOffset = offset; };
	State get_state() const { return state; };

private:
	Vector3 velocity;

	// 脈拍の速さ
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
};

