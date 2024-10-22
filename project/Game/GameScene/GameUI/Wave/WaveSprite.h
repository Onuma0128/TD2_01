#pragma once
#include <memory>

#include <Engine/Module/GameObject/SpriteObject.h>
#include <Engine/Math/Vector2.h>
#include <Engine/Module/Transform2D/Transform2D.h>

#include "Game/GlobalValues/GlobalValues.h"

#include "Game/GameScene/GameUI/Number/NumberSprite.h"

class Timeline;
class EnemyManager;

class WaveSprite : public SpriteObject
{
public:

	enum class WaveState {
		Normal,
		Return,
		Reappear,
	};

	WaveSprite(const std::string& textureName, const Vector2& pivot = Vector2{ 0.5f,0.5f }) noexcept(false);

	void update();

	void begin_rendering() noexcept;

	void draw() const;

	void WaveTextureNumbers();

	void Normal();

	void Return();

	void Reappear();

	float get_clearWaveFrame() { return clearWaveFrame_; }

	WaveState get_state()const { return state_; }

	/*========== イージング関数 ==========*/

	float easeInExpo(float t);

	float easeOutQuint(float t);


	float easeInBack(float t);

	float easeOutBack(float t);

private:

	WaveState state_ = WaveState::Normal;

	int waveNumber_ = 0;
	bool isAddWave_ = false;
	bool isClearSpriteMove_ = false;

	float clearCheckerFrame_ = 0;
	float clearWaveFrame_ = 0;

	Vector2 returnPosition_{};
	
	std::unique_ptr<NumberSprite> numberSprite_ = nullptr;
	std::unique_ptr<SpriteObject> clearSprite_ = nullptr;
	std::unique_ptr<SpriteObject> clearBackSprite_ = nullptr;

public:
    inline static Timeline* timeline_;
	inline static EnemyManager* enemyManager_;


};