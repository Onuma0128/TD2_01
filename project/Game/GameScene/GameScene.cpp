#include "GameScene.h"

#include <Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Application/Scene/SceneManager.h>
#include <Engine/Render/RenderPath/RenderPath.h>
#include <Engine/Render/RenderPathManager/RenderPathManager.h>
#include <Engine/DirectX/DirectXCore.h>
#include <Engine/Module/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Utility/SmartPointer.h>
#include <Engine/Application/Input/Input.h>

#include "Game/GlobalValues/GlobalValues.h"
#include "Game/TitleScene/TitleScene.h"
#include "Game/GameOverScene/GameOverScene.h"
#include "Game/TutorialScene/TutorialScene.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load() {
	std::string ResourceDirectory = "./Resources/GameScene/";
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models", "Sphere.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models", "ghost_model.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models", "hart.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models", "player_model.obj");
}

void GameScene::initialize() {
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
	collisionManager = eps::CreateUnique<CollisionManager>();

	enemyManager = eps::CreateUnique<EnemyManager>();
	enemyManager->set_collision_manager(collisionManager.get());
	
	beatManager = eps::CreateUnique<BeatManager>();

	BaseEnemy::beatManager = beatManager.get();
	PlayerBullet::beatManager = beatManager.get();
	Player::beatManager = beatManager.get();
	
	timeline = std::make_unique<Timeline>();
	timeline->Initialize();
	timeline->SetEnemyManager(enemyManager.get());
	
	playerHpManager_ = std::make_unique<PlayerHPManager>();
	playerHpManager_->initialize();
	
	BaseEnemy::playerHpManager_ = playerHpManager_.get();
	PlayerBullet::playerHpManager = playerHpManager_.get();
	Player::playerHpManager_ = playerHpManager_.get();

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
	PlayerBullet::player = player_.get();

	collisionManager->register_collider("Player", player_->get_hit_collider());

	create_enemy();
	create_enemy();

	const auto& bullets = player_->get_bullets();
	for (const std::unique_ptr<PlayerBullet>& bullet : bullets) {
		collisionManager->register_collider("Heart", bullet->get_collider());
	}

	BaseEnemy::targetPlayer = player_.get();

#ifdef _DEBUG
	editor = eps::CreateUnique<TimelineEditor>();
	editor->initialize(timeline.get(), camera3D_.get());
#endif // _DEBUG

}

void GameScene::poped() {
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void GameScene::finalize() {
}

void GameScene::begin() {
	player_->begin();
}

void GameScene::update() {
	//camera3D_->update();

	timeline->Update();

	player_->update();
	enemyManager->update();

#ifdef _DEBUG
	if (playerHpManager_->get_hp() <= 0) {
		SceneManager::SetSceneChange(std::make_unique<GameOverScene>(), 1, false);
	}
#endif // DEBUG
}

void GameScene::begin_rendering() {
	camera3D_->update_matrix();
	player_->begin_rendering();
	enemyManager->begin_rendering();

	collisionManager->update();
}

void GameScene::late_update() {
	collisionManager->collision("Player", "EnemyMelee"); // プレイヤーとエネミー近接
	collisionManager->collision("EnemyHit", "Beat"); // エネミーとビート
	collisionManager->collision("EnemyHit", "Heart"); // ハートとエネミー
}

void GameScene::draw() const {
	RenderPathManager::BeginFrame();
	camera3D_->set_command(1);
	player_->draw();
	enemyManager->draw();
#ifdef _DEBUG
	camera3D_->debug_draw();
	collisionManager->debug_draw3d();
	editor->draw_preview();
#endif // _DEBUG


	DirectXCore::ShowGrid();

	RenderPathManager::Next();
	RenderPathManager::Next();
}

#ifdef _DEBUG

void GameScene::debug_update() {
	ImGui::Begin("Camera3D");
	camera3D_->debug_gui();
	ImGui::End();

	ImGui::Begin("GameTimer");
	WorldClock::DebugGui();
	ImGui::End();

	ImGui::Begin("Collision");
	collisionManager->debug_gui();
	ImGui::End();

	GlobalValues::GetInstance().debug_gui();

	player_->debug_gui();

	timeline->debug_gui();
	
	ImGui::Begin("HP");
	ImGui::Text("HP : %d", playerHpManager_->get_hp());
	ImGui::End();

	editor->editor_gui();
}
#endif // _DEBUG

void GameScene::create_enemy() {
	//auto& newEnemy = enemies.emplace_back();
	//newEnemy.initialize();
	//collisionManager->register_collider("EnemyHit", newEnemy.get_hit_collider());
	//collisionManager->register_collider("Beat", newEnemy.get_beat_collider());
	//collisionManager->register_collider("EnemyMelee", newEnemy.get_melee_collider());
}
