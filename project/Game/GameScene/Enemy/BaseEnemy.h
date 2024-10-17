#pragma once

#include <Engine/Module/GameObject/GameObject.h>

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
public:
	enum class Type {
		Normal,
		Strong,
	};

public: // Contsructor/Destructor

public: // Member function
	void initialize(const Vector3& transform, const Vector3& forward, Type type_);
	void begin();
	void update();
	void begin_rendering();
	void draw() const;
	void draw_marker() const;

	void normal_animation();
	void beating_animation();
	void down_animetion();
	void revive_animation();

private:
	void damaged_callback(const BaseCollider* const other);
	void attack_callback(const BaseCollider* const other);

public: // Getter/Setter
	int get_hp() const { return hitpoint; };
	int get_state() const { return static_cast<int>(behavior.state()); };
	// ビート状態にする
	void start_beat();
	void beating();
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

	Type type;

	int hitpoint; // HP
	int maxHitpoint; // HP
	Vector3 velocity;
	Quaternion axisOfQuaternion;

	std::unique_ptr<GameObject> ghostMesh;
	std::unique_ptr<GameObject> hitMarkerMesh;

	Behavior<EnemyBehavior> behavior;

	int markedCount;
	float markingTimer;
	bool isAttakced;
	float behaviorTimer;

	float waveFrameCount;
	float initialY;
	bool isBeatingAnima;

	std::shared_ptr<SphereCollider> hitCollider;

	std::shared_ptr<SphereCollider> meleeCollider;
	std::shared_ptr<SphereCollider> beatCollider;

	ConstantBuffer<float> percentage;

public:
	static void SetApproachSpeed(float speed) { approachSpeed = speed; };

public: // Static value
	inline static const WorldInstance* targetPlayer = nullptr;
	inline static GlobalValues& globalValues = GlobalValues::GetInstance();
	inline static BeatManager* beatManager = nullptr;
	inline static PlayerHPManager* playerHpManager_ = nullptr;

private:
	inline static float approachSpeed = 0.0f;
};
