#include "WaveSprite.h"
#include <vector>
#include <list>

#include <Engine/Application/WorldClock/WorldClock.h>

#include "Game/GameScene/Timeline/Timeline.h"
#include "Game/GameScene/Enemy/BaseEnemy.h"
#include "Game/GameScene/EnemyManager/EnemyManager.h"
#include "Game/GameScene/Timeline/GameState.h"

WaveSprite::WaveSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot) 
{
	state_ = WaveState::Reappear;

	waveNumber_ = GameState::getInstance().getCurrentWave() + 1;

	clearWaveFrame_ = -1.0f;
	transform->set_scale(Vector2{ 1.5f,1.5f });
	transform->set_translate(Vector2{ 1500.0f,360.0f });
	returnPosition_ = transform->get_translate();

	numberSprite_ = std::make_unique<NumberSprite>("1.png");
	numberSprite_->set_translate({ transform->get_translate().x + 160 * transform->get_scale().x,transform->get_translate().y});
	numberSprite_->set_scale(transform->get_scale());

	clearSprite_ = std::make_unique<SpriteObject>("clear.png", Vector2{ 0.5f,0.5f });
	clearSprite_->set_translate({ 2000.0f,360.0f });

	clearBackSprite_ = std::make_unique<SpriteObject>("clearback.png", Vector2{ 0.5f,0.5f });
	clearBackSprite_->set_translate({ 2000.0f,360.0f });
}

void WaveSprite::update()
{
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


	WaveCount();

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
}

void WaveSprite::draw() const
{
	clearBackSprite_->draw();
	clearSprite_->draw();
	SpriteObject::draw();
	numberSprite_->draw();
}

void WaveSprite::WaveCount()
{
	switch (waveNumber_)
	{
	case 0:
		numberSprite_->set_texture("0.png");
		break;
	case 1:
		numberSprite_->set_texture("1.png");
		break;
	case 2:
		numberSprite_->set_texture("2.png");
		break;
	case 3:
		numberSprite_->set_texture("3.png");
		break;
	case 4:
		numberSprite_->set_texture("4.png");
		break;
	case 5:
		numberSprite_->set_texture("5.png");
		break;
	case 6:
		numberSprite_->set_texture("6.png");
		break;
	case 7:
		numberSprite_->set_texture("7.png");
		break;
	case 8:
		numberSprite_->set_texture("8.png");
		break;
	case 9:
		numberSprite_->set_texture("9.png");
		break;
	default:
		break;
	}
}

void WaveSprite::Normal()
{
	isAddWave_ = true;
	for (auto& enemy : enemyManager_->get_enemies()) {
		if (enemy.get_hp() > 0 || enemy.get_now_behavior() == EnemyBehavior::Revive) {
			isAddWave_ = false;
			break;
		}
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
		}
		if (clearWaveFrame_ >= 1.0f) {
			isClearSpriteMove_ = true;
			returnPosition_ = { 2000.0f,360.0f };
		}
		// ClearSpriteが画面に出てくる処理
		if (clearWaveFrame_ > 1.0f && clearWaveFrame_ <= 2.5f) {
			float t = (clearWaveFrame_ - 1.3f) / 1.0f;
			t = std::clamp(t, 0.0f, 1.0f);

			float easedT = easeOutBack(t);
			clearSprite_->set_translate(Vector2::Lerp(returnPosition_, { 640.0f,360.0f }, easedT));
			transform->set_translate({ clearSprite_->get_transform().get_translate().x - 240.0f,100.0f });

			t = easeOutQuint(clearWaveFrame_ - 1.0f);
			t = std::clamp(t, 0.0f, 1.0f);
			clearBackSprite_->set_translate(Vector2::Lerp(returnPosition_, { 640.0f,360.0f }, t));
		}
		// ClearSpriteが画面からフェードアウトする処理
		else if(clearWaveFrame_ > 2.5f && clearWaveFrame_ <= 3.8f) {
			float t = easeInBack(clearWaveFrame_ - 2.5f);
			t = std::clamp(t, -1.0f, 1.0f);
			clearSprite_->set_translate(Vector2::Lerp({ 640.0f,360.0f }, { -700.0f,360.0f }, t));
			transform->set_translate({ clearSprite_->get_transform().get_translate().x - 240.0f,100.0f });

			t = easeInExpo(clearWaveFrame_ - 2.8f);
			t = std::clamp(t, 0.0f, 1.0f);
			clearBackSprite_->set_translate(Vector2::Lerp({ 640.0f,360.0f }, { -700.0f,360.0f }, t));
		}
		// 全部のSpriteが画面から出たらStateを更新
		if (clearWaveFrame_ >= 3.8f) {
			state_ = WaveState::Reappear;
			clearWaveFrame_ = 0;
			++waveNumber_;
			transform->set_scale(Vector2{ 1.5f,1.5f });
			transform->set_translate(Vector2{ 1500.0f,360.0f });
			returnPosition_ = transform->get_translate();

			clearSprite_->set_translate({ -700.0f,360.0f });
			clearBackSprite_->set_translate({ -700.0f,360.0f });
			isClearSpriteMove_ = false;
		}
	}
}

void WaveSprite::Reappear()
{
	clearWaveFrame_ += WorldClock::DeltaSeconds();
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
