#include "Player.h"

#include "Engine/Application/Input/Input.h"
#include "Engine/Application/Input/InputEnum.h"
#include "Engine/Math/Definition.h"
#include "Engine/Application/WorldClock/WorldClock.h"
#include "Game/GameScene/Enemy/BaseEnemy.h"

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
	switch (state_)
	{
	case Player::State::Move:
		Move();
		break;
	case Player::State::Attack:
		Attack();
		break;
	default:
		break;
	}


	// 弾の座標更新
	for (auto it = bullets_.begin(); it != bullets_.end(); ) {
		auto& bullet = *it;

		if (bullet->get_state() == PlayerBullet::State::Follow) {
			bullet->set_parent(this->get_hierarchy());
		}
		if (bullet->get_state() == PlayerBullet::State::Attach) {
			// bullet->set_parent(enemy_->get_hierarchy());
		}
		bullet->set_play_translate(transform.get_translate());
		bullet->update();

		if (bullet->get_destructionCount() > 10.0f) {
			// イテレータが無効にならないように削除後に次の要素を取得
			it = bullets_.erase(it);
		}
		else {
			++it;
		}
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

void Player::debug_gui() {
	ImGui::Begin("Player");
	GameObject::debug_gui();
	ImGui::Text("%f", attackFrame);

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
	state_ = State::Attack;
}

void Player::Attack() {

	for (auto& bullet : bullets_) {
		// 通常攻撃(ハートを投げる)
		if (bullet->get_state() == PlayerBullet::State::Follow && attackFrame < 1.0f) {
			if (Input::IsReleasePad(PadID::A)) {
				bullet->attack(world_position(), CVector3::BASIS_Z * transform.get_quaternion());
				break;
			}
		}// ビート攻撃
		else if (bullet->get_state() == PlayerBullet::State::Attach && attackFrame >= 1.0f) {
			bullet->beatAttack();
		}
	}

	// パッドボタンが押されているなら攻撃をする
	if (Input::IsPressPad(PadID::A)) {
		attackFrame += 0.02f;
	}
	else {
		attackFrame = 0.0f;
		state_ = State::Move;
	}
}
