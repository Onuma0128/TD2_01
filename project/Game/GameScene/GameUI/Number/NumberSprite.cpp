#include "NumberSprite.h"
#include "Engine/Module/TextureManager/TextureManager.h"

NumberSprite::NumberSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot)
{
	globalValues.add_value<int>("GameUI", "NumberX", 192);
	globalValues.add_value<int>("GameUI", "NumberY", 0);
}

void NumberSprite::update()
{
	Vector2 translate = {
		static_cast<float>(globalValues.get_value<int>("GameUI", "NumberX")) +
		static_cast<float>(globalValues.get_value<int>("GameUI", "WaveX")),
		static_cast<float>(globalValues.get_value<int>("GameUI", "NumberY")) +
		static_cast<float>(globalValues.get_value<int>("GameUI", "WaveY"))
	};
	transform->set_translate(Vector2{ translate.x,translate.y });
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
