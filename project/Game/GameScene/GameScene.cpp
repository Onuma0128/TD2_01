#include "GameScene.h"

#include <Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Application/Scene/SceneManager.h>
#include <Engine/Render/RenderPath/RenderPath.h>
#include <Engine/Render/RenderPathManager/RenderPathManager.h>
#include <Engine/DirectX/DirectXCore.h>
#include <Engine/Module/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Utility/SmartPointer.h>

#include "Game/GlobalValues/GlobalValues.h"

#include "imgui.h"

GameScene::GameScene() = default;

GameScene::~GameScene() = default;

void GameScene::load() {
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models", "Sphere.obj");
}

void GameScene::initialize() {
	GlobalValues::GetInstance().inport_json_all();
	/*==================== カメラ ====================*/

	camera3D_ = std::make_unique<Camera3D>();
	camera3D_->initialize();
	camera3D_->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(60,0,0),
		{0,40,-25}
		});

	/*==================== シーン ====================*/
	collisionManager = eps::CreateUnique<CollisionManager>();
	beatManager = eps::CreateUnique<BeatManager>();
	BaseEnemy::beatManager = beatManager.get();
	PlayerBullet::beatManager = beatManager.get();
	Player::beatManager = beatManager.get();

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
}

void GameScene::poped() {
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void GameScene::finalize() {
}

void GameScene::begin() {
	player_->begin();
	for (BaseEnemy& enemy : enemies) {
		enemy.begin();
	}
}

void GameScene::update() {
	//camera3D_->update();
	player_->update();
	for (BaseEnemy& enemy : enemies) {
		enemy.update();
	}

	enemies.remove_if([](const BaseEnemy& enemy) {return !enemy.is_active(); });
}

void GameScene::begin_rendering() {
	camera3D_->update_matrix();
	player_->begin_rendering();
	for (BaseEnemy& enemy : enemies) {
		enemy.begin_rendering();
	}

	collisionManager->update();
}

void GameScene::late_update() {
	collisionManager->collision("Player", "EnemyMelee"); // プレイヤーとエネミー近接
	collisionManager->collision("EnemyHit", "Beat"); // エネミーとビート
	collisionManager->collision("EnemyHit", "Heart"); // ハートとエネミー
	for (BaseEnemy& enemy : enemies) {
		enemy.late_update();
	}
}

void GameScene::draw() const {
	RenderPathManager::BeginFrame();
	camera3D_->set_command(1);
	player_->draw();
#ifdef _DEBUG
	camera3D_->debug_draw();
	collisionManager->debug_draw3d();
#endif // _DEBUG


	DirectXCore::ShowGrid();
	for (const BaseEnemy& enemy : enemies) {
		enemy.draw();
	}

	RenderPathManager::Next();
	RenderPathManager::Next();
}

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
}

void GameScene::create_enemy() {
	auto& newEnemy = enemies.emplace_back();
	newEnemy.initialize();
	collisionManager->register_collider("EnemyHit", newEnemy.get_hit_collider());
	collisionManager->register_collider("Beat", newEnemy.get_beat_collider());
	collisionManager->register_collider("EnemyMelee", newEnemy.get_melee_collider());
}
