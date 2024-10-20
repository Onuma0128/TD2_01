#include "NumberSprite.h"
#include "Engine/Module/TextureManager/TextureManager.h"

NumberSprite::NumberSprite(const std::string& textureName, const Vector2& pivot) noexcept(false)
: SpriteObject(textureName, pivot)
{
}

void NumberSprite::initialize(const std::string& guiName)
{
	guiName_ = guiName;
	globalValues.add_value<int>("GameUI", guiName_ + "NumberX", 192);
	globalValues.add_value<int>("GameUI", guiName_ + "NumberY", 0);
	if (guiName_ == "Wave") {
		globalValues.add_value<float>("GameUI", guiName_ + "NumberSize", 0.0f);
	}
	else {
		globalValues.add_value<float>("GameUI", guiName_ + "NumberSize", 1.0f);
	}
}

void NumberSprite::update()
{
	Vector2 scale{};
	Vector2 translate{};

	if (guiName_ == "Wave") {
		scale = {
			globalValues.get_value<float>("GameUI",guiName_ + "NumberSize") +
			globalValues.get_value<float>("GameUI", "WaveSize"),
			globalValues.get_value<float>("GameUI",guiName_ + "NumberSize") +
			globalValues.get_value<float>("GameUI", "WaveSize")
		};
		translate = {
			static_cast<float>(globalValues.get_value<int>("GameUI",guiName_ + "NumberX")) +
			static_cast<float>(globalValues.get_value<int>("GameUI", "WaveX")),
			static_cast<float>(globalValues.get_value<int>("GameUI",guiName_ + "NumberY")) +
			static_cast<float>(globalValues.get_value<int>("GameUI", "WaveY"))
		};
	}
	else {
		scale = {
			globalValues.get_value<float>("GameUI",guiName_ + "NumberSize"),
			globalValues.get_value<float>("GameUI",guiName_ + "NumberSize")
		};
		translate = {
			static_cast<float>(globalValues.get_value<int>("GameUI",guiName_ + "NumberX")),
			static_cast<float>(globalValues.get_value<int>("GameUI",guiName_ + "NumberY"))
		};
	}
	transform->set_scale(scale);
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
