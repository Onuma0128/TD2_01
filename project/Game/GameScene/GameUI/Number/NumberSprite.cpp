#include "NumberSprite.h"
#include "Engine/Module/TextureManager/TextureManager.h"

NumberSprite::NumberSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot)
{
}

void NumberSprite::initialize(const Vector2 translate)
{
}

void NumberSprite::update()
{
	/*Vector2 scale{};
	Vector2 translate{};

	transform->set_scale(scale);
	transform->set_translate(Vector2{ translate.x,translate.y });*/
}

void NumberSprite::begin_rendering() noexcept
{
	SpriteObject::begin_rendering();
}

void NumberSprite::draw() const
{
	SpriteObject::draw();
}

void NumberSprite::set_number(const std::string& textureName)
{
	SpriteObject::set_texture(textureName);
}

void NumberSprite::set_color(const Vector3& color)
{
	this->color.red = color.x;
	this->color.green = color.y;
	this->color.blue = color.z;
}
