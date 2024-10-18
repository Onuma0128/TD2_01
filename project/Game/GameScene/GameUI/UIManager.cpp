#include "UIManager.h"

#include "Game/GameScene/Timeline/Timeline.h"
#include "Game/GameScene/Player/PlayerHPManager.h"


void UIManager::initialize()
{
	waveSprite_ = std::make_unique<WaveSprite>("wave.png");
	escSprite_ = std::make_unique<EscSprite>("esc.png");
}

void UIManager::update()
{
	waveSprite_->update();
	escSprite_->update();
}

void UIManager::begin_rendering()
{
	waveSprite_->begin_rendering();
	escSprite_->begin_rendering();
}

void UIManager::draw()
{
	waveSprite_->draw();
	escSprite_->draw();
}
