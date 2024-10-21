#include "HpSprite.h"

#include "Game/GameScene/Player/PlayerHPManager.h"

HpSprite::HpSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot)
{
	globalValues.add_value<int>("GameUI", "PlayerHpUI_X", 582);
	globalValues.add_value<int>("GameUI", "PlayerHpUI_Y", 656);
	globalValues.add_value<float>("GameUI", "PlayerHpUI_Size", 1.0f);

	numberSprite_ = std::make_unique<NumberSprite>("hp_0.png");
	numberSprite_->initialize("PlayerHP");
}

void HpSprite::update()
{
	Vector2 scale = {
		globalValues.get_value<float>("GameUI", "PlayerHpUI_Size"),
		globalValues.get_value<float>("GameUI", "PlayerHpUI_Size")
	};
	Vector2 translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "PlayerHpUI_X")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "PlayerHpUI_Y"))
	};
	transform->set_scale(scale);
	transform->set_translate(translate);

	hp_update();

	numberSprite_->update();
}

void HpSprite::begin_rendering() noexcept
{
	SpriteObject::begin_rendering();

	numberSprite_->begin_rendering();
}

void HpSprite::draw() const
{
	SpriteObject::draw();

	numberSprite_->draw();
}

void HpSprite::hp_update()
{
	int hpNum = playerHPManager_->get_hp();

	switch (hpNum)
	{
	case 0:
		numberSprite_->set_texture("hp_0.png");
		color.red = 1.0f;
		color.green = 0.0f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 1:
		numberSprite_->set_texture("hp_1.png");
		color.red = 0.8f;
		color.green = 0.1f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 2:
		numberSprite_->set_texture("hp_2.png");
		color.red = 0.8f;
		color.green = 0.5f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 3:
		numberSprite_->set_texture("hp_3.png");
		color.red = 0.5f;
		color.green = 0.5f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 4:
		numberSprite_->set_texture("hp_4.png");
		color.red = 0.2f;
		color.green = 0.8f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 5:
		numberSprite_->set_texture("hp_5.png");
		color.red = 0.0f;
		color.green = 1.0f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 6:
		numberSprite_->set_texture("hp_6.png");
		color.red = 0.0f;
		color.green = 1.0f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 7:
		numberSprite_->set_texture("hp_7.png");
		color.red = 0.0f;
		color.green = 1.0f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 8:
		numberSprite_->set_texture("hp_8.png");
		color.red = 0.0f;
		color.green = 1.0f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 9:
		numberSprite_->set_texture("hp_9.png");
		color.red = 0.0f;
		color.green = 1.0f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	case 10:
		numberSprite_->set_texture("hp_10.png");
		color.red = 0.0f;
		color.green = 1.0f;
		color.blue = 0.0f;
		numberSprite_->set_color({ color.red,color.green,color.blue });
		break;
	default:
		break;
	}
}
