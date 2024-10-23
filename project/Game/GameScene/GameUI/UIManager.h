#pragma once
#include <memory>
#include <list>

#include "Engine/Module/GameObject/SpriteObject.h"

#include "Game/GlobalValues/GlobalValues.h"

#include "Game/GameScene/GameUI/Wave/WaveSprite.h"
#include "Game/GameScene/GameUI/Hp/HpSprite.h"
#include "Game/GameScene/GameUI/Esc/EscSprite.h"
#include "Game/GameScene/GameUI/Button/ButtonSprite.h"

class EnemyManager;
class PlayerHPManager;
class GameOverCamera;

class UIManager
{
public:

	void initialize();

	void update();

	void begin_rendering();

	void draw();

	void input_update();

	void input_action();

	void gameOver();

	void allGameClear();

	WaveSprite* get_waveSprite()const { return waveSprite_.get(); }

private:

	std::unique_ptr<WaveSprite> waveSprite_ = nullptr;
	std::unique_ptr<HpSprite> hpSprite_ = nullptr;
	std::unique_ptr<EscSprite> escSprite_ = nullptr;
	std::unique_ptr<ButtonSprite> buttonSprite_ = nullptr;
	std::unique_ptr<ButtonSprite> spaceSprite_ = nullptr;
	std::unique_ptr<ButtonSprite> actionSprite_ = nullptr;

	bool isBeatComment_ = false;
	float beatCommentFrame_ = 0;
	std::unique_ptr<SpriteObject> beatCommentSprite_ = nullptr;
	std::unique_ptr<SpriteObject> retrySprite_ = nullptr;
	std::unique_ptr<SpriteObject> titleSprite_ = nullptr;

	float gameOverFrame_ = 1;
	float gameClearFrame_ = 1;
	float gameAllClearFrame_ = 1;

public:

	inline static GlobalValues& globalValues = GlobalValues::GetInstance();
	inline static EnemyManager* enemyManager_ = nullptr;
	inline static GameOverCamera* gameOverCamera_ = nullptr;

};