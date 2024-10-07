#pragma once
#include <memory>

#include "Engine/Module/GameObject/GameObject.h"

#include "Game/GameScene/Player/PlayerBullet.h"

class Player : public GameObject 
{
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


private:
	Vector2 input = CVector2::ZERO;
	Vector3 velocity = CVector3::ZERO;

	std::vector<std::unique_ptr<PlayerBullet>> bullets_;

};