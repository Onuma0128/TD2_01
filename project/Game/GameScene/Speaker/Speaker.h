#pragma once
#include <memory>
#include <vector>

#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Math/Vector3.h>

class Speaker
{
public:

	void initialize();

	void update();

	void begin_rendering();

	void draw();

	void debug_gui();

	Vector3 pulsatingScale(float time, float baseScale = 1.0f, float amplitude = 0.25f, float frequency = 1.0f);

private:

	std::vector<std::unique_ptr<GameObject>> speakers_;
	Vector3 objScale_{};

	float scaleFrame_ = 0;

};