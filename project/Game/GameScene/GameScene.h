#pragma once
#include <memory>

#include "Engine/Application/Scene/BaseScene.h"

#include "Engine/Module/Camera/Camera2D.h"
#include "Engine/Module/Camera/Camera3D.h"
#include "Engine/Module/Collision/CollisionManager.h"

#include "Engine/Render/RenderNode/Object3DNode/Object3DNode.h"
#include "Engine/Render/RenderNode/Sprite/SpriteNode.h"
#include "Engine/Render/RenderNode/Particle/ParticleMeshNode/ParticleMeshNode.h"
#include "Game/GameScene/RenderNode/PostEffects/Bloom/BloomNode.h"
#include "Game/GameScene/RenderNode/PostEffects/GaussianBlur/GaussianBlurNode.h"
#include "Game/GameScene/RenderNode/PostEffects/LuminanceExtraction/LuminanceExtractionNode.h"

#include "Game/GameScene/Player/Player.h"
#include "Game/GameScene/Player/PlayerHPManager.h"
#include "Game/GameScene/Player/PlayerSweat.h"
#include "Game/GameScene/BeatManager/BeatManager.h"
#include "Game/GameScene/Timeline/Timeline.h"
#include "Game/GameScene/GameUI/UIManager.h"
#include "Game/GameScene/EnemyManager/EnemyManager.h"
#include "Game/GameScene/RenderNode/CircleGauge/CircleGaugeNode.h"
#include "Game/GameScene/GameOverCamera/GameOverCamera.h"
#include "Game/GameScene/GameUI/Fade/Fade.h"

#ifdef _DEBUG
#include "Game/GameScene/Timeline/TimelineEditor/TimelineEditor.h"
#endif // _DEBUG

class GameScene : public BaseScene
{
public:
	GameScene();
	~GameScene();

public:
	void load()override;
	void initialize()override;
	void poped()override;
	void finalize()override;

	void begin()override;
	void update()override;
	void begin_rendering()override;
	void late_update()override;

	void draw()const override;

#ifdef _DEBUG
	void debug_update()override;
#endif 

private:
	void create_enemy();

private:
	std::unique_ptr<CollisionManager> collisionManager = nullptr;
	std::unique_ptr<BeatManager> beatManager = nullptr;
	std::unique_ptr<Timeline> timeline = nullptr;
	std::unique_ptr<EnemyManager> enemyManager = nullptr;

	std::unique_ptr<Camera3D> camera3D_ = nullptr;
	std::unique_ptr<GameObject> ground_ = nullptr;
	std::unique_ptr<GameOverCamera> gameOverCamera_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<PlayerHPManager> playerHpManager_ = nullptr;
	std::unique_ptr<UIManager> uiManager_ = nullptr;
	std::unique_ptr<Fade> fadeSprite_ = nullptr;

	std::shared_ptr<Object3DNode> object3dNode_ = nullptr;
	std::shared_ptr<ParticleMeshNode> particleMeshNode;
	std::shared_ptr<CircleGaugeNode> circleGaugeNode = nullptr;
	std::shared_ptr<SpriteNode> spriteNode_ = nullptr;
	std::shared_ptr<LuminanceExtractionNode> luminanceExtractionNode = nullptr;
	std::shared_ptr<GaussianBlurNode> gaussianBlurNode = nullptr;
	std::shared_ptr<BloomNode> bloomNode = nullptr;

#ifdef _DEBUG
	std::unique_ptr<TimelineEditor> editor = nullptr;
#endif // _DEBUG
};

