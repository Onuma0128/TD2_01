#include "WaveSprite.h"

#include "Game/GameScene/Timeline/Timeline.h"

WaveSprite::WaveSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot) 
{
	globalValues.add_value<int>("GameUI", "WaveX", 128);
	globalValues.add_value<int>("GameUI", "WaveY", 64);

	numberSprite_ = std::make_unique<NumberSprite>("0.png");
}

void WaveSprite::update()
{
	Vector2 translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "WaveX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "WaveY"))
	};
	transform->set_translate(Vector2{ translate.x,translate.y });

	WaveCount();

	numberSprite_->update();
}

void WaveSprite::begin_rendering() noexcept
{
	SpriteObject::begin_rendering();

	numberSprite_->begin_rendering();
}

void WaveSprite::draw() const
{
	SpriteObject::draw();

	numberSprite_->draw();
}

void WaveSprite::WaveCount()
{
	int waveNumber = static_cast<int>(std::distance(timeline_->GetWaveData().begin(), timeline_->GetNowWave())) + 1;

	switch (waveNumber)
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
