#pragma once
#include <memory>

#include <Engine/Module/GameObject/SpriteObject.h>
#include <Engine/Math/Vector2.h>
#include <Engine/Module/Transform2D/Transform2D.h>

class ButtonSprite : public SpriteObject
{
public:
	ButtonSprite(const std::string& textureName, const Vector2& pivot = Vector2{ 0.5f,0.5f }) noexcept(false);

	void update();

	void begin_rendering() noexcept;

	void draw() const;


	void set_translate(const Vector2& translate) { transform->set_translate(translate); }

	void set_texture(const std::string& textureName) {
		SpriteObject::set_texture(textureName);
	}

private:

public:

};

