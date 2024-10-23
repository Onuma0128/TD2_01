#include "Engine/WinApp.h"

#include <cstdint>

#include "Engine/Application/Scene/SceneManager.h"
#include "TestCode/SceneDemo.h"
#include "Game/GameScene/GameScene.h"
#include "Game/TitleScene/TitleScene.h"

// クライアント領域サイズ
const std::int32_t kClientWidth = 1280;
const std::int32_t kClientHight = 720;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp::Initialize("2103_ハートビート", kClientWidth, kClientHight);

	WorldClock::IsFixDeltaTime(true);
#ifdef _DEBUG
#endif // _DEBUG

#ifdef _DEBUG
	SceneManager::Initialize(std::make_unique<GameScene>());
#else
	SceneManager::Initialize(std::make_unique<TitleScene>());
#endif // _DEBUG


	WinApp::ShowAppWindow();

	while (true) {
		WinApp::BeginFrame();

		SceneManager::Begin();

		WinApp::ProcessMessage();

		if (WinApp::IsEndApp()) {
			break;
		}

		SceneManager::Update();

		SceneManager::Draw();

#ifdef _DEBUG
		SceneManager::DebugGui();
#endif // _DEBUG

		WinApp::EndFrame();
	}

	WinApp::Finalize();
}
