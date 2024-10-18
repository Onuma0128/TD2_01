#pragma once
#include <memory>

#include <Engine/Module/GameObject/SpriteObject.h>
#include <Engine/Math/Vector2.h>
#include <Engine/Module/Transform2D/Transform2D.h>

#include "Game/GlobalValues/GlobalValues.h"

#include "Game/GameScene/GameUI/Number/NumberSprite.h"

class Timeline;

class WaveSprite : public SpriteObject
{
public:

	WaveSprite(const std::string& textureName, const Vector2& pivot = Vector2{ 0.5f,0.5f }) noexcept(false);

	void update();

	void begin_rendering() noexcept;

	void draw() const;

	void WaveCount();

private:

	std::unique_ptr<NumberSprite> numberSprite_ = nullptr;

public:
	inline static GlobalValues& globalValues = GlobalValues::GetInstance();
    inline static Timeline* timeline_;


};