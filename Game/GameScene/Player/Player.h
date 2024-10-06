#pragma once
#include <memory>

#include "Engine/Module/GameObject/GameObject.h"
#include "Engine/Module/Camera/Camera3D.h"

#include "Game/GameScene/Player/PlayerBullet.h"

class Player : public GameObject 
{
public:
	Player();

	void Initialize();

	void Update();

	void Begin_Rendering();

	void Draw();

	void Debug_Update();

	/*==================== メンバ関数 ====================*/

	void Move();

	void Attack();


private:
	Vector2 input = CVector2::ZERO;
	Vector3 velocity = CVector3::ZERO;

	std::vector<std::unique_ptr<PlayerBullet>> bullets_;

};