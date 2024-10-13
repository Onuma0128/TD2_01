#include "Timeline.h"

#define NOMINMAX

#include <fstream>

#include <json.hpp>
#include <Windows.h>

#include <Engine/Application/WorldClock/WorldClock.h>
#include "Engine/Debug/Output.h"

#include "Game/GameScene/EnemyManager/EnemyManager.h"

using json = nlohmann::json;

void Timeline::Initialize() {
	LoadAll();
	nowWave = waveData.begin();
	if (!IsEndWaveAll()) {
		nextPopData = nowWave->popData.begin();
	}
	timer = 0;
}

void Timeline::Update() {
	// 終わっていたら更新しない
	if (IsEndWaveAll()) {
		return;
	}
	timer += WorldClock::DeltaSeconds();
	// 次ウェーブに行く処理
	// 最後まで湧いていて、全員倒した
	if (nextPopData == nowWave->popData.end() && enemyManager->get_enemies().empty()) {
		++nowWave; // ウェーブ進める
		// 最後まで到達してなかったら次の沸きデータを持ってくる
		if (!IsEndWaveAll()) {
			nextPopData = nowWave->popData.begin();
		}
		// 範囲外参照しないためにreturn
		else {
			return;
		}
		// リセット
		timer = 0;
	}

	// 現在ウェーブの更新
	while (nextPopData != nowWave->popData.end() && nextPopData->delay <= timer) {
		// 湧き処理
		enemyManager->create_enemy(nextPopData->translate, nextPopData->forward);

		// 次に進める
		++nextPopData;
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

	// データ取得
	json& data = root.at("PopData");
	// ウェーブ追加
	WaveData& newWaveData = waveData.emplace_back();

	newWaveData.numPop = data.at("NumEnemy");

	for (int i = 0; i < newWaveData.numPop; ++i) {
		// 発生データ
		PopData& popData = newWaveData.popData.emplace_back();
		std::string test = std::format("{:02}", i);
		json& popJson = data.at(test);
		popData.delay = popJson["Delay"];
		popData.translate = { popJson["Translate"].at(0), popJson["Translate"].at(1),popJson["Translate"].at(2) };
		popData.forward = { popJson["Forward"].at(0),popJson["Forward"].at(1),popJson["Forward"].at(2) };
	}

	Console("[Timeline] Successed open wave data. \'{}\'", directoryPath.string());
}

void Timeline::LoadAll() {
	// open file
	const std::filesystem::path settingFile = TimelinePath.string() + "Timeline.json";
	std::ifstream ifstream{ settingFile };

	// 開けなかったらログを出す
	if (ifstream.fail()) {
		std::string message = std::format("[Timeline] Failed open Timeline file. \'{}\'", settingFile.string());
		ConsoleA(message);
		MessageBoxA(nullptr, message.c_str(), "Timeline", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	// Json読み込み
	json root{ json::parse(ifstream) };
	ifstream.close();

	int numWaveData = root.at("NumWaveData");
	waveData.reserve(numWaveData);

	const auto& waveArray = root.at("WaveFiles");

	for (const auto& waveFileName : waveArray) {
		Load(LoadPath.string() + std::format("{}", waveFileName.get<std::string>()));
	}
}

#ifdef _DEBUG
#include <imgui.h>
void Timeline::debug_gui() {
	ImGui::Begin("Timeline");
	ImGui::Text("Wave : %d/%d", std::distance(waveData.begin(), nowWave), waveData.size());
	if (nowWave != waveData.end()) {
		ImGui::Text("Poped : %d/%d", std::distance(nowWave->popData.begin(), nextPopData), nowWave->popData.size());
	}
	else {
		if (!waveData.empty()) {
			auto size = waveData.rbegin()->numPop;
			ImGui::Text("Poped : %d/%d", size, size);
		}
		else {
			ImGui::Text("Poped : 0/0");
		}
	}
	ImGui::Checkbox("Editor", &isActiveEditor);
	ImGui::End();
}
#endif // _DEBUG
