#include "Player.h"

#include <Engine/Application/Input/Input.h>
#include <Engine/Application/Input/InputEnum.h>
#include <Engine/Math/Definition.h>
#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Utility/SmartPointer.h>

#include "Game/GameScene/Player/PlayerHPManager.h"

#include "Game/GameScene/BeatManager/BeatManager.h"


#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

Player::Player() {
	initialize();
}

void Player::initialize() {
	globalValues.add_value<int>("Enemy", "BeatingDamage", 20);
	globalValues.add_value<int>("Player", "NumBullets", 10);
	globalValues.add_value<float>("Player", "Speed", 3.0f);
	globalValues.add_value<float>("Player", "ThrowTime", 0.3f);
	globalValues.add_value<float>("Player", "TurnAroundSpeed", 0.2f);
	globalValues.add_value<float>("Player", "ColliderRadius", 1.0f);

	// 描画オブジェクトを設定
	playerMesh = std::make_unique<GameObject>("player_model.obj");
	playerMesh->initialize();
	playerMesh->set_parent(*this);

	// 弾の初期化
	// 弾の数を指定
	uint32_t numBullets = globalValues.get_value<int>("Player", "NumBullets");
	for (uint32_t i = 0; i < numBullets; ++i) {
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		float parametric = static_cast<float>(i) / numBullets;
		// Playerとペアレント
		bullet->initialize(*this);
		bullet->set_angle_offset(PI2 * parametric);
		bullets_.emplace_back(std::move(bullet));
	}

	hitCollider = eps::CreateShared<SphereCollider>();
	hitCollider->initialize();
	hitCollider->set_parent(*playerMesh);
	hitCollider->get_transform().set_translate_y(1.0f);
	hitCollider->set_on_collision_enter(
		std::bind(&Player::OnCollisionCallBack, this, std::placeholders::_1)
	);
}

void Player::begin() {
	// 再代入
	releaseButton = false;
	input = CVector2::ZERO;
	InputPad();
}

void Player::update() {
	playerMesh->update();
	switch (state_) {
	case Player::State::Move:
		Move();
		break;
	case Player::State::Beating:
		Beating();
		break;
	case Player::State::Throwing:
		state_ = State::Move;
		break;
	default:
		break;
	}

	hitCollider->set_radius(globalValues.get_value<float>("Player", "ColliderRadius"));

	// 弾の座標更新
	for (auto& bullet : bullets_) {
		bullet->update();
	}
}

void Player::begin_rendering() noexcept {
	update_matrix();
	playerMesh->begin_rendering();

	for (auto& bullet : bullets_) {
		bullet->begin_rendering();
	}
}

void Player::draw() const {
	playerMesh->draw();

	for (auto& bullet : bullets_) {
		bullet->draw();
	}
}

#ifdef _DEBUG

void Player::debug_gui() {
	ImGui::Begin("Player");
	playerMesh->debug_gui();
	ImGui::End();
}
#endif // _DEBUG

void Player::InputPad() {
	// パッドボタンが押されているなら攻撃をする
	if (Input::IsTriggerPad(PadID::A) || Input::IsTriggerKey(KeyID::Space)) {
		attackFrame = 0;
	}
	else if (Input::IsPressPad(PadID::A) || Input::IsPressKey(KeyID::Space)) {
		attackFrame += WorldClock::DeltaSeconds();
	}
	else if (Input::IsReleasePad(PadID::A) || Input::IsReleaseKey(KeyID::Space)) {
		attackFrame = 0;
		if (!unreleaseOnce) {
			releaseButton = true;
		}
		else {
			unreleaseOnce = false;
		}
	}
	input = Input::StickL();

	if (Input::IsPressKey(KeyID::W)) {
		input.y = 1.0f;
	}
	if (Input::IsPressKey(KeyID::S)) {
		input.y = -1.0f;
	}
	if (Input::IsPressKey(KeyID::A)) {
		input.x = -1.0f;
	}
	if (Input::IsPressKey(KeyID::D)) {
		input.x = 1.0f;
	}

}

void Player::Move() {
	// 入力から向き決定
	Vector3 moveDirection = Vector3{ input.x, 0, input.y };
	// Velocityに変換
	velocity = moveDirection * globalValues.get_value<float>("Player", "Speed");
	// 足す
	transform.plus_translate(velocity * WorldClock::DeltaSeconds());

	// 移動があれば向きを更新
	if (velocity != CVector3::ZERO) {
		const Quaternion& quaternion = transform.get_quaternion();
		const Quaternion target = Quaternion::LookForward(velocity.normalize());
		float turnAround = globalValues.get_value<float>("Player", "TurnAroundSpeed");
		transform.set_quaternion(Quaternion::Slerp(quaternion, target, turnAround));
	}

	// 長押し時間がThrowTimeより長いならビート状態に遷移
	if (attackFrame >= globalValues.get_value<float>("Player", "ThrowTime") && !beatManager->empty_pair()) {
		SetBeat();
	}
	// その前にボタンが離れたら投げる
	else if (releaseButton) {
		ThrowHeart();
	}
}

void Player::SetBeat() {
	state_ = State::Beating;
	beatManager->do_beat();
}

void Player::Beating() {
	bool killAll = beatManager->empty_pair();
	// ボタンが離れたらor敵が全員倒れたらMoveに戻す
	if (releaseButton || killAll) {
		state_ = State::Move;
		beatManager->pause_beat();
		// すべての敵が倒れた場合はボタンが押しっぱなしなので
		// 1回反応させないようにする
		if (killAll) {
			unreleaseOnce = true;
		}
	}
}

void Player::ThrowHeart() {
	state_ = State::Throwing;
	for (auto& bullet : bullets_) {
		// 追跡状態だったらbulletを投げる
		if (bullet->get_state() == PlayerBullet::State::Follow && playerHpManager_->get_hp() >= 2) {
			bullet->Throw(world_position(), CVector3::BASIS_Z * transform.get_quaternion());
			playerHpManager_->set_state(HP_State::Damage);
			// 1回投げたら終わる
			return;
		}
	}
}

std::weak_ptr<SphereCollider> Player::get_hit_collider() const {
	return hitCollider;
}

const std::vector<std::unique_ptr<PlayerBullet>>& Player::get_bullets() const {
	return bullets_;
}

void Player::OnCollisionCallBack(const BaseCollider* const other) {
	if (other->group() == "EnemyMelee") {
		for (auto& bullet : bullets_) {
			if (bullet->get_state() == PlayerBullet::State::Follow) {
				bullet->lost();
				playerHpManager_->set_state(HP_State::Damage);
				break;
			}
		}
	}
}
