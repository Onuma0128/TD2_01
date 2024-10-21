#pragma once
#include <memory>

#include <Engine/Module/GameObject/SpriteObject.h>
#include <Engine/Math/Vector2.h>
#include <Engine/Module/Transform2D/Transform2D.h>

#include "Game/GlobalValues/GlobalValues.h"

#include "Game/GameScene/GameUI/Number/NumberSprite.h"

class PlayerHPManager;

class HpSprite : public SpriteObject
{
public:

	HpSprite(const std::string& textureName, const Vector2& pivot = Vector2{ 0.5f,0.5f }) noexcept(false);

	void update();

	void begin_rendering() noexcept;

	void draw() const;

	void hp_update();

private:

	std::unique_ptr<NumberSprite> numberSprite_ = nullptr;

public:
	inline static PlayerHPManager* playerHPManager_ = nullptr;

};