#include "Fade.h"

#include <Engine/Application/WorldClock/WorldClock.h>

void Fade::initialize()
{
	sprite_ = std::make_unique<SpriteObject>("fade.png", Vector2{ 0.5f,0.5f });
	sprite_->set_translate(Vector2{ 640.0f,360.0f });
	fadeFrame_ = 1.0f;
	state_ = FadeState::FadeOut;
}

void Fade::update()
{
	switch (state_)
	{
	case Fade::FadeState::FadeIN:
		fade_in();
		break;
	case Fade::FadeState::FadeOut:
		fade_out();
		break;
	case Fade::FadeState::FadeStop:

		break;
	default:
		break;
	}
}

void Fade::begin_rendering()
{
	sprite_->begin_rendering();
}

void Fade::draw()
{
	sprite_->draw();
}

void Fade::fade_in()
{
	fadeFrame_ += WorldClock::DeltaSeconds();
	fadeFrame_ = std::clamp(fadeFrame_, 0.0f, 1.0f);
	float t = std::clamp(fadeFrame_, 0.0f, 1.0f);
	sprite_->set_alpha(t);
	if (fadeFrame_ >= 1.0f) {
		state_ = FadeState::FadeStop;
	}
}

void Fade::fade_out()
{
	fadeFrame_ -= WorldClock::DeltaSeconds();
	fadeFrame_ = std::clamp(fadeFrame_, 0.0f, 1.0f);
	float t = std::clamp(fadeFrame_, 0.0f, 1.0f);
	sprite_->set_alpha(t);
	if (fadeFrame_ <= 0.0f) {
		state_ = FadeState::FadeStop;
	}
}
