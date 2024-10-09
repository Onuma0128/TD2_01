#include "BeatManager.h"

#include "Game/GameScene/Player/PlayerBullet.h"
#include "Game/GameScene/Enemy/BaseEnemy.h"

void BeatManager::set_next_enemy(BaseEnemy* enemy) {
	nextPair.enemy = enemy;
	check_make_pair();
}

void BeatManager::set_next_heart(PlayerBullet* heart) {
	nextPair.heart = heart;
	check_make_pair();
}

bool BeatManager::empty_pair() {
	return enemyBeatPair.empty();
}

void BeatManager::do_beat() {
	for (auto& pair : enemyBeatPair) {
		pair.first->do_beat();
		pair.second->BeatAttack();
	}
}

void BeatManager::pause_beat() {
	for (auto& pair : enemyBeatPair) {
		pair.first->pause_beat();
		pair.second->StopBeat();
	}
}

// 敵が気絶した場合に呼び出す
void BeatManager::enemy_down(BaseEnemy* enemy) {
	if (enemyBeatPair.contains(enemy)) {
		// 削除するポインタの取得
		auto eraseHearts = enemyBeatPair.equal_range(enemy);
		for (auto& itr = eraseHearts.first; itr != eraseHearts.second; ++itr) {
			PlayerBullet*& eraseHeart = itr->second;
			// 止める
			eraseHeart->ComeBack();
			// 登録解除
			heartBeatPair.erase(eraseHeart);
		}
		enemyBeatPair.erase(enemy);
	}
}

// 保持されて10秒経過したら呼び出す
void BeatManager::recovery(BaseEnemy* enemy) {
	if (enemyBeatPair.contains(enemy)) {
		// 削除するポインタの取得
		auto eraseHearts = enemyBeatPair.equal_range(enemy);
		for (auto& itr = eraseHearts.first; itr != eraseHearts.second; ++itr) {
			PlayerBullet*& eraseHeart = itr->second;
			// 削除
			eraseHeart->lost();
			// 登録解除
			heartBeatPair.erase(eraseHeart);
		}
		enemyBeatPair.erase(enemy);
	}
}

void BeatManager::check_make_pair() {
	if (nextPair.enemy && nextPair.heart) {
		enemyBeatPair.emplace(nextPair.enemy, nextPair.heart);
		heartBeatPair.emplace(nextPair.heart, nextPair.enemy);

		nextPair.enemy = nullptr;
		nextPair.heart = nullptr;
	}
}
