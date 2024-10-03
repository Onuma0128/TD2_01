#pragma once
#include <memory>

#include "Engine/Module/GameObject/GameObject.h"
#include "Engine/Module/Camera/Camera3D.h"

class Player : public GameObject 
{
public:
	Player();

	void Initialize();

	void Update();

	void Begin_Rendering(Camera3D* camera);

	void Draw();

	void Debug_Update();

	/*==================== メンバ関数 ====================*/

	void Move();

	void Attack();


private:

	std::unique_ptr<GameObject> playerObject_ = nullptr;

};