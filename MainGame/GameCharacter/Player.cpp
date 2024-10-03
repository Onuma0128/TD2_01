#include "Player.h"

#include "Engine/Application/Input/Input.h"
#include "Engine/Application/Input/InputEnum.h"
#include "Engine/Math/Definition.h"

#include "imgui.h"

Player::Player()
{
	Initialize();
}

void Player::Initialize()
{
	playerObject_ = std::make_unique<GameObject>();
	playerObject_->reset_object("Sphere.obj");

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
	float verocity = 0.03f;
	Vector3 translate = playerObject_->get_transform().get_translate();

	// 仮の動き
	if (Input::IsPressKey(KeyID::A)) {
		translate.x -= verocity;
	}
	if (Input::IsPressKey(KeyID::D)) {
		translate.x += verocity;
	}
	if (Input::IsPressKey(KeyID::S)) {
		translate.z -= verocity;
	}
	if (Input::IsPressKey(KeyID::W)) {
		translate.z += verocity;}

	playerObject_->get_transform().set_translate(translate);
}

void Player::Attack()
{
	if (Input::IsTriggerKey(KeyID::Space)) {

	}
}
