#pragma once

#include <list>
#include <filesystem>

#include <Engine/Math/Vector3.h>

class BaseEnemy;
class EnemyManager;

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

private:
	void Load(const std::filesystem::path& directoryPath);
	void LoadAll();

#ifdef _DEBUG
	void ResetWave(int wave = 0);
#endif // _DEBUG

public:
	bool IsEndWaveAll() { return nowWave == waveData.end(); };
	void SetEnemyManager(EnemyManager* enemyManager_) { enemyManager = enemyManager_; };

#ifdef _DEBUG
public:
	void debug_gui();
#endif // _DEBUG

private:
	float timer;
	std::vector<PopData>::iterator nextPopData;
	std::vector<WaveData>::iterator nowWave;
	std::vector<WaveData> waveData;

	EnemyManager* enemyManager;

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
