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
		BeatAttack,
		Comeback,
	};

public:
	void initialize(const WorldInstance& parent);

	void update() override;

	void move();

	void attack(const Vector3& worldPosition, const Vector3& velocityDirection);

	void beatAttack();

public:
	void set_angle_offset(float offset) { angleOffset = offset; };
	State get_state() const { return state; }
	void set_play_translate(const Vector3& translate) { playerPos = translate; }
	float get_destructionCount()const { return destructionCount; }

private:
	Vector3 playerPos;
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
	float parametric;

	float angleOffset;
	Vector3 distanceOffset{ 0,0,1.5f };

	float onGroundTimer = 0;

	// 弾が敵に当たっている時間
	float destructionCount = 0;

	// 攻撃したか
	State state;
};

