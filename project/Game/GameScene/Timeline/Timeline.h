#pragma once

#include <list>
#include <filesystem>

#include <Engine/Math/Vector3.h>

class BaseEnemy;
class EnemyManager;
class Player;

class Timeline {
#ifdef _DEBUG
	friend class TimelineEditor;
#endif // _DEBUG

	struct PopData {
		float delay;
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
	bool IsEndWaveAll() { return nowWave == waveData.end(); };
	void SetEnemyManager(EnemyManager* enemyManager_) { enemyManager = enemyManager_; };
	void SetPlayer(Player* player_) { player = player_; };

#ifdef _DEBUG
public:
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
};
