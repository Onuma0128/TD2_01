#include "Player.h"

#include "Engine/Application/Input/Input.h"
#include "Engine/Application/Input/InputEnum.h"
#include "Engine/Math/Definition.h"
#include "Engine/Application/WorldClock/WorldClock.h"

#include "imgui.h"

Player::Player()
{
	Initialize();
}

void Player::Initialize()
{
	reset_object("Sphere.obj");

	// 弾の初期化
	// 弾の数を指定
	uint32_t bulletIndex = 10;
	for (uint32_t i = 0; i < bulletIndex; ++i) {
		float theta = PI2 * (i + 1) / bulletIndex;
		Vector3 offset = Vector3{ std::cos(theta), 0.0f, std::sin(theta), };
		Vector3 bulletPos = world_position() + offset * 1.5f;
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
		bullet->Update(world_position());
	}
}

void Player::Begin_Rendering()
{
	begin_rendering();

	for (auto& bullet : bullets_) {
		bullet->Begin_Rendering();
	}
}

void Player::Draw() {
	GameObject::draw();

	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void Player::Debug_Update()
{
	ImGui::Begin("Player");
	GameObject::debug_gui();
	ImGui::End();

	for (auto& bullet : bullets_) {
		//bullet->Debug_Update();
	}
}

void Player::Move()
{
	float speed = 3.0f;
	input = CVector2::ZERO;
	Vector2 input = Input::StickL();
	velocity = { input.x, 0, input.y };

	get_transform().plus_translate(velocity * speed * WorldClock::DeltaSeconds());
}

void Player::Attack()
{
	if (Input::IsTriggerPad(PadID::A)) {
		for (auto& bullet : bullets_) {
			if (bullet->GetIsAttack() == false) {
				bullet->SetIsAttack(true);
				//bullet->SetVelocity(velocity_);
				break;
			}
		}
	}
}
