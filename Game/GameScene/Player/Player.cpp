#include "Player.h"

#include "Engine/Application/Input/Input.h"
#include "Engine/Application/Input/InputEnum.h"
#include "Engine/Math/Definition.h"
#include "Engine/Application/WorldClock/WorldClock.h"

#include "imgui.h"

Player::Player() {
	initialize();
}

void Player::initialize() {
	// 描画オブジェクトを設定
	reset_object("Sphere.obj");

	// 弾の初期化
	// 弾の数を指定
	uint32_t bulletIndex = 10;
	for (uint32_t i = 0; i < bulletIndex; ++i) {
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		float parametric = static_cast<float>(i) / bulletIndex;
		bullet->initialize(*this);
		bullet->set_angle_offset(PI2 * parametric);
		bullets_.emplace_back(std::move(bullet));
	}
}

void Player::update() {
	Move();

	Attack();

	// 弾の座標更新
	for (auto& bullet : bullets_) {
		bullet->update();
	}
}

void Player::begin_rendering() noexcept {
	GameObject::begin_rendering();

	for (auto& bullet : bullets_) {
		bullet->begin_rendering();
	}
}

void Player::draw() const {
	GameObject::draw();

	for (auto& bullet : bullets_) {
		bullet->draw();
	}
}

void Player::Debug_Update() {
	ImGui::Begin("Player");
	GameObject::debug_gui();
	ImGui::End();
}

void Player::Move() {
	float speed = 3.0f;
	input = CVector2::ZERO;
	Vector2 input = Input::StickL();
	velocity = { input.x, 0, input.y };

	transform.plus_translate(velocity * speed * WorldClock::DeltaSeconds());

	if (velocity != CVector3::ZERO) {
		const Quaternion& quaternion = transform.get_quaternion();
		const Quaternion target = Quaternion::LookForward(velocity.normalize());
		transform.set_rotate(Quaternion::Slerp(quaternion, target, 0.2f));
	}
}

void Player::Attack() {
	if (Input::IsTriggerPad(PadID::A)) {
		for (auto& bullet : bullets_) {
			if (bullet->get_state() == PlayerBullet::State::Follow) {
				bullet->attack(world_position(), CVector3::BASIS_Z * transform.get_quaternion());
				break;
			}
		}
	}
}
