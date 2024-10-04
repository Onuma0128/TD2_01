#include "PlayerBullet.h"
#include "MainGame/GameCharacter/Player.h"
#include "Engine/Application/GameTimer/GameTimer.h"
#include "imgui.h"

void PlayerBullet::Initialize(const Vector3& position)
{
	bulletObject_ = std::make_unique<GameObject>();
	bulletObject_->reset_object("Sphere.obj");
	bulletObject_->get_transform().set_scale({ 0.2f,0.2f,0.2f });
	bulletObject_->get_transform().set_translate(position);

	// 脈拍の速さ
	heartbeatSpeed_ = 0.0f;
	// スケールの振幅
	heartbeatAmplitude_ = 0.05f;
	// 基本のスケール値
	baseScale_ = 0.2f;
}

void PlayerBullet::Update(const Vector3& position)
{
	// 脈拍のようなスケール変化
	heartbeatSpeed_ += 0.1f;
	// 時間に基づいてスケールが脈打つように変化する
	float scaleValue = baseScale_ + heartbeatAmplitude_ * sinf(heartbeatSpeed_);
	Vector3 scale = { scaleValue, scaleValue, scaleValue };
	bulletObject_->get_transform().set_scale(scale);

	// 攻撃処理が始まってなければプレイヤーの周りを回転
	if (!isAttack_) {
		Vector3 translate = Vector3{
			cosf(theta_) * 1.5f,
			0.0f,
			sinf(theta_) * 1.5f
		};
		theta_ += 0.01f;

		bulletObject_->get_transform().set_translate(translate + position);
	}
	//攻撃処理がされたらプレイヤーのVelocityに飛んでいく
	if (isAttack_) {
		float speed = 10.0f;
		Vector3 translate = bulletObject_->get_transform().get_translate();

		translate.x += velocity_.x * speed * GameTimer::DeltaTime();
		translate.z += velocity_.y * speed * GameTimer::DeltaTime();

		bulletObject_->get_transform().set_translate(translate);
	}
}

void PlayerBullet::Begin_Rendering(Camera3D* camera)
{
	bulletObject_->begin_rendering(*camera);
}

void PlayerBullet::Draw()
{
	bulletObject_->draw();
}

void PlayerBullet::Debug_Update()
{
	ImGui::Begin("Bullet");
	bulletObject_->debug_gui();
	ImGui::End();
}
