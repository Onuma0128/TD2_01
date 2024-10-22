#pragma once
#include <memory>
#include <algorithm>

#include "Engine/Module/GameObject/SpriteObject.h"
#include "Engine/Math/Vector2.h"

class Fade
{
public:

	enum class FadeState {
		FadeIN,
		FadeOut,
		FadeStop,
	};

	void initialize();

	void update();

	void begin_rendering();

	void draw();

	void fade_in();

	void fade_out();

	void set_state(FadeState state) { state_ = state; }

private:

	FadeState state_;
	float fadeFrame_ = 0;
	std::unique_ptr<SpriteObject> sprite_ = nullptr;

};