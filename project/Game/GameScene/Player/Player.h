#pragma once

#include "Engine/Module/GameObject/GameObject.h"
#include <Engine/Module/Collision/Collider/SphereCollider.h>

#include <memory>

#include "Game/GameScene/Player/PlayerBullet.h"

#include "Game/GlobalValues/GlobalValues.h"

class BaseEnemy;
class BeatManager;

class Player : public GameObject 
{
public:
	enum class State {
		Move,
		Beating,
		Throwing,
	};

public:
	Player();

	void initialize();

	void begin() override;

	void update()override;

	void begin_rendering() noexcept override;

	void draw() const override;

	void debug_gui();

	/*==================== メンバ関数 ====================*/
	void InputPad();

	void Move();

	void SetBeat();

	void Beating();

	void ThrowHeart();

public:
	State get_state() const { return state_; }
	std::weak_ptr<SphereCollider> get_hit_collider() const;
	const std::vector<std::unique_ptr<PlayerBullet>>& get_bullets() const;

private:
	void OnCollisionCallBack(const BaseCollider* const other);

private:
	bool releaseButton;
	bool unreleaseOnce;
	Vector2 input = CVector2::ZERO;
	Vector3 velocity = CVector3::ZERO;

	std::vector<std::unique_ptr<PlayerBullet>> bullets_;
	float attackFrame = 0;
	std::shared_ptr<SphereCollider> hitCollider;

	State state_;

public:
	inline static GlobalValues& globalValues = GlobalValues::GetInstance();
	inline static BeatManager* beatManager = nullptr;
};