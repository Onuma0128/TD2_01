#include "GameScene.h"

#include "Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Application/Scene/SceneManager.h"
#include "Engine/Render/RenderPath/RenderPath.h"
#include "Engine/Render/RenderPathManager/RenderPathManager.h"
#include "Engine/DirectX/DirectXCore.h"
#include "Engine/Module/PolygonMesh/PolygonMeshManager.h"

#include "imgui.h"

GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load()
{
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models", "Sphere.obj");
}

void GameScene::initialize()
{
	/*==================== カメラ ====================*/

	camera3D_ = std::make_unique<Camera3D>();
	camera3D_->initialize();
	camera3D_->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(60,0,0),
		{0,40,-25}
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

	RenderPathManager::RegisterPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)), std::move(path));
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));

	/*==================== ゲームオブジェクト ====================*/

	player_ = std::make_unique<Player>();

	enemy = std::make_unique<BaseEnemy>();
	enemy->initialize();

	BaseEnemy::targetPlayer = player_.get();
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
	enemy->begin();
}

void GameScene::update()
{
	//camera3D_->update();
	player_->update();
	enemy->update();
}

void GameScene::begin_rendering()
{
	camera3D_->update_matrix();
	player_->begin_rendering();
	enemy->begin_rendering();
}

void GameScene::late_update()
{
	enemy->late_update();
}

void GameScene::draw() const
{
	RenderPathManager::BeginFrame();
	camera3D_->set_command(1);
	player_->draw();
#ifdef _DEBUG
	camera3D_->debug_draw();
#endif // _DEBUG


	DirectXCore::ShowGrid();
	enemy->draw();

	RenderPathManager::Next();
	RenderPathManager::Next();
}

void GameScene::debug_update()
{
	ImGui::Begin("Camera3D");
	camera3D_->debug_gui();
	ImGui::End();

	ImGui::Begin("GameTimer");
	WorldClock::DebugGui();
	ImGui::End();

	player_->debug_gui();
}