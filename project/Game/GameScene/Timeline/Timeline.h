#pragma once

#include <filesystem>
#include <vector>

#include "Game/GameScene/Enemy/BaseEnemy.h"
#include <Engine/Math/Vector3.h>

class EnemyManager;
class WaveSprite;
class Player;

class Timeline {
#ifdef _DEBUG
	friend class TimelineEditor;
#endif // _DEBUG

	struct PopData {
		float delay;
		BaseEnemy::Type type;
		Vector3 translate;
		Vector3 forward;
	};

	struct WaveData {
		int playerHitpoint;
		float enemyApproachSpeed;
		std::vector<PopData> popData;
	};
public:
	Timeline() = default;
	~Timeline() = default;

	Timeline(const Timeline&) = delete;
	Timeline& operator=(const Timeline&) = delete;

public:
	void Initialize();
	void Update();

	void Start();

private:
	void Load(const std::filesystem::path& directoryPath);
	void LoadAll();

	void ResetNowWave();

public:
	bool is_all_enemy_poped() const;
	bool IsEndWaveAll() { return nowWave == waveData.end(); };
	void SetEnemyManager(EnemyManager* enemyManager_) { enemyManager = enemyManager_; };
	void SetPlayer(Player* player_) { player = player_; }

	const std::vector<WaveData>& GetWaveData() { return waveData; }
	std::vector<WaveData>::const_iterator GetNowWave() const { return nowWave; }

#ifdef _DEBUG
public:
	bool GetIsActiveEditor()const { return isActiveEditor; }
	bool GetisDemoPlay()const { return isDemoPlay; }
	void debug_gui();

private:
	void ResetWaveDebug(int wave = 0);
#endif // _DEBUG

private:
	float timer;
	std::vector<PopData>::iterator nextPopData;
	std::vector<WaveData>::iterator nowWave;
	std::vector<WaveData> waveData;

	EnemyManager* enemyManager;
	Player* player;

#ifdef _DEBUG
	bool isActiveEditor;
	bool isDemoPlay;
#endif // _DEBUG

private:
	static const inline std::filesystem::path TimelinePath
	{ "./Resources/GameScene/Timeline/" };
	static const inline std::filesystem::path LoadPath
	{ TimelinePath.string() + "WaveData/" };

#ifdef _DEBUG
public:
	WaveSprite* waveSprite = nullptr;
#endif // _DEBUG
};
