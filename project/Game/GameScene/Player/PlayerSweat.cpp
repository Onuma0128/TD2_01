#include "PlayerSweat.h"

#include "Engine/Math/Definition.h"
#include "Engine/Utility/TimedCall/TimedCall.h"

void PlayerSweat::initialize(const Vector3& velocity)
{
	reset_object("playerSweat.obj");

	float angle = globalValues.get_value<int>("Sweat", "Radius") * ToRadian;

	Vector3 normalizedVelocity = {
		-velocity.normalize_safe().z,
		velocity.normalize_safe().y,
		velocity.normalize_safe().x
	};

	Quaternion rotation = Quaternion::AngleAxis(normalizedVelocity, angle);
	initialRotation_ = rotation;

	offset_.x = static_cast<float>(rand() % 7 - 3) * 0.1f;
	offset_.z = static_cast<float>(rand() % 7 - 3) * 0.1f;

	velocity_ = -velocity.normalize_safe();
	float randomY = static_cast<float>(rand() % 3 + 1);
	velocity_.y = globalValues.get_value<float>("Sweat", "velocityY") * randomY;
	acceleration_ = globalValues.get_value<float>("Sweat", "AccelerationY") * randomY;

	// これをtransformにセットするなど
	transform.set_quaternion(rotation);
	transform.plus_translate(player->world_position() + offset_);
}

void PlayerSweat::update()
{
	if (!isActive) {
		return;
	}

	// 時間管理を忘れずに
	velocity_.y -= acceleration_ * WorldClock::DeltaSeconds();
	Vector3 translate = transform.get_translate() + velocity_ * WorldClock::DeltaSeconds();
	// velocity.yに対して回転を減少する
	float t = std::clamp((velocity_.y), 0.0f, 1.0f);
	Quaternion currentRotation = Quaternion::Slerp(initialRotation_, CQuaternion::IDENTITY, 1.0f - t);
	float scale_t = std::clamp(translate.y, 0.0f, 1.0f);
	// transformを更新
	transform.set_scale(Vector3{ scale_t ,scale_t ,scale_t });
	transform.set_quaternion(currentRotation);
	transform.set_translate(translate);

	if (translate.y <= -0.5f) {
		isActive = false;
	}
}
