#include "Player.h"

#include <Engine/Application/Input/Input.h>
#include <Engine/Application/Input/InputEnum.h>
#include <Engine/Math/Definition.h>
#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Utility/SmartPointer.h>
#include <Engine/Application/Scene/SceneManager.h>
#include <Engine/Module/Collision/CollisionManager.h>

#include <Game/GameOverScene/GameOverScene.h>
#include "Game/GameScene/Player/PlayerHPManager.h"
#include "Game/GameScene/BeatManager/BeatManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void Player::initialize() {
	globalValues.add_value<float>("Player", "BeatIntervalBase", 1.0f);
	globalValues.add_value<float>("Player", "BeatIntervalMin", 0.1f);

	globalValues.add_value<int>("Enemy", "BeatingDamage", 20);
	globalValues.add_value<int>("Player", "NumBullets", 3);
	globalValues.add_value<float>("Player", "Speed", 3.0f);
	globalValues.add_value<float>("Player", "ThrowTime", 0.3f);
	globalValues.add_value<float>("Player", "TurnAroundSpeed", 0.2f);
	globalValues.add_value<float>("Player", "ColliderRadius", 1.0f);

	globalValues.add_value<float>("Player", "NockbackTime", 0.5f);
	globalValues.add_value<float>("Player", "MaxNockbackStrength", 10.0f);
	globalValues.add_value<float>("Player", "InvincibleTime", 2.0f);

	// 汗
	globalValues.add_value<int>("Sweat", "NumSweat", 5);
	globalValues.add_value<float>("Sweat", "velocityY", 0.1f);
	globalValues.add_value<int>("Sweat", "Radius", 240);
	globalValues.add_value<float>("Sweat", "AccelerationY", 0.4f);
	globalValues.add_value<float>("Sweat", "SmallerScale", 0.25f);

	// 死ぬ時のアニメーション
	globalValues.add_value<float>("DeadAnimation", "BeatScale", 0.5f);
	globalValues.add_value<float>("DeadAnimation", "DownCount", 1.0f);


	// 描画オブジェクトを設定
	playerMesh = std::make_unique<GameObject>("player_model.obj");
	playerMesh->initialize();
	playerMesh->set_parent(*this);

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
		InvincibleUpdate();
		break;
	case Player::State::Beating:
		Beating();
		break;
	case Player::State::Throwing:
		state_ = State::Move;
		break;
	case Player::State::NockBack:
		KnockBack();
		InvincibleUpdate();
		break;
	case Player::State::Dead:
		Dead();
		break;
	default:
		break;
	}

	hitCollider->set_radius(globalValues.get_value<float>("Player", "ColliderRadius"));

	// 弾の座標更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->update();
	}

	for (std::unique_ptr<PlayerSweat>& sweat : sweats_) {
		sweat->update();
	}
	// プレイヤーのHPが0なら死亡処理
	if (playerHpManager_->get_hp() <= 0 && state_ != State::Dead) {
		state_ = State::Dead;
		lastBeat_ = true;
		axisOfQuaternion_ = transform.get_quaternion();
	}
}

void Player::begin_rendering() noexcept {
	update_matrix();
	playerMesh->begin_rendering();

	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->begin_rendering();
	}
	for (std::unique_ptr<PlayerSweat>& sweat : sweats_) {
		sweat->begin_rendering();
	}
}

void Player::draw() const {
	if (isInvincible_) {
		if (static_cast<int>(invincibleFrame_ * 10) % 2 == 0) {
			playerMesh->draw();
		}
	}
	else {
		// 通常時は常に描画
		playerMesh->draw();
	}

	for (const std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->draw();
	}

	for (const std::unique_ptr<PlayerSweat>& sweat : sweats_) {
		sweat->draw();
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
	beatingTimer = 0;
	beatManager->start_beat();
}

void Player::Beating() {
	beatingTimer += WorldClock::DeltaSeconds();
	// 途中でダメージを食らうとインターバルが変わるので、毎回取得する
	float baseInterval = globalValues.get_value<float>("Player", "BeatIntervalBase");
	float minInterval = globalValues.get_value<float>("Player", "BeatIntervalMin");
	float maxHp = (float)playerHpManager_->get_hp() / (float)playerHpManager_->get_max_hitpoint();
	// インターバル間隔を線形補間で算出
	float beatAttackInterval = std::lerp(minInterval, baseInterval, maxHp);
	// インターバルより長いならビートを発生させる
	if (beatingTimer >= beatAttackInterval) {
		beatingTimer = std::fmod(beatingTimer, beatAttackInterval);
		beatManager->beating();
	}
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
			AddSweat();
			// 1回投げたら終わる
			return;
		}
	}
}

void Player::KnockBack()
{
	// ノックバックフレームを更新
	nockBackFrame_ += WorldClock::DeltaSeconds();

	// ノックバックが続く時間
	float knockbackDuration = globalValues.get_value<float>("Player", "NockbackTime");
	float t = nockBackFrame_ / knockbackDuration;

	// イージング関数を使ってノックバック強さを決定
	float easedStrength = EaseOutCubic(t);

	// ノックバック方向を計算
	Vector3 knockbackDirection = transform.get_translate() - damageSourcePosition_;
	knockbackDirection = knockbackDirection.normalize();
	knockbackDirection.y = 0.0f;

	// ノックバック強さ（最大値を設定）
	float maxKnockbackStrength = globalValues.get_value<float>("Player", "MaxNockbackStrength");

	// ノックバック移動（イージングで減少する）
	transform.plus_translate(knockbackDirection * maxKnockbackStrength * easedStrength * WorldClock::DeltaSeconds());

	// ノックバック終了判定
	if (nockBackFrame_ >= knockbackDuration) {
		nockBackFrame_ = 0;
		state_ = State::Move;
	}
}

void Player::AddSweat()
{
	if (playerHpManager_->get_hp() <= playerHpManager_->get_max_hitpoint() / 2) {
		uint32_t numSweat = globalValues.get_value<int>("Sweat","NumSweat");
		for (uint32_t i = 0; i < numSweat; ++i) {
			std::unique_ptr<PlayerSweat> sweat = std::make_unique<PlayerSweat>();
			sweat->initialize(CVector3::BASIS_Z * transform.get_quaternion());
			sweats_.emplace_back(std::move(sweat));
		}
	}
}

void Player::InvincibleUpdate()
{
	// 無敵時間の更新
	invincibleFrame_ += WorldClock::DeltaSeconds();
	if (invincibleFrame_ >= globalValues.get_value<float>("Player", "InvincibleTime")) {
		isInvincible_ = false;
		invincibleFrame_ = globalValues.get_value<float>("Player", "InvincibleTime");
	}
}

void Player::Dead()
{
	invincibleFrame_ = 0.0f;

	if (lastBeat_) {
		downFrame_ += WorldClock::DeltaSeconds();

		// 一回だけスケールを膨らませる
		float t = downFrame_; // 0.0f から 1.0f の範囲で進行

		// プレイヤーの一回限りの膨張動作
		float beatScale = 1.0f + globalValues.get_value<float>("DeadAnimation", "BeatScale") * (1.0f - std::clamp(t / 0.1f, 0.0f, 1.0f));

		// プレイヤーのスケールを更新
		transform.set_scale(Vector3(beatScale, beatScale, beatScale));

		if (downFrame_ >= globalValues.get_value<float>("DeadAnimation","DownCount")) {
			lastBeat_ = false;
			downFrame_ = 0.0f;
			// スケールを完全に元に戻す
			transform.set_scale(CVector3::BASIS);
		}
	}
	else {
		float angle = 80.0f * ToRadian;

		// ローカルZ軸に沿った回転クォータニオンを生成
		Quaternion zRotation = Quaternion::AngleAxis(CVector3::BASIS_Z, angle);

		// ローカルZ軸回転を現在のクォータニオンに掛け合わせて適用
		Quaternion combinedRotation = axisOfQuaternion_ * zRotation;

		// Slerpで回転を補間
		downFrame_ += WorldClock::DeltaSeconds();
		downFrame_ = std::clamp(downFrame_, 0.0f, 1.0f);

		transform.set_quaternion(Quaternion::Slerp(axisOfQuaternion_, combinedRotation, downFrame_));

		// 死んだらシーン切り替え
		SceneManager::SetSceneChange(std::make_unique<GameOverScene>(), 2, false);
	}
}

float Player::EaseOutCubic(float t)
{
	return 1.0f - powf(1.0f - t, 10.0f);
}

std::weak_ptr<SphereCollider> Player::get_hit_collider() const {
	return hitCollider;
}

void Player::reset_hitpoint(int hitpoint_) {
	if (hitpoint_ < 2) {
		hitpoint_ = 2;
	}
	// 弾の初期化
	// 弾の数を指定
	bullets_.clear();
	bullets_.reserve(hitpoint_);
	for (int i = 0; i < hitpoint_; ++i) {
		std::unique_ptr<PlayerBullet>& bullet = bullets_.emplace_back();
		bullet = eps::CreateUnique<PlayerBullet>();
		float parametric = static_cast<float>(i) / hitpoint_;
		// Playerとペアレント
		bullet->initialize(*this);
		bullet->set_angle_offset(PI2 * parametric);
		collisionManager->register_collider("Heart", bullet->get_collider());
	}

	playerHpManager_->reset_max_hp(hitpoint_);
}

void Player::OnCollisionCallBack(const BaseCollider* const other) {
	if (other->group() == "EnemyMelee" && !isInvincible_) {
		for (auto& bullet : bullets_) {
			if (bullet->get_state() == PlayerBullet::State::Follow) {
				bullet->lost();
				playerHpManager_->set_state(HP_State::Damage);
				damageSourcePosition_ = other->world_position();
				state_ = State::NockBack;
				isInvincible_ = true;
				invincibleFrame_ = 0.0f;
				break;
			}
		}
	}
}
