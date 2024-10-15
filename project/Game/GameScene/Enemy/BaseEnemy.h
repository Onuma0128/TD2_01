#pragma once

#include <Engine/Module/GameObject/GameObject.h>

#include <variant>

#include <Engine/Utility/TimedCall/TimedCall.h>
#include <Engine/Module/Collision/Collider/SphereCollider.h>
#include <Engine/Module/Behavior/Behavior.h>
#include <Engine/DirectX/DirectXResourceObject/ConstantBuffer/ConstantBuffer.h>

#include "Game/GlobalValues/GlobalValues.h"

enum class EnemyBehavior {
	Spawn,
	Approach,
	Attack,
	Beating,
	DamagedHeart,
	DamagedBeat,
	Down,
	Revive,
	Erase
};

class Player;
class PlayerHPManager;
class BeatManager;

class BaseEnemy : public WorldInstance {
private: // Structs
	struct SpwanBehaviorWork {
		TimedCall<void(void)> timedCall;
	};
	struct ApproachBehaviorWork {
		float attackDistance;
		float speed;
	};
	struct AttackBehaviorWork {
		float timer;
	};
	struct BeatingBehaviorWork {
		float timer;
	};
	struct DamagedBehaviorWork {
		TimedCall<void(void)> damagedTimedCall;
	};
	struct DownBehaviorWork {
		float timer;
	};
	struct ReviveBehaviorWork {
		TimedCall<void(void)> revicedCall;
	};
	struct EraseBehaviorWork {
		TimedCall<void(void)> despawnTimedCall;
	};

public: // Contsructor/Destructor

public: // Member function
	void initialize(const Vector3& transform, const Vector3& forward);
	void update();
	void begin_rendering();
	void draw() const;
	void draw_marker() const;

private:
	void damaged_callback(const BaseCollider* const other);
	void attack_callback(const BaseCollider* const other);

public: // Getter/Setter
	// ビート状態にする
	void do_beat();
	void pause_beat();
	EnemyBehavior get_now_behavior() const;
	std::weak_ptr<SphereCollider> get_hit_collider();
	std::weak_ptr<SphereCollider> get_beat_collider();
	std::weak_ptr<SphereCollider> get_melee_collider();

private: // BehaviorFunctions
	void spawn_initialize();
	void spawn_update();
	void approach_initialize();
	void approach_update();
	void attack_initialize();
	void attack_update();
	void beating_initialize();
	void beating_update();
	void damaged_heart_initialize();
	void damaged_heart_update();
	void damaged_beat_initialize();
	void damaged_beat_update();
	void down_initialize();
	void down_update();
	void revive_initialize();
	void revive_update();
	void erase_initialize();
	void erase_update();

private: // Member values
	bool isDead;
	int hitpoint; // HP
	int maxHitpoint; // HP
	Vector3 velocity;

	std::unique_ptr<GameObject> ghostMesh;
	std::unique_ptr<GameObject> hitMarkerMesh;

	Behavior<EnemyBehavior> behavior;
	std::variant<
		SpwanBehaviorWork,
		ApproachBehaviorWork,
		AttackBehaviorWork,
		BeatingBehaviorWork,
		DamagedBehaviorWork,
		DownBehaviorWork,
		ReviveBehaviorWork,
		EraseBehaviorWork
	> behaviorValue;

	int markedCount;
	float markingTimer;
	bool isAttakced;

	std::shared_ptr<SphereCollider> hitCollider;

	std::shared_ptr<SphereCollider> meleeCollider;
	std::shared_ptr<SphereCollider> beatCollider;

	ConstantBuffer<float> percentage;

public: // Static value
	inline static const WorldInstance* targetPlayer = nullptr;
	inline static GlobalValues& globalValues = GlobalValues::GetInstance();
	inline static BeatManager* beatManager = nullptr;
	inline static PlayerHPManager* playerHpManager_ = nullptr;
};
