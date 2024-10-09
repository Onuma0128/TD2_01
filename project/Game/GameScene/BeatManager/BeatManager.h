#pragma once

#include <unordered_map>

class BaseEnemy;
class PlayerBullet;
class BaseCollider;

class BeatManager {
private:
	struct BeatPair {
		BaseEnemy* enemy;
		PlayerBullet* heart;
	};

	struct EnemyPairMaker {
		BaseEnemy* enemy;
		const BaseCollider* otherCollider;
	};

	struct HeartPairMaker {
		PlayerBullet* heart;
		const BaseCollider* otherCollider;
	};

public:
	void set_next_enemy(BaseEnemy* enemy);
	void set_next_heart(PlayerBullet* heart);
	bool empty_pair();

	void do_beat();
	void pause_beat();
	void enemy_down(BaseEnemy* enemy);
	void recovery(BaseEnemy* enemy);

private:
	void check_make_pair();

private:
	BeatPair nextPair;

	std::unordered_multimap<BaseEnemy*, PlayerBullet*> enemyBeatPair;
	std::unordered_map<PlayerBullet*, BaseEnemy*> heartBeatPair;
};
