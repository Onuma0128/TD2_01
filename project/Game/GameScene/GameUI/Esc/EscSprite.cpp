#include "EscSprite.h"

EscSprite::EscSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot)
{
	globalValues.add_value<int>("GameUI", "EscX", 96);
	globalValues.add_value<int>("GameUI", "EscY", 656);
	globalValues.add_value<float>("GameUI", "EscSize",1.0f);
}

void EscSprite::update()
{
	Vector2 scale = {
		globalValues.get_value<float>("GameUI", "EscSize"),
		globalValues.get_value<float>("GameUI", "EscSize")
	};
	Vector2 translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "EscX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "EscY"))
	};
	transform->set_scale(scale);
	transform->set_translate(translate);
}

void EscSprite::begin_rendering() noexcept
{
	SpriteObject::begin_rendering();
}

void EscSprite::draw() const
{
	//SpriteObject::draw();
}
