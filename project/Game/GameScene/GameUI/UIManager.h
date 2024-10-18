#pragma once
#include <memory>

#include "Engine/Module/GameObject/SpriteObject.h"

#include "Game/GlobalValues/GlobalValues.h"

#include "Game/GameScene/GameUI/Wave/WaveSprite.h"
#include "Game/GameScene/GameUI/Esc/EscSprite.h"

class UIManager
{
public:

	void initialize();

	void update();

	void begin_rendering();

	void draw();


private:

	std::unique_ptr<WaveSprite> waveSprite_ = nullptr;
	std::unique_ptr<EscSprite> escSprite_ = nullptr;

};