#include "Timeline.h"

#define NOMINMAX

#include <fstream>

#include <json.hpp>
#include <Windows.h>

#include <Engine/Application/WorldClock/WorldClock.h>
#include "Engine/Debug/Output.h"

#include "Game/GameScene/EnemyManager/EnemyManager.h"
#include "Game/GameScene/Player/Player.h"
#include "Game/GameScene/Timeline/GameState.h"
#include "Game/GameScene/GameUI/Wave/WaveSprite.h"

using json = nlohmann::json;

void Timeline::Initialize() {
	LoadAll();
	// シングルトンから現在のWave番号を取得し、そのWaveからスタート
	int currentWave = GameState::getInstance().getCurrentWave();
	if (currentWave < waveData.size()) {
		nowWave = waveData.begin() + currentWave;
	}
	else {
		nowWave = waveData.begin();
	}
	timer = 0;
}

void Timeline::Update() {
#ifdef _DEBUG
	if (isActiveEditor && !isDemoPlay) {
		return;
	}
#endif // _DEBUG

	// 終わっていたら更新しない
	if (IsEndWaveAll()) {
		return;
	}
	timer += WorldClock::DeltaSeconds();
	// 次ウェーブに行く処理
	// 最後まで湧いていて、全員倒した
	if (nextPopData == nowWave->popData.end() && enemyManager->get_enemies().empty()) {
		++nowWave; // ウェーブ進める
		ResetNowWave();

		if (IsEndWaveAll()) {
			return;
		}
	}

	// 現在ウェーブの更新
	while (nextPopData != nowWave->popData.end() && nextPopData->delay <= timer) {
		// 湧き処理
		enemyManager->create_enemy(nextPopData->translate, nextPopData->forward, nextPopData->type);

		// 次に進める
		++nextPopData;
	}
}

void Timeline::Start() {
	// シングルトンからWave番号をロードして開始
	int currentWave = GameState::getInstance().getCurrentWave();
	nowWave = waveData.begin() + currentWave;
	if (!IsEndWaveAll()) {
		ResetNowWave();
	}
}

void Timeline::Load(const std::filesystem::path& directoryPath) {
	// open file
	std::ifstream ifstream{ directoryPath };

	// 開けなかったらログを出す
	if (ifstream.fail()) {
		std::string message = std::format("[Timeline] Failed open file. \'{}\'", directoryPath.string());
		ConsoleA(message);
		MessageBoxA(nullptr, message.c_str(), "Timeline", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	// Json読み込み
	json root{ json::parse(ifstream) };
	// 閉じる
	ifstream.close();

	// ウェーブ追加
	WaveData& newWaveData = waveData.emplace_back();

	if (root.contains("PlayerHitPoint")) {
		newWaveData.playerHitpoint = root["PlayerHitPoint"];
	}
	if (root.contains("EnemyApproachSpeed")) {
		newWaveData.enemyApproachSpeed = root["EnemyApproachSpeed"];
	}

	// データ取得
	json& data = root.at("PopData");

	for (int i = 0; i < data.size(); ++i) {
		// 発生データ
		PopData& popData = newWaveData.popData.emplace_back();
		std::string test = std::format("{:02}", i);
		json& popJson = data.at(test);
		popData.delay = popJson["Delay"];
		popData.type = static_cast<BaseEnemy::Type>(popJson["Type"]);
		popData.translate = { popJson["Translate"].at(0), popJson["Translate"].at(1),popJson["Translate"].at(2) };
		popData.forward = { popJson["Forward"].at(0),popJson["Forward"].at(1),popJson["Forward"].at(2) };
	}

	Console("[Timeline] Successed open wave data. \'{}\'\n", directoryPath.string());
}

void Timeline::LoadAll() {
	// open file
	const std::filesystem::path settingFile = TimelinePath.string() + "Timeline.json";
	std::ifstream ifstream{ settingFile };

	// 開けなかったらログを出す
	if (ifstream.fail()) {
		std::string message = std::format("[Timeline] Failed open Timeline file. \'{}\'\n", settingFile.string());
		ConsoleA(message);
		MessageBoxA(nullptr, message.c_str(), "Timeline", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	// Json読み込み
	json root{ json::parse(ifstream) };
	ifstream.close();

	const auto& waveArray = root.at("WaveFiles");

	for (const auto& waveFileName : waveArray) {
		Load(LoadPath.string() + std::format("{}", waveFileName.get<std::string>()));
	}
}

void Timeline::ResetNowWave() {
	// 最後まで到達してなかったら次の沸きデータを持ってくる
	if (!IsEndWaveAll()) {
		nextPopData = nowWave->popData.begin();
		player->reset_hitpoint(nowWave->playerHitpoint);
		BaseEnemy::SetApproachSpeed(nowWave->enemyApproachSpeed);
		// 現在のWave番号をシングルトンに保存
		GameState::getInstance().setCurrentWave(static_cast<int>(std::distance(waveData.begin(), nowWave)));

	}
	// リセット
	timer = 0;
}

#ifdef _DEBUG

#include <imgui.h>

void Timeline::debug_gui() {
	ImGui::Begin("Timeline");
	if (ImGui::Checkbox("Editor", &isActiveEditor)) {
		ResetWaveDebug(0);
	}
	if (isActiveEditor && !isDemoPlay) {
		ImGui::Text("Editting");
	}
	else {
		ImGui::Text("Wave : %d/%d", std::distance(waveData.begin(), nowWave), waveData.size());
		if (nowWave != waveData.end()) {
			ImGui::Text("Poped : %d/%d", std::distance(nowWave->popData.begin(), nextPopData), nowWave->popData.size());
		}
		else {
			if (!waveData.empty()) {
				auto size = waveData.rbegin()->popData.size();
				ImGui::Text("Poped : %d/%d", size, size);
			}
			else {
				ImGui::Text("Poped : 0/0");
			}
		}
	}
	ImGui::End();
}

void Timeline::ResetWaveDebug(int wave) {
	enemyManager->clear();
	nowWave = waveData.begin() + wave;
	ResetNowWave();
	waveSprite->reset();
}
#endif // _DEBUG
