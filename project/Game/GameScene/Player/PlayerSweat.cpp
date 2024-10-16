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

	offset_.x = static_cast<float>(rand() % 15 - 7) * 0.1f;
	offset_.z = static_cast<float>(rand() % 5 - 5) * 0.1f;

	velocity_ = -velocity.normalize_safe() / 50.0f;
	velocity_.y = globalValues.get_value<float>("Sweat", "velocityY");

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
	velocity_.y -= globalValues.get_value<float>("Sweat", "AccelerationY") * WorldClock::DeltaSeconds();
	Vector3 translate = transform.get_translate() + velocity_;
	// velocity.yに対して回転を減少する
	float t = std::clamp((velocity_.y + globalValues.get_value<float>("Sweat", "velocityY")) / 0.2f, 0.0f, 1.0f);
	Quaternion currentRotation = Quaternion::Slerp(initialRotation_, CQuaternion::IDENTITY, 1.0f - t);

	float scale_t = std::clamp((velocity_.y + globalValues.get_value<float>("Sweat", "SmallerScale")) / 0.2f, 0.0f, 1.0f);
	// transformを更新
	transform.set_scale(Vector3{ scale_t ,scale_t ,scale_t });
	transform.set_quaternion(currentRotation);
	transform.set_translate(translate);

	if (transform.get_translate().y <= -0.5f) {
		isActive = false;
	}
}
