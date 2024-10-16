#pragma once

#include "Engine/Module/GameObject/GameObject.h"

#include "Game/GlobalValues/GlobalValues.h"

class PlayerSweat : public GameObject
{
public:
	void initialize(const Vector3& velocity);

	void update() override;

private:

	Vector3 offset_ = { 0.0f,1.5f,0.0f };
	Vector3 velocity_ = { 0.0f,0.1f,0.0f };
	// 最初の回転
	Quaternion initialRotation_{};

public:
	inline static GlobalValues& globalValues = GlobalValues::GetInstance();
	inline static const WorldInstance* player = nullptr;
};