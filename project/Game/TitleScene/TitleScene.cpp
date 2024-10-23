#include "TitleScene.h"

#include <Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Application/Scene/SceneManager.h>
#include <Engine/Render/RenderPath/RenderPath.h>
#include <Engine/Render/RenderPathManager/RenderPathManager.h>
#include <Engine/DirectX/DirectXCore.h>
#include <Engine/Module/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Utility/SmartPointer.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Module/TextureManager/TextureManager.h>

#include "Game/GlobalValues/GlobalValues.h"
#include "Game/GameScene/GameScene.h"
#include "Game/GameScene/Timeline/GameState.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

TitleScene::TitleScene() = default;

TitleScene::~TitleScene() {
	clickAudio_->finalize();
}

void TitleScene::load() {
	std::string ResourceDirectory = "./Resources/GameScene/";
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/gameSprite", "fade.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/gameSprite", "title.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/gameSprite", "titleBack.png");
	AudioManager::RegisterLoadQue(ResourceDirectory + "Audio", "click.wav");
}

void TitleScene::initialize() {
	GlobalValues::GetInstance().inport_json_all();
	/*==================== カメラ ====================*/

	camera3D_ = std::make_unique<Camera3D>();
	camera3D_->initialize();
	camera3D_->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(49,0,0),
		{0,23,-21}
		});

	Camera2D::Initialize();

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

	RenderPathManager::RegisterPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)), std::move(path));
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));

	GameState::getInstance().setCurrentWave(0);

	fadeSprite_ = std::make_unique<Fade>();
	fadeSprite_->initialize();

	titleSprite_ = std::make_unique<SpriteObject>("title.png", Vector2{ 0.5f,0.5f });
	titleBackSprite_ = std::make_unique<SpriteObject>("titleBack.png", Vector2{ 0.5f,0.5f });
	titleSprite_->set_translate({ 640,360 });
	titleBackSprite_->set_translate({ 640,360 });

	clickAudio_ = std::make_unique<AudioPlayer>();
	clickAudio_->initialize("click.wav");
}

void TitleScene::poped() {
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void TitleScene::finalize() {
}

void TitleScene::begin() {
}

void TitleScene::update() {
	//camera3D_->update();
	fadeSprite_->update();

	if (Input::IsReleaseKey(KeyID::Space) || Input::IsReleasePad(PadID::A) && !isGameScene_) {
		SceneManager::SetSceneChange(std::make_unique<GameScene>(), 1, false);
		fadeSprite_->set_state(Fade::FadeState::FadeIN);
		clickAudio_->restart();
		clickAudio_->play();
		isGameScene_ = true;
	}
}

void TitleScene::begin_rendering() {
	camera3D_->update_matrix();

	titleBackSprite_->begin_rendering();
	titleSprite_->begin_rendering();
	fadeSprite_->begin_rendering();
}

void TitleScene::late_update() {
}

void TitleScene::draw() const {
	RenderPathManager::BeginFrame();
	camera3D_->set_command(1);
#ifdef _DEBUG
	camera3D_->debug_draw();
#endif // _DEBUG


	DirectXCore::ShowGrid();

	RenderPathManager::Next();

	titleBackSprite_->draw();
	titleSprite_->draw();
	fadeSprite_->draw();
	RenderPathManager::Next();
}

#ifdef _DEBUG

void TitleScene::debug_update() {
	ImGui::Begin("Camera3D");
	camera3D_->debug_gui();
	ImGui::End();

	ImGui::Begin("GameTimer");
	WorldClock::DebugGui();
	ImGui::End();

	GlobalValues::GetInstance().debug_gui();
}
#endif // _DEBUG
