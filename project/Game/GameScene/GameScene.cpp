#include "GameScene.h"

#include <Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h>
#include <Engine/Render/RenderPath/RenderPath.h>
#include <Engine/DirectX/DirectXCore.h>
#include <Engine/Utility/SmartPointer.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Render/RenderTargetGroup/SwapChainRenderTargetGroup.h>
#include <Engine/Render/RenderTargetGroup/SingleRenderTarget.h>
#include <Engine/DirectX/DirectXResourceObject/OffscreenRender/OffscreenRender.h>

#include <Engine/Application/Scene/SceneManager.h>
#include <Engine/Render/RenderPathManager/RenderPathManager.h>
#include <Engine/Module/PolygonMesh/PolygonMeshManager.h>
#include <Engine/Module/TextureManager/TextureManager.h>
#include "Engine/Application/Audio/AudioManager.h"

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

GameScene::~GameScene() {
	gameBGM_->finalize();
}

void GameScene::load() {
	std::string ResourceDirectory = "./Resources/GameScene/";
	PolygonMeshManager::RegisterLoadQue("./EngineResources/Models", "Sphere.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/enemy", "enemy.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/enemyDamage", "enemyDamage.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/bigEnemy", "bigEnemy.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/bigEnemyDamage", "bigEnemyDamage.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/HitMarker", "HitMarker.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models", "hart.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/player", "player.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/playerSweat", "playerSweat.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/ground", "ground.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/Particle", "beat-particle.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/Effects/Beat", "beating.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/Effects/EnemyRevive", "EnemyRevive.obj");
	PolygonMeshManager::RegisterLoadQue(ResourceDirectory + "Models/speaker", "speaker.obj");

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
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "clear.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "clearback.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "beatComment.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "gameover_comment.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "cleartext_title.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/UI", "cleartext_retry.png");
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
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/numbers", "10.png");
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
	// GameSprite
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/gameSprite", "fade.png");
	TextureManager::RegisterLoadQue(ResourceDirectory + "Textures/gameSprite", "allclear.png");
	// Audio
	AudioManager::RegisterLoadQue(ResourceDirectory + "Audio", "BGM.wav");
	AudioManager::RegisterLoadQue(ResourceDirectory + "Audio", "enemydamage.wav");
	AudioManager::RegisterLoadQue(ResourceDirectory + "Audio", "playerdamage.wav");
	AudioManager::RegisterLoadQue(ResourceDirectory + "Audio", "throw.wav");
	AudioManager::RegisterLoadQue(ResourceDirectory + "Audio", "click.wav");
	AudioManager::RegisterLoadQue(ResourceDirectory + "Audio", "clear.wav");
	AudioManager::RegisterLoadQue(ResourceDirectory + "Audio", "allclear.wav");
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
	BeatManager::collisionManager = collisionManager.get();

	enemyManager = eps::CreateUnique<EnemyManager>();
	enemyManager->set_collision_manager(collisionManager.get());
	enemyManager->initialize();

	beatManager = eps::CreateUnique<BeatManager>();
	beatManager->initalize();

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
	WaveSprite::enemyManager_ = enemyManager.get();
	Player::waveSprite_ = uiManager_.get()->get_waveSprite();
	
	BaseEnemy::playerHpManager_ = playerHpManager_.get();
	PlayerBullet::playerHpManager = playerHpManager_.get();
	Player::playerHpManager_ = playerHpManager_.get();

	postEffectManager = eps::CreateUnique<PostEffectManager>();

	renderTarget = eps::CreateShared<SingleRenderTarget>();
	renderTarget->initialize();

	object3dNode_ = std::make_unique<Object3DNode>();
	object3dNode_->initialize();
	object3dNode_->set_config(eps::to_bitflag(RenderNodeConfig::ContinueDrawBefore) | RenderNodeConfig::ContinueUseDpehtBefore);
	object3dNode_->set_render_target(renderTarget);

	particleMeshNode = std::make_unique<ParticleMeshNode>();
	particleMeshNode->initialize();
	particleMeshNode->set_config(eps::to_bitflag(RenderNodeConfig::ContinueDrawBefore) | RenderNodeConfig::ContinueUseDpehtBefore | RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueUseDpehtAfter);
	particleMeshNode->set_render_target(renderTarget);

	circleGaugeNode = std::make_unique<CircleGaugeNode>();
	circleGaugeNode->initialize();
	circleGaugeNode->set_config(eps::to_bitflag(RenderNodeConfig::ContinueDrawBefore) | RenderNodeConfig::ContinueDrawAfter | RenderNodeConfig::ContinueUseDpehtAfter);
	circleGaugeNode->set_render_target(renderTarget);

	spriteNode_ = std::make_unique<SpriteNode>();
	spriteNode_->initialize();
	spriteNode_->set_config(RenderNodeConfig::ContinueDrawAfter);
	spriteNode_->set_render_target(renderTarget);

	luminanceExtractionNode = eps::CreateShared<LuminanceExtractionNode>();
	luminanceExtractionNode->initialize();
	luminanceExtractionNode->set_render_target();
	luminanceExtractionNode->set_texture_resource(spriteNode_->result_stv_handle());

	gaussianBlurNode = eps::CreateShared<GaussianBlurNode>();
	gaussianBlurNode->initialize();
	gaussianBlurNode->set_render_target();
	gaussianBlurNode->set_texture_resource(luminanceExtractionNode->result_stv_handle());

	bloomNode = eps::CreateShared<BloomNode>();
	bloomNode->initialize();
	bloomNode->set_render_target();
	bloomNode->set_base_texture(spriteNode_->result_stv_handle());
	bloomNode->set_blur_texture(gaussianBlurNode->result_stv_handle());

	chromaticAberrationNode = eps::CreateShared<ChromaticAberrationNode>();
	chromaticAberrationNode->initialize();
	chromaticAberrationNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
	chromaticAberrationNode->set_texture_resource(bloomNode->result_stv_handle());

	RenderPath path{};
	path.initialize({ object3dNode_,particleMeshNode,circleGaugeNode,spriteNode_, luminanceExtractionNode, gaussianBlurNode, bloomNode,chromaticAberrationNode });

	postEffectManager->initialize(chromaticAberrationNode);
	//postEffectManager->(player_.get());

	// 背景色の設定
	GlobalValues::GetInstance().add_value<Vector3>("GameConfig", "BackgroundColor", { 0.015f, 0.015f, 0.015f });
	Vector3 clearColor = GlobalValues::GetInstance().get_value<Vector3>("GameConfig", "BackgroundColor");
	renderTarget->offscreen_render().set_claer_color({ clearColor.x, clearColor.y, clearColor.z, 1.0f });

	RenderPathManager::RegisterPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)), std::move(path));
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));

	/*==================== ゲームオブジェクト ====================*/

	player_ = std::make_unique<Player>();
	player_->initialize();
	PlayerBullet::player = player_.get();
	PlayerSweat::player = player_.get();
	timeline->SetPlayer(player_.get());
	player_->set_posteffect_manager(postEffectManager.get());

	collisionManager->register_collider("Player", player_->get_hit_collider());

	BaseEnemy::targetPlayer = player_.get();

	timeline->Start();

	ground_ = std::make_unique<GameObject>("ground.obj");
	ground_->initialize();

	gameOverCamera_ = std::make_unique<GameOverCamera>();
	gameOverCamera_->camera3d_ = camera3D_.get();
	gameOverCamera_->player_ = player_.get();
	gameOverCamera_->initialize();
	UIManager::gameOverCamera_ = gameOverCamera_.get();

	fadeSprite_ = std::make_unique<Fade>();
	fadeSprite_->initialize();
	GameOverCamera::fadeSprite_ = fadeSprite_.get();
	WaveSprite::fadeSprite_ = fadeSprite_.get();

	// Audio
	gameBGM_ = std::make_unique<AudioPlayer>();
	gameBGM_->initialize("BGM.wav");
	gameBGM_->set_loop(true);
	gameBGM_->set_volume(0.3f);
	gameBGM_->play();

	speaker_ = std::make_unique<Speaker>();
	speaker_->initialize();


#ifdef _DEBUG
	editor = eps::CreateUnique<TimelineEditor>();
	editor->initialize(timeline.get(), camera3D_.get());
	timeline->waveSprite = uiManager_->get_waveSprite();
#endif // _DEBUG

}

void GameScene::poped() {
	RenderPathManager::SetPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void GameScene::finalize() {
	renderTarget->finalize();
	object3dNode_->finalize();
	particleMeshNode->finalize();
	circleGaugeNode->finalize();
	spriteNode_->finalize();
	luminanceExtractionNode->finalize();
	gaussianBlurNode->finalize();
	bloomNode->finalize();
	chromaticAberrationNode->finalize();
	gameBGM_->finalize();
	RenderPathManager::UnregisterPath("GameScene" + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void GameScene::begin() {
	player_->begin();
	enemyManager->begin();

#ifdef _DEBUG
	Vector3 clearColor = GlobalValues::GetInstance().get_value<Vector3>("GameConfig", "BackgroundColor");

	renderTarget->offscreen_render().set_claer_color({ clearColor.x, clearColor.y, clearColor.z, 1.0f });
#endif // _DEBUG

}

void GameScene::update() {
	timeline->Update();

	player_->update();
	enemyManager->update();
	speaker_->update();

	beatManager->update();

	uiManager_->update();

	gameOverCamera_->update();
	fadeSprite_->update();
	postEffectManager->update();
}

void GameScene::begin_rendering() {
	camera3D_->update_matrix();
	player_->begin_rendering();
	enemyManager->begin_rendering();
	beatManager->begin_rendering();
	uiManager_->begin_rendering();
	speaker_->begin_rendering();

	collisionManager->update();

	ground_->begin_rendering();
	gameOverCamera_->begin_rendering();
	fadeSprite_->begin_rendering();
}

void GameScene::late_update() {
	collisionManager->collision("Player", "EnemyMelee"); // プレイヤーとエネミー近接
	collisionManager->collision("EnemyHit", "Beat"); // エネミーとビート
	collisionManager->collision("EnemyHit", "BeatParticle"); // エネミーとビートパーティクル
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
	//DirectXCore::ShowGrid();
#endif // _DEBUG

	ground_->draw();
	speaker_->draw();
	beatManager->draw();

	RenderPathManager::Next();
	// 3Dパーティクル
	camera3D_->set_command(1);
	beatManager->draw_particle();
	enemyManager->draw_particle();

	RenderPathManager::Next();
	// マーカー
	camera3D_->set_command(1);
	enemyManager->draw_marker();
	RenderPathManager::Next();
	// スプライト
	uiManager_->draw();

	gameOverCamera_->draw();
	fadeSprite_->draw();

	// PE開始
	// Bloom
	RenderPathManager::Next();
	luminanceExtractionNode->draw();
	RenderPathManager::Next();
	gaussianBlurNode->draw();
	RenderPathManager::Next();
	bloomNode->draw();
	RenderPathManager::Next();
	// ここまでBloom
	RenderPathManager::Next();
	// 色収差
	chromaticAberrationNode->draw();
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

	speaker_->debug_gui();

	timeline->debug_gui();

	ImGui::Begin("HP");
	ImGui::Text("HP : %d", playerHpManager_->get_hp());
	ImGui::End();

	editor->editor_gui();

	enemyManager->debug_gui();

	postEffectManager->debug_gui();
	ImGui::Begin("BloomEffect");
	if (ImGui::CollapsingHeader("Bloom")) {
		if (ImGui::TreeNode("Luminiance")) {
			luminanceExtractionNode->debug_gui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("GaussianBlur")) {
			gaussianBlurNode->debug_gui();
			ImGui::TreePop();
		}
		bloomNode->debug_gui();
	}
	ImGui::End();
}
#endif // _DEBUG
