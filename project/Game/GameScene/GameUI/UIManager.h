#pragma once
#include <memory>
#include <list>

#include "Engine/Module/GameObject/SpriteObject.h"

#include "Game/GlobalValues/GlobalValues.h"

#include "Game/GameScene/GameUI/Wave/WaveSprite.h"
#include "Game/GameScene/GameUI/Esc/EscSprite.h"
#include "Game/GameScene/GameUI/Button/ButtonSprite.h"

class EnemyManager;
class PlayerHPManager;

class UIManager
{
public:

	void initialize();

	void update();

	void begin_rendering();

	void draw();

	void input_update();

	void hp_update();

private:

	std::unique_ptr<WaveSprite> waveSprite_ = nullptr;
	std::unique_ptr<EscSprite> escSprite_ = nullptr;
	std::unique_ptr<ButtonSprite> buttonSprite_ = nullptr;
	std::unique_ptr<ButtonSprite> spaceSprite_ = nullptr;
	std::unique_ptr<ButtonSprite> actionSprite_ = nullptr;
	std::unique_ptr<NumberSprite> playerHpSprite_ = nullptr;

public:

	inline static GlobalValues& globalValues = GlobalValues::GetInstance();
	inline static EnemyManager* enemyManager_ = nullptr;
	inline static PlayerHPManager* playerHPManager_ = nullptr;

};