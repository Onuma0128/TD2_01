#include "Player.h"

#include "Engine/Application/Input/Input.h"
#include "Engine/Application/Input/InputEnum.h"
#include "Engine/Math/Definition.h"
#include "Engine/Application/GameTimer/GameTimer.h"

#include "imgui.h"

Player::Player()
{
	Initialize();
}

void Player::Initialize()
{
	playerObject_ = std::make_unique<GameObject>();
	playerObject_->reset_object("Sphere.obj");

	// 弾の初期化
	// 弾の数を指定
	uint32_t bulletIndex = 10;
	for (uint32_t i = 0; i < bulletIndex; ++i) {
		Vector3 bulletPos = playerObject_->get_transform().get_translate() +
		Vector3{ 
			cosf(PI2 * ((float)i + 1) / (float)bulletIndex),
			0.0f,
			sinf(PI2 * ((float)i + 1) / (float)bulletIndex)
		} * 1.5f;
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize(bulletPos);
		bullet->SetTheta(PI2 * ((float)i + 1) / (float)bulletIndex);
		bullets_.push_back(std::move(bullet));
	}
}

void Player::Update()
{
	Move();

	Attack();

	// 弾の座標更新
	for (auto& bullet : bullets_) {
		bullet->Update(playerObject_->get_transform().get_translate());
	}
}

void Player::Begin_Rendering(Camera3D* camera)
{
	playerObject_->begin_rendering(*camera);

	for (auto& bullet : bullets_) {
		bullet->Begin_Rendering(camera);
	}
}

void Player::Draw()
{
	playerObject_->draw();

	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void Player::Debug_Update()
{
	ImGui::Begin("Player");
	playerObject_->debug_gui();
	ImGui::End();

	for (auto& bullet : bullets_) {
		bullet->Debug_Update();
	}
}

void Player::Move()
{
	float speed = 3.0f;
	if (Input::StickL().x != 0 && Input::StickL().y != 0) {
		velocity_ = Input::StickL();
	}
	Vector2 velocity = Input::StickL();

	Vector3 translate = playerObject_->get_transform().get_translate();

	translate.x += velocity.x * speed * GameTimer::DeltaTime();
	translate.z += velocity.y * speed * GameTimer::DeltaTime();

	playerObject_->get_transform().set_translate(translate);
}

void Player::Attack()
{
	if (Input::IsTriggerPad(PadID::A)) {
		for (auto& bullet : bullets_) {
			if (bullet->GetIsAttack() == false) {
				bullet->SetIsAttack(true);
				bullet->SetVelocity(velocity_);
				break;
			}
		}
	}
}
