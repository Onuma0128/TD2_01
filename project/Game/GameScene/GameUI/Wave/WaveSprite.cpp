#include "WaveSprite.h"
#include <vector>
#include <list>
#include <format>

#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Application/Input/InputEnum.h>
#include <Engine/Application/Scene/SceneManager.h>

#include "Game/GameScene/Timeline/Timeline.h"
#include "Game/GameScene/Enemy/BaseEnemy.h"
#include "Game/GameScene/EnemyManager/EnemyManager.h"
#include "Game/GameScene/Timeline/GameState.h"
#include "Game/GameScene/GameUI/Fade/Fade.h"
#include "Game/TitleScene/TitleScene.h"
#include "Game/GameScene/GameScene.h"

WaveSprite::WaveSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot) 
{
	reset();
}

WaveSprite::~WaveSprite()
{
	clearAudio_->finalize();
	allClearAudio_->finalize();
	clickAudio_->finalize();
}

void WaveSprite::reset()
{
	state_ = WaveState::Reappear;

	waveNumber_ = GameState::getInstance().getCurrentWave() + 1;

	clearWaveFrame_ = -1.0f;
	transform->set_scale(Vector2{ 1.5f,1.5f });
	transform->set_translate(Vector2{ 1500.0f,360.0f });
	returnPosition_ = transform->get_translate();

	numberSprite_ = std::make_unique<NumberSprite>("1.png");
	numberSprite_->set_translate({ transform->get_translate().x + 160 * transform->get_scale().x,transform->get_translate().y });
	numberSprite_->set_scale(transform->get_scale());
	WaveTextureNumbers();

	clearSprite_ = std::make_unique<SpriteObject>("clear.png", Vector2{ 0.5f,0.5f });
	clearSprite_->set_translate({ 2000.0f,360.0f });

	clearBackSprite_ = std::make_unique<SpriteObject>("clearback.png", Vector2{ 0.5f,0.5f });
	clearBackSprite_->set_translate({ 2000.0f,360.0f });

	allclearSprite_ = std::make_unique<SpriteObject>("allclear.png", Vector2{ 0.5f,0.5f });
	allclearSprite_->set_translate({ 2000.0f,360.0f });

	isClearSpriteMove_ = false;
	isSceneChenge_ = false;

	clearAudio_ = std::make_unique<AudioPlayer>();
	clearAudio_->initialize("clear.wav");
	clearAudio_->set_volume(0.3f);

	allClearAudio_ = std::make_unique<AudioPlayer>();
	allClearAudio_->initialize("allclear.wav");
	allClearAudio_->set_volume(0.3f);

	clickAudio_ = std::make_unique<AudioPlayer>();
	clickAudio_->initialize("click.wav");
	clickAudio_->set_volume(0.2f);
}

void WaveSprite::clear_animation_reset()
{
	state_ = WaveState::Reappear;
	clearWaveFrame_ = 0;
	++waveNumber_;
	WaveTextureNumbers();
	transform->set_scale(Vector2{ 1.5f,1.5f });
	transform->set_translate(Vector2{ 1500.0f,360.0f });
	returnPosition_ = transform->get_translate();
	if (waveNumber_ > 10) {
		returnPosition_ = { 2000,360 };
	}

	clearSprite_->set_translate({ -700.0f,360.0f });
	if (waveNumber_ <= 10) {
		clearBackSprite_->set_translate({ -700.0f,360.0f });
	}
	isClearSpriteMove_ = false;
}

void WaveSprite::update()
{
#ifdef _DEBUG
	if (timeline_->GetIsActiveEditor() && !timeline_->GetisDemoPlay()) {
		return;
	}
#endif // DEBUG

	switch (state_)
	{
	case WaveState::Normal:
		Normal();
		break;
	case WaveState::Return:
		Return();
		break;
	case WaveState::Reappear:
		Reappear();

		break;
	}

	numberSprite_->set_translate({ transform->get_translate().x + 180 * transform->get_scale().x,transform->get_translate().y });
	numberSprite_->set_scale(transform->get_scale());
	numberSprite_->update();
}

void WaveSprite::begin_rendering() noexcept
{
	SpriteObject::begin_rendering();

	numberSprite_->begin_rendering();

	clearBackSprite_->begin_rendering();
	clearSprite_->begin_rendering();
	allclearSprite_->begin_rendering();
}

void WaveSprite::draw() const
{
#ifdef _DEBUG
	if (timeline_->GetIsActiveEditor() && !timeline_->GetisDemoPlay()) {
		return;
	}
#endif // DEBUG
	clearBackSprite_->draw();
	clearSprite_->draw();
	allclearSprite_->draw();
	SpriteObject::draw();
	numberSprite_->draw();
}

void WaveSprite::WaveTextureNumbers()
{
	numberSprite_->set_texture(std::format("{}.png", waveNumber_));
}

void WaveSprite::Normal()
{
	isAddWave_ = true;
	if (timeline_->is_all_enemy_poped()) {
		for (auto& enemy : enemyManager_->get_enemies()) {
			if (enemy.get_hp() > 0 || enemy.get_now_behavior() == EnemyBehavior::Revive || enemy.get_now_behavior() == EnemyBehavior::Beating) {
				isAddWave_ = false;
				break;
			}
		}
	}
	else {
		isAddWave_ = false;
	}
	if (isAddWave_) {
		clearCheckerFrame_ += WorldClock::DeltaSeconds();
	}
	else {
		clearCheckerFrame_ = 0.0f;
	}
	if (clearCheckerFrame_ > 0.75f) {
		state_ = WaveState::Return;
		returnPosition_ = transform->get_translate();
		clearCheckerFrame_ = 0.0f;
		clearWaveFrame_ = 0.0f;
	}
	
}

void WaveSprite::Return()
{
	clearWaveFrame_ += WorldClock::DeltaSeconds();
	if (clearWaveFrame_ >= 0.0f) {
		// WaveSpriteが画面からフェードアウト
		if (!isClearSpriteMove_) {
			float t = easeInBack(clearWaveFrame_);
			t = std::clamp(t, -1.0f, 1.0f);
			transform->set_translate(Vector2::Lerp(returnPosition_, Vector2{ returnPosition_.x - 360,returnPosition_.y }, t));
			if (clearWaveFrame_ >= 1.0f) {
				isClearSpriteMove_ = true;
				returnPosition_ = { 2000.0f,360.0f };
			}
		}
		// ClearSpriteが画面に出てくる処理
		else if (clearWaveFrame_ <= 3.5f) {
			// 1.0f, 2.5f
			float t = (clearWaveFrame_ - 1.3f) / 1.0f;
			t = std::clamp(t, 0.0f, 1.0f);
			if (t == 0.0f) {
				clearAudio_->restart();
				clearAudio_->play();
			}

			float easedT = easeOutBack(t);
			clearSprite_->set_translate(Vector2::Lerp(returnPosition_, { 640.0f,360.0f }, easedT));
			transform->set_translate({ clearSprite_->get_transform().get_translate().x - 240.0f,100.0f });

			t = easeOutQuint(clearWaveFrame_ - 1.0f);
			t = std::clamp(t, 0.0f, 1.0f);
			clearBackSprite_->set_translate(Vector2::Lerp(returnPosition_, { 640.0f,360.0f }, t));
		}
		// ClearSpriteが画面からフェードアウトする処理
		else if(clearWaveFrame_ <= 4.8f) {
			float t = easeInBack(clearWaveFrame_ - 3.5f);
			t = std::clamp(t, -1.0f, 1.0f);
			clearSprite_->set_translate(Vector2::Lerp({ 640.0f,360.0f }, { -700.0f,360.0f }, t));
			transform->set_translate({ clearSprite_->get_transform().get_translate().x - 240.0f,100.0f });

			if (waveNumber_ < 10) {
				t = easeInExpo(clearWaveFrame_ - 3.8f);
				t = std::clamp(t, 0.0f, 1.0f);
				clearBackSprite_->set_translate(Vector2::Lerp({ 640.0f,360.0f }, { -700.0f,360.0f }, t));
			}
		}
		// 全部のSpriteが画面から出たらStateを更新
		else {
			clear_animation_reset();
		}
	}
}

void WaveSprite::Reappear()
{
	clearWaveFrame_ += WorldClock::DeltaSeconds();
	if (waveNumber_ <= 10) {
		if (clearWaveFrame_ <= 1.0f) {
			float duration = 1.0f;
			float t = clearWaveFrame_ / duration;
			t = std::clamp(t, 0.0f, 1.0f);

			float easedT = easeOutBack(t);
			transform->set_translate(Vector2::Lerp(returnPosition_, Vector2{ 600.0f, 360.0f }, easedT));
		}
		else if (clearWaveFrame_ >= 1.5f) {
			float t = easeInBack(clearWaveFrame_ - 1.5f);
			t = std::clamp(t, -1.0f, 1.0f);
			transform->set_scale(Vector2::Lerp(Vector2{ 1.5f,1.5f }, Vector2{ 1.0f,1.0f }, t));
			transform->set_translate(Vector2::Lerp(Vector2{ 600.0f, 360.0f }, Vector2{ 128.0f,64.0f }, t));
			if (clearWaveFrame_ >= 2.5f) {
				state_ = WaveState::Normal;
				transform->set_scale(Vector2{ 1.0f,1.0f });
				transform->set_translate(Vector2{ 128.0f,64.0f });
			}
		}
	}
	else {
		if (clearWaveFrame_ >= 0.0f) {
			// AllClearSpriteが画面に出てくる処理
			if (clearWaveFrame_ > 0.0f && clearWaveFrame_ <= 2.0f) {
				float t = (clearWaveFrame_ - 0.3f) / 1.0f;
				t = std::clamp(t, 0.0f, 1.0f);
				if (t == 0.0f) {
					allClearAudio_->restart();
					allClearAudio_->play();
				}

				float easedT = easeOutBack(t);
				allclearSprite_->set_translate(Vector2::Lerp(returnPosition_, { 640.0f,360.0f }, easedT));

			}
			else {
				if (!isSceneChenge_) {
					if (Input::IsReleaseKey(KeyID::Space) || Input::IsReleasePad(PadID::A)) {
						SceneManager::SetSceneChange(std::make_unique<TitleScene>(), 1, false);
						fadeSprite_->set_state(Fade::FadeState::FadeIN);
						GameState::getInstance().setCurrentWave(0);
						clickAudio_->restart();
						clickAudio_->play();
						isSceneChenge_ = true;
					}
					if (clearWaveFrame_ > 7.0f) {
						SceneManager::SetSceneChange(std::make_unique<TitleScene>(), 1, false);
						fadeSprite_->set_state(Fade::FadeState::FadeIN);
						clickAudio_->restart();
						clickAudio_->play();
						isSceneChenge_ = true;
					}
				}
			}
		}
	}
}

float WaveSprite::easeInExpo(float t)
{
	return t == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * t - 10.0f);
}

float WaveSprite::easeOutQuint(float t)
{
	return 1.0f - powf(1.0f - t, 5.0f);
}

float WaveSprite::easeInBack(float t)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 2.0f;

	return c3 * t * t * t - c1 * t * t;
}
float WaveSprite::easeOutBack(float t)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
}
;
