#include "GameScene.h"

#include <Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Application/Scene/SceneManager.h>
#include <Engine/Render/RenderPath/RenderPath.h>
#include <Engine/Render/RenderPathManager/RenderPathManager.h>
#include <Engine/DirectX/DirectXCore.h>
#include <Engine/Module/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Module/TextureManager/TextureManager.h>
#include <Engine/Utility/SmartPointer.h>
#include <Engine/Application/Input/Input.h>

#include "Game/GlobalValues/GlobalValues.h"
#include "Game/TitleScene/TitleScene.h"
#include "Game/GameOverScene/GameOverScene.h"
#include "Game/TutorialScene/TutorialScene.h"

#include "Game/GameScene/GameUI/Wave/WaveSprite.h"
#include "Game/GameScene/GameUI/Hp/HpSprite.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load() {
	std::string ResourceDirectory = "./Resources/GameScene/";
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models", "Sphere.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/enemy", "enemy.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/enemyDamage", "enemyDamage.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/Ghost", "ghost_strong.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/HitMarker", "HitMarker.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models", "hart.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/player", "player.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/playerSweat", "playerSweat.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/ground", "ground.obj");

	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "wave.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "hp.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "esc.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "A_button.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "A_button_push.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "Space_button.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "Space_button_push.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "Attack.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "Beat.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "gameOver.png");
	// Wave用
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "0.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "1.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "2.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "3.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "4.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "5.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "6.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "7.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "8.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "9.png");
	// HP用
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_0.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_1.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_2.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_3.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_4.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_5.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_6.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_7.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_8.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_9.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/hp_numbers", "hp_10.png");
}

void GameScene::initialize() {
	GlobalValues::GetInstance().inport_json_all();
	/*==================== カメラ ====================*/

	Camera2D::Initialize();

	camera3D_ = std::make_unique<Camera3D>();
	camera3D_->initialize();
	camera3D_->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(49,0,0),
		{0,23,-21}
		});

	/*==================== シーン ====================*/
	collisionManager = eps::CreateUnique<CollisionManager>();
	Player::collisionManager = collisionManager.get();

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

	uiManager_ = std::make_unique<UIManager>();
	uiManager_->initialize();
	UIManager::enemyManager_ = enemyManager.get();
	HpSprite::playerHPManager_ = playerHpManager_.get();
	WaveSprite::timeline_ = timeline.get();
	
	BaseEnemy::playerHpManager_ = playerHpManager_.get();
	PlayerBullet::playerHpManager = playerHpManager_.get();
	Player::playerHpManager_ = playerHpManager_.get();

	object3dNode_ = std::make_unique<Object3DNode>();
	object3dNode_->initialize();
	object3dNode_->set_config(eps::to_bitflag(RenderNodeConfig::ContinueDrawBefore) | RenderNodeConfig::ContinueUseDpehtBefore);
	object3dNode_->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	circleGaugeNode = std::make_unique<CircleGaugeNode>();
	circleGaugeNode->initialize();
	circleGaugeNode->set_config(eps::to_bitflag(RenderNodeConfig::ContinueDrawBefore) | RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueUseDpehtAfter);
	circleGaugeNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	spriteNode_ = std::make_unique<SpriteNode>();
	spriteNode_->initialize();
	spriteNode_->set_config(eps::to_bitflag(RenderNodeConfig::ContinueDrawAfter) | RenderNodeConfig::ContinueDrawBefore);
	spriteNode_->set_render_target_SC(DirectXSwapChain::GetRenderTarget());

	RenderPath path{};
	path.initialize({ object3dNode_,circleGaugeNode,spriteNode_ });

	RenderPathManager::RegisterPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)), std::move(path));
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));

	/*==================== ゲームオブジェクト ====================*/

	player_ = std::make_unique<Player>();
	player_->initialize();
	PlayerBullet::player = player_.get();
	PlayerSweat::player = player_.get();
	timeline->SetPlayer(player_.get());

	collisionManager->register_collider("Player", player_->get_hit_collider());

	create_enemy();
	create_enemy();

	BaseEnemy::targetPlayer = player_.get();

	timeline->Start();

	ground_ = std::make_unique<GameObject>("ground.obj");
	ground_->initialize();

	gameOverCamera_ = std::make_unique<GameOverCamera>();
	gameOverCamera_->camera3d_ = camera3D_.get();
	gameOverCamera_->player_ = player_.get();
	gameOverCamera_->initialize();

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
	enemyManager->begin();
}

void GameScene::update() {
	timeline->Update();

	player_->update();
	enemyManager->update();

	uiManager_->update();

	gameOverCamera_->update();
}

void GameScene::begin_rendering() {
	camera3D_->update_matrix();
	player_->begin_rendering();
	enemyManager->begin_rendering();
	uiManager_->begin_rendering();

	collisionManager->update();

	ground_->begin_rendering();
	gameOverCamera_->begin_rendering();
}

void GameScene::late_update() {
	collisionManager->collision("Player", "EnemyMelee"); // プレイヤーとエネミー近接
	collisionManager->collision("EnemyHit", "Beat"); // エネミーとビート
	collisionManager->collision("EnemyHit", "Heart"); // ハートとエネミー
}

void GameScene::draw() const {
	RenderPathManager::BeginFrame();
	// 3Dオブジェ
	camera3D_->set_command(1);
	player_->draw();
	enemyManager->draw();
#ifdef _DEBUG
	camera3D_->debug_draw();
	collisionManager->debug_draw3d();
	editor->draw_preview();
#endif // _DEBUG

	//DirectXCore::ShowGrid();
	ground_->draw();

	RenderPathManager::Next();
	// マーカー
	enemyManager->draw_marker();
	RenderPathManager::Next();
	// スプライト
	uiManager_->draw();

	gameOverCamera_->draw();
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

	enemyManager->debug_gui();
}
#endif // _DEBUG

void GameScene::create_enemy() {
	//auto& newEnemy = enemies.emplace_back();
	//newEnemy.initialize();
	//collisionManager->register_collider("EnemyHit", newEnemy.get_hit_collider());
	//collisionManager->register_collider("Beat", newEnemy.get_beat_collider());
	//collisionManager->register_collider("EnemyMelee", newEnemy.get_melee_collider());
}
