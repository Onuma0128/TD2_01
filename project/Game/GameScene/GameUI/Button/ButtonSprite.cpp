#include "ButtonSprite.h"

ButtonSprite::ButtonSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot)
{
}

void ButtonSprite::update()
{
}

void ButtonSprite::begin_rendering() noexcept
{
	SpriteObject::begin_rendering();
}

void ButtonSprite::draw() const
{
	SpriteObject::draw();
}
