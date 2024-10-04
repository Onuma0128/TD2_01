#pragma once

#include <Engine/Module/GameObject/GameObject.h>

#include <optional>
#include <functional>
#include <vector>
#include <variant>

#include <Engine/Utility/TimedCall/TimedCall.h>
#include <Engine/Module/Collision/Collider/SphereCollider.h>

enum class EnemyBehavior {
	Spawn,
	Approach,
	Attack,
	Beating,
	Damaged,
	Despawn,
};

class Player;

class BaseEnemy : public GameObject {
private: // Structs
	struct SpwanBehaviorWork {
		TimedCall<void(void)> timedCall;
	};
	struct ApproachBehaviorWork {
		float attackDistance;
		float speed;
	};
	struct AttackBehaviorWork {
		std::shared_ptr<SphereCollider> attackCollider;
		TimedCall<void(void)> attackTimedCall;
	};
	struct BeatingBehaviorWork {
		std::shared_ptr<SphereCollider> beatCollider;
	};
	struct DamagedBehaviorWork {
		TimedCall<void(void)> damagedTimedCall;
	};
	struct DespawnBehaviorWork {
		TimedCall<void(void)> despawnTimedCall;
	};

public: // Contsructor/Destructor

public: // Member function
	void initialize();
	void update() override;

public: // Getter/Setter
	// ビート状態にする
	void do_beat();

private: // BehaviorFunctions
	void spawn_initialize();
	void spawn_update();
	void approach_initialize();
	void approach_update();
	void attack_initialize();
	void attack_update();
	void beating_initialize();
	void beating_update();
	void damaged_initialize();
	void damaged_update();
	void despawn_initialize();
	void despawn_update();

private: // Member values
	bool isDead;
	int32_t hitpoint; // HP
	std::shared_ptr<SphereCollider> collider;

	EnemyBehavior behavior;
	std::variant<
		SpwanBehaviorWork,
		ApproachBehaviorWork,
		AttackBehaviorWork,
		BeatingBehaviorWork,
		DamagedBehaviorWork,
		DespawnBehaviorWork
	> behaviorValue;
	std::optional<EnemyBehavior> behaviorRequest;

	std::vector<std::function<void(void)>> behaviorUpdateFunc;
	std::vector<std::function<void(void)>> behaviorInitFunc;

public: // Static value
	inline static const Player* targetPlayer = nullptr;
};
