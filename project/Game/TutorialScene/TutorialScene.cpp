#include "TutorialScene.h"

#include <Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Application/Scene/SceneManager.h>
#include <Engine/Render/RenderPath/RenderPath.h>
#include <Engine/Render/RenderPathManager/RenderPathManager.h>
#include <Engine/DirectX/DirectXCore.h>
#include <Engine/Module/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Utility/SmartPointer.h>
#include <Engine/Application/Input/Input.h>

#include "Game/GlobalValues/GlobalValues.h"
#include "Game/GameScene/GameScene.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

TutorialScene::TutorialScene() = default;

TutorialScene::~TutorialScene() = default;

void TutorialScene::load() {
}

void TutorialScene::initialize() {
	GlobalValues::GetInstance().inport_json_all();
	/*==================== カメラ ====================*/

	camera3D_ = std::make_unique<Camera3D>();
	camera3D_->initialize();
	camera3D_->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(49,0,0),
		{0,23,-21}
		});

	/*==================== シーン ====================*/

	object3dNode_ = std::make_unique<Object3DNode>();
	object3dNode_->initialize();
	object3dNode_->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	spriteNode_ = std::make_unique<SpriteNode>();
	spriteNode_->initialize();
	spriteNode_->set_config(eps::to_bitflag(RenderNodeConfig::ContinueDrawAfter) | RenderNodeConfig::ContinueDrawBefore);
	spriteNode_->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	RenderPath path{};
	path.initialize({ object3dNode_,spriteNode_ });

	RenderPathManager::RegisterPath("GameOverScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)), std::move(path));
	RenderPathManager::SetPath("GameOverScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void TutorialScene::poped() {
	RenderPathManager::SetPath("GameOverScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void TutorialScene::finalize() {
}

void TutorialScene::begin() {
}

void TutorialScene::update() {
	//camera3D_->update();
	if (Input::IsTriggerKey(KeyID::Return)) {
		SceneManager::SetSceneChange(std::make_unique<GameScene>(), 1, false);
	}
}

void TutorialScene::begin_rendering() {
	camera3D_->update_matrix();
}

void TutorialScene::late_update() {
}

void TutorialScene::draw() const {
	RenderPathManager::BeginFrame();
	camera3D_->set_command(1);
#ifdef _DEBUG
	camera3D_->debug_draw();
#endif // _DEBUG

	DirectXCore::ShowGrid();

	RenderPathManager::Next();
	RenderPathManager::Next();
}

#ifdef _DEBUG

void TutorialScene::debug_update() {
	ImGui::Begin("Camera3D");
	camera3D_->debug_gui();
	ImGui::End();

	ImGui::Begin("GameTimer");
	WorldClock::DebugGui();
	ImGui::End();

	GlobalValues::GetInstance().debug_gui();
}
#endif // _DEBUG
