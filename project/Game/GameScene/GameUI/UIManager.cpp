#include "UIManager.h"

#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Application/Input/InputEnum.h>
#include <Engine/Math/Easing.h>

#include "Game/GameScene/Timeline/Timeline.h"
#include "Game/GameScene/EnemyManager/EnemyManager.h"
#include "Game/GameScene/Player/PlayerHPManager.h"
#include "Game/GameScene/GameOverCamera/GameOverCamera.h"


void UIManager::initialize()
{
	waveSprite_ = std::make_unique<WaveSprite>("wave.png");
	hpSprite_ = std::make_unique<HpSprite>("hp.png");
	escSprite_ = std::make_unique<EscSprite>("esc.png");
	buttonSprite_ = std::make_unique<ButtonSprite>("A_button.png");
	spaceSprite_ = std::make_unique<ButtonSprite>("Space_button.png");
	actionSprite_ = std::make_unique<ButtonSprite>("Attack.png");
	beatCommentSprite_ = std::make_unique<SpriteObject>("beatComment.png", Vector2{ 0.5f,0.5f });
	retrySprite_ = std::make_unique<SpriteObject>("cleartext_retry.png", Vector2{ 0.5f,0.5f });
	retrySprite_->set_translate({ 1920,632 });
	titleSprite_ = std::make_unique<SpriteObject>("cleartext_title.png", Vector2{ 0.5f,0.5f });
	titleSprite_->set_translate({ 1920,632 });
	GameOverCamera::waveSprite_ = waveSprite_.get();

	globalValues.add_value<int>("GameUI", "BeatCommentX", 1160);
	globalValues.add_value<int>("GameUI", "BeatCommentY", 380);
	globalValues.add_value<float>("GameUI", "BeatCommentSize", 1.0f);
	globalValues.add_value<int>("GameUI", "PlayerActionX", 1160);
	globalValues.add_value<int>("GameUI", "PlayerActionY", 480);
	globalValues.add_value<float>("GameUI", "PlayerActionSize", 1.0f);
	globalValues.add_value<int>("GameUI", "ButtonX", 1160);
	globalValues.add_value<int>("GameUI", "ButtonY", 576);
	globalValues.add_value<float>("GameUI", "ButtonSize", 1.0f);
	globalValues.add_value<int>("GameUI", "SpaceX", 1160);
	globalValues.add_value<int>("GameUI", "SpaceY", 672);
	globalValues.add_value<float>("GameUI", "SpaceSize", 1.0f);

	gameOverFrame_ = 1;
	gameClearFrame_ = 1;
	gameAllClearFrame_ = 0;
}

void UIManager::update()
{
	waveSprite_->update();
	hpSprite_->update();


	allGameClear();
	gameOver();
	// ボタン系のスプライトを更新
	input_action();
	if (gameOverCamera_->get_state() == GameOverCamera::CameraState::GameOverSprite ||
		(waveSprite_->get_waveNumber() > 10 && gameClearFrame_ != 1.0f)) {
		return;
	}
	input_update();
	escSprite_->update();
	buttonSprite_->update();
	spaceSprite_->update();
	actionSprite_->update();
}

void UIManager::begin_rendering()
{
	waveSprite_->begin_rendering();
	hpSprite_->begin_rendering();
	//escSprite_->begin_rendering();
	buttonSprite_->begin_rendering();
	spaceSprite_->begin_rendering();
	actionSprite_->begin_rendering();
	beatCommentSprite_->begin_rendering();
	retrySprite_->begin_rendering();
	//titleSprite_->begin_rendering();
}

void UIManager::draw()
{
	hpSprite_->draw();
	//escSprite_->draw();
	buttonSprite_->draw();
	spaceSprite_->draw();
	actionSprite_->draw();

	if (isBeatComment_) {
		beatCommentSprite_->draw();
	}
	waveSprite_->draw();
	retrySprite_->draw();
	//titleSprite_->draw();
}

void UIManager::input_update()
{
	Vector2 translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "ButtonX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "ButtonY"))
	};
	buttonSprite_->set_size(globalValues.get_value<float>("GameUI", "ButtonSize"));
	buttonSprite_->set_translate(translate);

	translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "SpaceX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "SpaceY"))
	};
	spaceSprite_->set_size(globalValues.get_value<float>("GameUI", "SpaceSize"));
	spaceSprite_->set_translate(translate);


	translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "PlayerActionX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "PlayerActionY"))
	};
	actionSprite_->set_size(globalValues.get_value<float>("GameUI", "PlayerActionSize"));
	actionSprite_->set_translate(translate);

	translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "BeatCommentX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "BeatCommentY"))
	};
	beatCommentSprite_->set_scale({
		globalValues.get_value<float>("GameUI", "BeatCommentSize"),
		globalValues.get_value<float>("GameUI", "BeatCommentSize") 
	});
	beatCommentSprite_->set_translate(translate);
}

void UIManager::input_action()
{
	// Aボタンの処理
	if (Input::IsPressPad(PadID::A)) {
		buttonSprite_->set_texture("A_button_push.png");
	}
	else {
		buttonSprite_->set_texture("A_button.png");
	}
	// スペースの処理
	if (Input::IsPressKey(KeyID::Space)) {
		spaceSprite_->set_texture("Space_button_push.png");
	}
	else {
		spaceSprite_->set_texture("Space_button.png");
	}
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
		isBeatComment_ = true;
		if (Input::IsPressPad(PadID::A) || Input::IsPressKey(KeyID::Space)) {
			beatCommentFrame_ += WorldClock::DeltaSeconds();
		}
		if (beatCommentFrame_ > 10.0f) {
			isBeatComment_ = false;
		}
	}
	else {
		actionSprite_->set_texture("Attack.png");
		isBeatComment_ = false;
	}
}

void UIManager::gameOver()
{
	if (gameOverCamera_->get_state() == GameOverCamera::CameraState::GameOverSprite && gameOverFrame_ > 0.0f) {
		gameOverFrame_ -= WorldClock::DeltaSeconds();
		float t = std::clamp(gameOverFrame_, 0.0f, 1.0f);
		escSprite_->set_alpha(t);
		spaceSprite_->set_alpha(t);
		buttonSprite_->set_alpha(t);
		actionSprite_->set_alpha(t);
		beatCommentSprite_->set_alpha(t);
		hpSprite_->set_alpha(t);
		if (t == 0.0f) {
			escSprite_->set_translate({ 1920,600 });
			spaceSprite_->set_translate({ 1920,600 });
			buttonSprite_->set_translate({ 1920,600 });
			retrySprite_->set_translate({ 1920,552 });
			titleSprite_->set_translate({ 1920,552 });
			escSprite_->set_alpha(1);
			spaceSprite_->set_alpha(1);
			buttonSprite_->set_alpha(1);
		}
	}
	else {
		if (waveSprite_->get_waveNumber() <= 10 && gameClearFrame_ == 1.0f) {
			float t = Easing::Out::Quint(gameOverCamera_->get_frame() - 2.0f);
			t = std::clamp(t, 0.0f, 1.0f);
			if (gameOverCamera_->get_frame() >= 2.0f) {
				//escSprite_->set_translate(Vector2::Lerp(Vector2{ 1920,600 }, Vector2{ 456,600 }, t));
				buttonSprite_->set_translate(Vector2::Lerp(Vector2{ 1920,600 }, Vector2{ 560,600 }, t));
				spaceSprite_->set_translate(Vector2::Lerp(Vector2{ 1920,600 }, Vector2{ 700,600 }, t));
				retrySprite_->set_translate(Vector2::Lerp(Vector2{ 1920,552 }, Vector2{ 700,552 }, t));
				//titleSprite_->set_translate(Vector2::Lerp(Vector2{ 1920,552 }, Vector2{ 456,552 }, t));
			}
		}
	}
}

void UIManager::allGameClear()
{
	if (waveSprite_->get_waveNumber() == 10 && waveSprite_->get_state() == WaveSprite::WaveState::Return) {
		gameClearFrame_ -= WorldClock::DeltaSeconds();
		float t = std::clamp(gameClearFrame_, 0.0f, 1.0f);
		escSprite_->set_alpha(t);
		spaceSprite_->set_alpha(t);
		buttonSprite_->set_alpha(t);
		actionSprite_->set_alpha(t);
		beatCommentSprite_->set_alpha(t);
		hpSprite_->set_alpha(t);
		if (t == 0.0f) {
			escSprite_->set_translate({ 1920,680 });
			spaceSprite_->set_translate({ 1920,680 });
			buttonSprite_->set_translate({ 1920,680 });
			retrySprite_->set_translate({ 1920,632 });
			titleSprite_->set_translate({ 1920,632 });
			gameAllClearFrame_ = 0;
		}
	}
	else if (waveSprite_->get_state() == WaveSprite::WaveState::Reappear) {
		if (gameOverCamera_->get_state() != GameOverCamera::CameraState::GameOverSprite && gameClearFrame_ <= 0.0f) {
			gameAllClearFrame_ += WorldClock::DeltaSeconds();
			float t = Easing::Out::Quint(gameAllClearFrame_);
			t = std::clamp(t, 0.0f, 1.0f);
			escSprite_->set_alpha(1);
			spaceSprite_->set_alpha(1);
			buttonSprite_->set_alpha(1);
			//escSprite_->set_translate(Vector2::Lerp(Vector2{ 1920,680 }, Vector2{ 456,680 }, t));
			buttonSprite_->set_translate(Vector2::Lerp(Vector2{ 1920,680 }, Vector2{ 560,680 }, t));
			spaceSprite_->set_translate(Vector2::Lerp(Vector2{ 1920,680 }, Vector2{ 700,680 }, t));
			retrySprite_->set_translate(Vector2::Lerp(Vector2{ 1920,632 }, Vector2{ 700,632 }, t));
			//titleSprite_->set_translate(Vector2::Lerp(Vector2{ 1920,632 }, Vector2{ 456,632 }, t));
		}
	}
}
