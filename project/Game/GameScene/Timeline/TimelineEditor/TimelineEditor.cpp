#ifdef _DEBUG

#include "TimelineEditor.h"

#define NOMINMAX

#include <fstream>

#include <imgui.h>
#include <json.hpp>

#include <Engine/Application/Input/Input.h>
#include <Engine/WinApp.h>
#include <Engine/Module/Camera/Camera3D.h>
#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Math/Definition.h>

#include "Game/GameScene/EnemyManager/EnemyManager.h"

using json = nlohmann::json;

bool InRange(const Vector2& value, const Vector2& min, const Vector2& max) {
	return value == Vector2{ std::clamp(value.x, min.x, max.x), std::clamp(value.y, min.y, max.y) };
}

void TimelineEditor::initialize(Timeline* timeline_, Camera3D* camera_) {
	assert(timeline_);
	assert(camera_);
	timeline = timeline_;
	camera = camera_;
	forawardAngle = 0;
}

void TimelineEditor::editor_gui() {
	if (!timeline->isActiveEditor) {
		return;
	}
	input();
	get_nearest_enemy();

	ImGui::Begin("Editor");

	if (ImGui::Button("Export")) {
		export_json_all();
	}

	demoplay();

	if (timeline->isDemoPlay) {
		ImGui::End();
		return;
	}

	config();

	wave_editor();

	pop_list();

	ImGui::End();

	detail_window();

	operation();
}

void TimelineEditor::draw_preview() {
	if (!timeline->isActiveEditor || !editWave.has_value() || timeline->isDemoPlay) {
		return;
	}
	Timeline::WaveData& waveData = timeline->waveData[editWave.value()];
	for (int i = 0; GameObject & object : previews) {
		object.get_transform().set_translate(waveData.popData[i].translate);
		object.get_transform().set_translate_y(1.0f);
		object.get_transform().set_quaternion(Quaternion::LookForward(waveData.popData[i].forward));
		object.begin_rendering();
		object.draw();
		++i;
	}
}

void TimelineEditor::input() {
	Vector2 mousePosition = Input::MousePosition();
	// ウィンドウサイズ取得
	static const Vector2 windowSize = {
		static_cast<float>(WinApp::GetClientWidth()),
		static_cast<float>(WinApp::GetClientHight()) };
	// マウス位置
	// 範囲外なら何もしない
	if (InRange(mousePosition, CVector2::ZERO, windowSize)) {
		Ray mouseRay{};
		Matrix4x4 vpvInv = (camera->vp_matrix_debug() * Camera3D::MakeViewportMatrix(CVector2::ZERO, windowSize, 0, 1)).inverse();
		// nearとfarの取得
		Vector3 nearMouseRay = Transform3D::Homogeneous(mousePosition.convert(0), vpvInv);
		Vector3 farMouseRay = Transform3D::Homogeneous(mousePosition.convert(1), vpvInv);
		mouseRay.origin = nearMouseRay;
		mouseRay.direction = (farMouseRay - nearMouseRay).normalize_safe();
		// 地面との距離を取る
		float t = (1.0f - Vector3::DotProduct(mouseRay.origin, CVector3::BASIS_Y)) / Vector3::DotProduct(mouseRay.direction, CVector3::BASIS_Y);
		// マウスRayが地面と当たる位置を算出
		std::optional<Vector3> preRayToGround = rayToGround;
		rayToGround = mouseRay.origin + mouseRay.direction * t;
		if (preRayToGround.has_value()) {
			rayToGroundDelta = rayToGround.value() - preRayToGround.value();
		}
		else {
			rayToGroundDelta = CVector3::ZERO;
		}
	}
	else {
		rayToGround = std::nullopt;
		rayToGroundDelta = CVector3::ZERO;
	}

	constexpr float PressTime = 0.2f;
	// マウスクリック
	mouseLeftCrickTimer += WorldClock::DeltaSeconds();
	if (Input::IsTriggerMouse(MouseID::Left)) {
		// 連続クリックだったらダブルクリック
		if (mouseLeftCrickTimer < PressTime) {
			mouseState = MouseState::DoubleCrick;
		}
		// クリックしたらタイマーリセット
		mouseLeftCrickTimer = 0;
	}
	// 長押し状態の場合
	if (mouseState == MouseState::None &&
		Input::IsPressMouse(MouseID::Left) &&
		(mouseLeftCrickTimer >= PressTime || rayToGroundDelta != CVector3::ZERO)) {
		mouseState = MouseState::SingleCrick;
	}
	// 離したら入力解除
	if (Input::IsReleaseMouse(MouseID::Left)) {
		mouseState = MouseState::None;
	}
}

void TimelineEditor::export_json_all() {
	sort_all_wave_deta();

	json root;
	root["WaveFiles"] = json::array();
	auto& array = root["WaveFiles"];
	for (int i = 0; i < timeline->waveData.size(); ++i) {
		array.push_back(std::format("{:02}.json", i));
	}
	std::ofstream ofstream{ Timeline::TimelinePath.string() + "Timeline.json", std::ios::trunc };
	ofstream << std::setfill('\t') << std::setw(1) << root;
	ofstream.close();

	for (int j = 0; auto & wave : timeline->waveData) {
		json waveJson;
		waveJson["PlayerHitPoint"] = wave.playerHitpoint;
		waveJson["EnemyApproachSpeed"] = wave.enemyApproachSpeed;
		waveJson["PopData"] = json::object();
		json& popDataJson = waveJson["PopData"];
		for (int i = 0; auto & pop : wave.popData) {
			std::string objName = std::format("{:02}", i);
			popDataJson[objName] = json::object();
			popDataJson[objName]["Delay"] = pop.delay;
			popDataJson[objName]["Type"] = pop.type;
			popDataJson[objName]["Translate"] = json::array({ pop.translate.x,pop.translate.y,pop.translate.z });
			popDataJson[objName]["Forward"] = json::array({ pop.forward.x,pop.forward.y,pop.forward.z });
			++i;
		}
		ofstream.open(Timeline::LoadPath.string() + std::format("{:02}.json", j), std::ios::trunc);
		ofstream << std::setfill('\t') << std::setw(1) << waveJson;
		ofstream.close();
		++j;
	}
}

void TimelineEditor::demoplay() {
	if (ImGui::CollapsingHeader("Demoplay")) {
		if (timeline->isDemoPlay) {
			if (ImGui::Button("Stop")) {
				stop_demoplay();
			}
		}
		else {
			if (ImGui::Button("Run")) {
				if (editWave.has_value()) {
					run_demoplay();
				}
			}
			ImGui::Checkbox("DemoplayEditWave", &isDemoplayEditWave);
		}
	}
}

void TimelineEditor::config() {
	if (ImGui::CollapsingHeader("Config")) {
		ImGui::RadioButton("Create", (int*)&editType, EditType::Create); ImGui::SameLine();
		ImGui::RadioButton("Edit", (int*)&editType, EditType::Edit); ImGui::SameLine();
		ImGui::RadioButton("Erase", (int*)&editType, EditType::Delete);
	}
}

void TimelineEditor::wave_editor() {
	if (ImGui::CollapsingHeader("WaveEditor")) {
		// ウェーブ数編集
		static int NumWave = static_cast<int>(timeline->waveData.size());
		ImGui::SetNextItemWidth(100.0f);
		ImGui::InputInt("NumWave", &NumWave);
		NumWave = std::max(NumWave, 0);
		ImGui::SameLine();
		if (ImGui::Button("Apply")) {
			timeline->waveData.resize(NumWave);
			if (editWave.has_value() && editWave.value() >= timeline->waveData.size()) {
				reset_wave(std::nullopt);
			}
		}
		// ウェーブ選択ラジオボタン
		for (int i = 0; i < static_cast<int>(timeline->waveData.size()); ++i) {
			if (ImGui::RadioButton(std::format("{: >2}##Wave", i + 1).c_str(), editWave == i)) {
				reset_wave(i);
			}
			if (i % 5 != 4 && i + 1 != timeline->waveData.size()) {
				ImGui::SameLine();
			}
		}
		// 編集中ウェーブの設定
		if (editWave.has_value()) {
			Timeline::WaveData& wave = timeline->waveData[editWave.value()];
			ImGui::SetNextItemWidth(100.0f);
			ImGui::InputInt("PlayerHitpoint", &wave.playerHitpoint);
			ImGui::SetNextItemWidth(100.0f);
			ImGui::DragFloat("EnemyApproachSpeed", &wave.enemyApproachSpeed, 0.1f);

		}
	}
}

void TimelineEditor::pop_list() {
	if (ImGui::CollapsingHeader("PopList")) {
		if (timeline->waveData.empty() || !editWave.has_value()) {
			return;
		}
		for (int i = 0; auto & popData : timeline->waveData[editWave.value()].popData) {
			if (ImGui::Selectable(std::format("{: >2}##List", i).c_str(), selectPopData == i)) {
				reset_select(i);
			}
			++i;
		}
	}
}

void TimelineEditor::get_nearest_enemy() {
	if (rayToGround.has_value() && !timeline->waveData.empty() && editWave.has_value()) {
		for (int i = 0; i < timeline->waveData[editWave.value()].popData.size(); ++i) {
			Timeline::PopData& popData = timeline->waveData[editWave.value()].popData[i];
			if (((popData.translate + CVector3::BASIS_Y) - rayToGround.value()).length() < 1.0f) {
				nearestIndex = i;
				return;
			}
		}
		nearestIndex = std::nullopt;
	}
}

void TimelineEditor::detail_window() {
	if (!selectPopData.has_value() || !editWave.has_value()) {
		return;
	}

	bool isOpen = true;
	ImGui::Begin("DetailWindow", &isOpen);
	Timeline::PopData& popData = timeline->waveData[editWave.value()].popData[selectPopData.value()];

	ImGui::DragFloat("Delay", &popData.delay, 0.01f, 0.0f, std::numeric_limits<float>::infinity());
	ImGui::DragFloat3("Translate", &popData.translate.x, 0.01f);
	if (ImGui::DragFloat("Forward", &forawardAngle, 0.1f)) {
		popData.forward = CVector3::BASIS_Z * Quaternion::EulerDegree(0, forawardAngle, 0);
	}
	if (ImGui::RadioButton("Normal##Detail", reinterpret_cast<int*>(&popData.type), static_cast<int>(BaseEnemy::Type::Normal))) {
		previews[selectPopData.value()].reset_object("ghost_model.obj");
		popData.type = BaseEnemy::Type::Normal;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Strong##Detail", reinterpret_cast<int*>(&popData.type), static_cast<int>(BaseEnemy::Type::Strong))) {
		previews[selectPopData.value()].reset_object("ghost_strong.obj");
		popData.type = BaseEnemy::Type::Strong;
	}

	ImGui::End();

	// Windowが閉じられたら表示しないようにする
	if (!isOpen) {
		reset_select(std::nullopt);
	}
}

void TimelineEditor::operation() {
	if (!rayToGround.has_value() || !editWave.has_value() || ImGui::GetIO().WantCaptureMouse) {
		return;
	}
	Vector3& groundPosition = rayToGround.value();
	switch (editType) {
		// 生成
	case TimelineEditor::Create:
		if (Input::IsTriggerMouse(MouseID::Left)) {
			auto& newData = timeline->waveData[editWave.value()].popData.emplace_back(
				0.0f,
				BaseEnemy::Type::Normal,
				groundPosition,
				CVector3::BASIS_Z
			);
			newData.translate.y = 0;
			previews.emplace_back("ghost_model.obj");
		}
		break;

		// 編集
	case TimelineEditor::Edit:
		// 近いやつがなければ何もしない
		if (!nearestIndex.has_value()) {
			return;
		}
		switch (mouseState) {
		case TimelineEditor::MouseState::None:
			// do nothing
			break;
		case TimelineEditor::MouseState::SingleCrick:
			// 移動
		{
			timeline->waveData[editWave.value()].popData[nearestIndex.value()].translate += rayToGroundDelta;
		}
		break;
		case TimelineEditor::MouseState::DoubleCrick:
			reset_select(nearestIndex);
			break;
		default:
			break;
		}
		break;

		// 削除
	case TimelineEditor::Delete:
		if (Input::IsTriggerMouse(MouseID::Left)) {
			if (nearestIndex.has_value()) {
				// 選択を解除
				reset_select(std::nullopt);
				// 要素を削除
				auto& popData = timeline->waveData[editWave.value()].popData;
				popData.erase(
					popData.begin() + nearestIndex.value()
				);
				previews.pop_back();
			}
		}
		break;
	default:
		break;
	}

}

void TimelineEditor::reset_wave(std::optional<int> wave) {
	editWave = wave;
	if (editWave.has_value()) {
		Timeline::WaveData& waveData = timeline->waveData[editWave.value()];
		previews.resize(waveData.popData.size());
		for (int i = 0; i < waveData.popData.size(); ++i) {
			previews[i].get_transform().set_translate(waveData.popData[i].translate);
			previews[i].reset_object("ghost_model.obj");
		}
	}
	else {
		previews.clear();
	}
	reset_select(std::nullopt);
}

void TimelineEditor::reset_select(std::optional<int> value) {
	selectPopData = value;
	if (selectPopData.has_value() && editWave.has_value()) {
		Vector3& forward = timeline->waveData[editWave.value()].popData[selectPopData.value()].forward;
		forawardAngle = std::acos(Vector3::DotProduct(CVector3::BASIS_Z, forward)) * ToDegree;
	}
}

void TimelineEditor::run_demoplay() {
	if (!editWave.has_value()) {
		return;
	}
	timeline->isDemoPlay = true;
	sort_all_wave_deta();
	auto waveBegin = timeline->waveData.begin();
	if (isDemoplayEditWave) {
		timeline->ResetWaveDebug(editWave.value());
	}
	else {
		timeline->ResetWaveDebug(0);
	}
}

void TimelineEditor::stop_demoplay() {
	timeline->isDemoPlay = false;
	timeline->enemyManager->clear();
}

void TimelineEditor::sort_all_wave_deta() {
	for (Timeline::WaveData& wave : timeline->waveData) {
		std::sort(wave.popData.begin(), wave.popData.end(),
			[](const Timeline::PopData& rhs, const Timeline::PopData& lhs) {
			return rhs.delay < lhs.delay;
		});
	}
}

#endif // _DEBUG
