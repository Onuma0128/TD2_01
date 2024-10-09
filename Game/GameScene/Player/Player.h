#pragma once

#include "Engine/Module/GameObject/GameObject.h"
#include <Engine/Module/Collision/Collider/SphereCollider.h>

#include <memory>

#include "Game/GameScene/Player/PlayerBullet.h"

#include "Game/GlobalValues/GlobalValues.h"

class Player : public GameObject 
{
public:
	Player();

	void initialize();

	void update()override;

	void begin_rendering() noexcept override;

	void draw() const override;

	void debug_gui();

	/*==================== メンバ関数 ====================*/

	void Move();

	void Attack();

public:
	std::weak_ptr<SphereCollider> get_hit_collider() const;
	const std::list<std::unique_ptr<PlayerBullet>>& get_bullets() const;

private:
	void OnCollisionCallBack(const BaseCollider* const other);

private:
	Vector2 input = CVector2::ZERO;
	Vector3 velocity = CVector3::ZERO;

	std::shared_ptr<SphereCollider> hitCollider;

	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	GlobalValues& globalValues = GlobalValues::GetInstance();
};