#include "UIManager.h"

#include <Engine/Application/Input/Input.h>
#include <Engine/Application/Input/InputEnum.h>

#include "Game/GameScene/Timeline/Timeline.h"
#include "Game/GameScene/EnemyManager/EnemyManager.h"
#include "Game/GameScene/Player/PlayerHPManager.h"


void UIManager::initialize()
{
	waveSprite_ = std::make_unique<WaveSprite>("wave.png");
	escSprite_ = std::make_unique<EscSprite>("esc.png");
	buttonSprite_ = std::make_unique<ButtonSprite>("A_button.png");
	spaceSprite_ = std::make_unique<ButtonSprite>("Space_button.png");
	actionSprite_ = std::make_unique<ButtonSprite>("Attack.png");

	globalValues.add_value<int>("GameUI", "PlayerActionX", 1160);
	globalValues.add_value<int>("GameUI", "PlayerActionY", 480);
	globalValues.add_value<int>("GameUI", "ButtonX", 1160);
	globalValues.add_value<int>("GameUI", "ButtonY", 576);
	globalValues.add_value<int>("GameUI", "SpaceX", 1160);
	globalValues.add_value<int>("GameUI", "SpaceY", 672);
}

void UIManager::update()
{
	waveSprite_->update();
	escSprite_->update();
	// ボタン系のスプライトを更新
	input_update();
	buttonSprite_->update();
	spaceSprite_->update();
	actionSprite_->update();
}

void UIManager::begin_rendering()
{
	waveSprite_->begin_rendering();
	escSprite_->begin_rendering();
	buttonSprite_->begin_rendering();
	spaceSprite_->begin_rendering();
	actionSprite_->begin_rendering();
}

void UIManager::draw()
{
	waveSprite_->draw();
	escSprite_->draw();
	buttonSprite_->draw();
	spaceSprite_->draw();
	actionSprite_->draw();
}

void UIManager::input_update()
{
	// Aボタンの処理
	if (Input::IsPressPad(PadID::A)) {
		buttonSprite_->set_texture("A_button_push.png");
	}
	else {
		buttonSprite_->set_texture("A_button.png");
	}
	Vector2 translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "ButtonX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "ButtonY"))
	};
	buttonSprite_->set_translate(translate);
	buttonSprite_->update();

	// スペースの処理
	if (Input::IsPressKey(KeyID::Space)) {
		spaceSprite_->set_texture("Space_button_push.png");
	}
	else {
		spaceSprite_->set_texture("Space_button.png");
	}
	translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "SpaceX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "SpaceY"))
	};
	spaceSprite_->set_translate(translate);

	// プレイヤーのアクションスプライト処理
	bool isBeatUI = false;
	for (auto& enemy : enemyManager_->get_enemies()) {
		if (enemy.get_isBeatUI()) {
			isBeatUI = true;
			break;
		}
	}
	if (isBeatUI) {
		actionSprite_->set_texture("Beat.png");
	}
	else {
		actionSprite_->set_texture("Attack.png");
	}
	translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "PlayerActionX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "PlayerActionY"))
	};
	actionSprite_->set_translate(translate);
}
