#pragma once
#include <memory>

#include "Engine/Module/GameObject/GameObject.h"
#include "Engine/Module/Camera/Camera3D.h"

class Player;

class PlayerBullet : public GameObject
{
public:

	void Initialize(const Vector3& position);

	void Update(const Vector3& position);

	void Begin_Rendering(Camera3D* camera);

	void Draw();

	void Debug_Update();


	void SetVelocity(const Vector2& velocity) { velocity_ = velocity; }
	void SetTheta(float theta) { theta_ = theta; }

	bool GetIsAttack() { return isAttack_; }
	void SetIsAttack(bool isAttack) { isAttack_ = isAttack; }

private:

	std::unique_ptr<GameObject> bulletObject_;
	Vector2 velocity_{};

	// 脈拍の速さ
	float heartbeatSpeed_;
	// スケールの振幅
	float heartbeatAmplitude_;
	// 基本のスケール値
	float baseScale_;
	// プレイヤーの周りを回転
	float theta_ = 0;

	// 攻撃したか
	bool isAttack_ = false;
};
