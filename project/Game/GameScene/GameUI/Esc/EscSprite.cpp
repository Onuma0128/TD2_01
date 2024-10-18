#include "EscSprite.h"

EscSprite::EscSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot)
{
	globalValues.add_value<int>("GameUI", "EscX", 96);
	globalValues.add_value<int>("GameUI", "EscY", 656);
}

void EscSprite::update()
{
	Vector2 translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "EscX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "EscY"))
	};
	transform->set_translate(Vector2{ translate.x,translate.y });
}

void EscSprite::begin_rendering() noexcept
{
	SpriteObject::begin_rendering();
}

void EscSprite::draw() const
{
	SpriteObject::draw();
}
