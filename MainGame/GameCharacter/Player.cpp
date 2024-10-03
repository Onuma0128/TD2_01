#include "Player.h"

#include "Engine/Application/Input/Input.h"
#include "Engine/Application/Input/InputEnum.h"

#include "imgui.h"

Player::Player()
{
	Initialize();
}

void Player::Initialize()
{
	playerObject_ = std::make_unique<GameObject>();
	playerObject_->reset_object("Sphere.obj");
}

void Player::Update()
{
	Move();

	Attack();
}

void Player::Begin_Rendering(Camera3D* camera)
{
	playerObject_->begin_rendering(*camera);
}

void Player::Draw()
{
	playerObject_->draw();
}

void Player::Debug_Update()
{
	ImGui::Begin("Player");
	playerObject_->debug_gui();
	ImGui::End();
}

void Player::Move()
{
	Vector3 translate = playerObject_->get_transform().get_translate();

	// 仮の動き
	if (Input::IsPressKey(KeyID::A)) {
		translate.x -= 0.03f;
	}
	if (Input::IsPressKey(KeyID::D)) {
		translate.x += 0.03f;
	}
	if (Input::IsPressKey(KeyID::S)) {
		translate.z -= 0.03f;
	}
	if (Input::IsPressKey(KeyID::W)) {
		translate.z += 0.03f;
	}

	playerObject_->get_transform().set_translate(translate);
}

void Player::Attack()
{
}
