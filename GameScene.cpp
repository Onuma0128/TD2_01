#include "GameScene.h"

#include "Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Application/Scene/SceneManager.h"
#include "Engine/Render/RenderPath/RenderPath.h"
#include "Engine/Render/RenderPathManager/RenderPathManager.h"
#include "Engine/DirectX/DirectXCore.h"

#include "imgui.h"

GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load()
{
}

void GameScene::initialize()
{
	/*==================== カメラ ====================*/

	camera3D_ = std::make_unique<Camera3D>();
	camera3D_->initialize();
	camera3D_->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(45,0,0),
		{0,10,-10}
		});

	/*==================== シーン ====================*/

	object3dNode_ = std::make_unique<Object3DNode>();
	object3dNode_->initialize();
	object3dNode_->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	using RTGConfig = BaseRenderTargetGroup::RTGConfing;

	spriteNode_ = std::make_unique<SpriteNode>();
	spriteNode_->initialize();
	spriteNode_->set_rt_config(eps::to_bitflag(RTGConfig::ContinueDrawAfter) | RTGConfig::ContinueDrawBefore);
	spriteNode_->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	RenderPath path{};
	path.initialize({ object3dNode_,spriteNode_ });

	RenderPathManager::RegisterPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)), std::move(path));
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));

	/*==================== ゲームオブジェクト ====================*/

}

void GameScene::poped()
{
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void GameScene::finalize()
{
}

void GameScene::begin()
{
}

void GameScene::update()
{
	camera3D_->update();
}

void GameScene::begin_rendering()
{
	camera3D_->update_matrix();
}

void GameScene::late_update()
{
}

void GameScene::draw() const
{
	RenderPathManager::BeginFrame();

	DirectXCore::ShowGrid(*camera3D_);

	RenderPathManager::Next();
	RenderPathManager::Next();
}

void GameScene::debug_update()
{
	ImGui::Begin("Camera3D");
	camera3D_->debug_gui();
	ImGui::End();
}