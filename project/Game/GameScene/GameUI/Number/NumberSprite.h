#pragma once
#include <memory>

#include <Engine/Module/GameObject/SpriteObject.h>
#include <Engine/Math/Vector2.h>
#include <Engine/Module/Transform2D/Transform2D.h>

#include "Game/GlobalValues/GlobalValues.h"

class NumberSprite : public SpriteObject
{
public:
	NumberSprite(const std::string& textureName, const Vector2& pivot = Vector2{ 0.5f,0.5f }) noexcept(false);

	void update();

	void begin_rendering() noexcept;

	void draw() const;

	void set_number(const std::string& textureName);

private:

public:
	inline static GlobalValues& globalValues = GlobalValues::GetInstance();

};

