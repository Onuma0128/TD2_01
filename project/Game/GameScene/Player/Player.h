#pragma once

#include "Engine/Module/GameObject/GameObject.h"
#include <Engine/Module/Collision/Collider/SphereCollider.h>
#include "Engine/Application/Audio/AudioPlayer.h"

#include <memory>

#include "Game/GameScene/Player/PlayerBullet.h"
#include "Game/GameScene/Player/PlayerSweat.h"

#include "Game/GlobalValues/GlobalValues.h"

class BaseEnemy;
class BeatManager;
class PlayerHPManager;
class CollisionManager;
class WaveSprite;
class PostEffectManager;

class Player : public WorldInstance 
{
public:
	enum class State {
		Move,
		Beating,
		Throwing,
		NockBack,
		Dead,
	};

public:
	void initialize();

	void begin();

	void update();

	void begin_rendering() noexcept;

	void draw() const;

#ifdef _DEBUG
	void debug_gui();
#endif // _DEBUG

	/*==================== メンバ関数 ====================*/
	void InputPad();

	void Move();

	void SetBeat();

	void Beating();

	void ThrowHeart();

	void KnockBack();

	void AddSweat();

	void InvincibleUpdate();

	void Dead();

	float EaseOutCubic(float t);

public:
	State get_state() const { return state_; }
	std::weak_ptr<SphereCollider> get_hit_collider() const;
	void reset_hitpoint(int hitpoint_);
	const std::vector<std::unique_ptr<PlayerBullet>>& get_bullets() const {
		return bullets_;
	}
	void set_posteffect_manager(PostEffectManager* peManager) { postEffectManager = peManager; };

private:
	void OnCollisionCallBack(const BaseCollider* const other);

private:
	bool releaseButton;
	bool unreleaseOnce;

	float beatingTimer;

	std::unique_ptr<GameObject> playerMesh;

	Vector2 input = CVector2::ZERO;
	Vector3 velocity = CVector3::ZERO;

	std::vector<std::unique_ptr<PlayerBullet>> bullets_;
	float attackFrame = 0;
	std::shared_ptr<SphereCollider> hitCollider;

	std::vector<std::unique_ptr<PlayerSweat>> sweats_;

	// ノックバック
	float nockBackFrame_ = 0;
	Vector3 damageSourcePosition_{ 0,0,1 };

	// 無敵時間
	bool isInvincible_ = false;
	float invincibleFrame_ = 0;

	// 今の回転軸保存(死ぬ時使う)
	Quaternion axisOfQuaternion_{};
	float downFrame_ = 0;
	bool lastBeat_ = false;

	State state_;

	PostEffectManager* postEffectManager;

	// Audio
	std::unique_ptr<AudioPlayer> damageAudio_ = nullptr;

	std::vector<std::unique_ptr<AudioPlayer>> throwAudios_;
	int throwCount = 0;

public:
	inline static GlobalValues& globalValues = GlobalValues::GetInstance();
	inline static BeatManager* beatManager = nullptr;
	inline static PlayerHPManager* playerHpManager_ = nullptr;
	inline static CollisionManager* collisionManager = nullptr;
	inline static WaveSprite* waveSprite_ = nullptr;
};