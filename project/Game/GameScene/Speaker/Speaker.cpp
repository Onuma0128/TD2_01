#include "Speaker.h"
#include <cmath>

#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Math/Definition.h>

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void Speaker::initialize()
{
	Quaternion objQuat[4] = {
		Quaternion::AngleAxis(CVector3::BASIS_Y, (180 * ToRadian)),
		Quaternion::AngleAxis(CVector3::BASIS_Y, (0 * ToRadian)),
		Quaternion::AngleAxis(CVector3::BASIS_Y, (90 * ToRadian)),
		Quaternion::AngleAxis(CVector3::BASIS_Y, (-90 * ToRadian)),
	};

	Vector3 objPos[4] = {
		{ -3.175f,0.0f,6.364f },
		{ 3.175f,0.0f,-6.364f },
		{ -9.37f,0.0f,-3.316f },
		{ 9.37f,0.0f,3.316f }
	};
	for (int i = 0; i < 4; ++i) {
		std::unique_ptr<GameObject> obj = std::make_unique<GameObject>("speaker.obj");
		obj->get_transform().set_quaternion(objQuat[i]);
		obj->get_transform().set_translate(objPos[i]);
		speakers_.push_back(std::move(obj));
	}
}

void Speaker::update()
{
	scaleFrame_ += WorldClock::DeltaSeconds();
	objScale_ = pulsatingScale(scaleFrame_);
	for (auto& speaker : speakers_) {
		speaker->get_transform().set_scale(objScale_);
		speaker->update();
	}
}

void Speaker::begin_rendering()
{
	for (auto& speaker : speakers_) {
		speaker->begin_rendering();
	}
}

void Speaker::draw()
{
	for (auto& speaker : speakers_) {
		speaker->draw();
	}
}

#ifdef _DEBUG

void Speaker::debug_gui() {
	ImGui::Begin("Speaker");
	for (auto& speaker : speakers_) {
		speaker->debug_gui();
	}
	ImGui::End();
}
#endif // _DEBUG

Vector3 Speaker::pulsatingScale(float time, float baseScale, float amplitude, float frequency)
{
	// abs(sin)を使って、激しくバクバクさせる
	float scale = baseScale + amplitude * std::fabsf(std::sinf(frequency * time * 2.0f * PI)); // スケールが一方向に変化する
	return Vector3(scale, scale, scale);
}