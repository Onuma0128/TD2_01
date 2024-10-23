#pragma once
#include <memory>

#include "Engine/Math/Vector2.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Module/Transform2D/Transform2D.h"
#include "Engine/Module/GameObject/SpriteObject.h"
#include "Engine/Application/Audio/AudioPlayer.h"

#include "Game/GlobalValues/GlobalValues.h"

class Camera3D;
class Player;
class Fade;

class GameOverCamera
{
public:

	enum class CameraState {
		Move,
		Stop,
		GameOverSprite,
	};

	void initialize();

	void update();

	void begin_rendering();

	void draw();

	/*========== メンバ関数 ==========*/

	float easeOutQuint(float t);

	void Move();

	void Stop();

	void SpriteMove();

private:

	CameraState state_;

	Vector2 oldSpritePos_{};
	Vector3 oldPos_{};
	Vector3 playerPos_{};

	float cameraFrame_ = 0;
	bool isMoveCamera_ = false;

	std::unique_ptr<SpriteObject> gameOverSprite_ = nullptr;

	std::unique_ptr<AudioPlayer> clickAudio_ = nullptr;

public:

	inline static GlobalValues& globalValues = GlobalValues::GetInstance();
	inline static Camera3D* camera3d_ = nullptr;
	inline static Player* player_ = nullptr;
	inline static Fade* fadeSprite_ = nullptr;

};