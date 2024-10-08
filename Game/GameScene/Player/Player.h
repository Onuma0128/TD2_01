#pragma once

#include "Engine/Module/GameObject/GameObject.h"

#include <memory>

#include "Game/GameScene/Player/PlayerBullet.h"

class BaseEnemy;

class Player : public GameObject 
{
public:
	enum class State {
		Move,
		Attack,
	};

public:
	Player();

	void initialize();

	void update()override;

	void begin_rendering() noexcept override;

	void draw() const override;

	void debug_gui();

	/*==================== メンバ関数 ====================*/

	void Move();

	void Attack();

	void SetEnemy(BaseEnemy* enemy) { enemy_ = enemy; }
	State get_state() const { return state_; }


private:
	Vector2 input = CVector2::ZERO;
	Vector3 velocity = CVector3::ZERO;

	BaseEnemy* enemy_ = nullptr;

	std::vector<std::unique_ptr<PlayerBullet>> bullets_;
	float attackFrame = 0;

	State state_;
};