#pragma once
#include <memory>

#include "Engine/Render/RenderNode/Object3DNode/Object3DNode.h"
#include "Engine/Render/RenderNode/Sprite/SpriteNode.h"
#include "Engine/Application/Scene/BaseScene.h"

#include "Engine/Module/Camera/Camera3D.h"

class TitleScene : public BaseScene
{
public:
	TitleScene();
	~TitleScene();

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

	std::unique_ptr<Camera3D> camera3D_ = nullptr;

	std::shared_ptr<Object3DNode> object3dNode_ = nullptr;
	std::shared_ptr<SpriteNode> spriteNode_ = nullptr;
};

