#pragma once

#include <memory>

#include "Engine/Render/RenderNode/ChromaticAberration/ChromaticAberrationNode.h"

class Player;

enum class PostEffectState {
	NANE,
	DAMANGE,
	BEATING,
};

class PostEffectManager {
public:

public:
	PostEffectManager() = default;
	~PostEffectManager() = default;

public:
	void initialize(
		const std::shared_ptr<ChromaticAberrationNode>& chromaticAberrationNode
	);
	void update();

private:
	void damange_update();
	void beating_update();

public:
	void set_reaction(PostEffectState state);
	void beat_cycle(float cycle);

#ifdef _DEBUG
public:
	void debug_gui();
#endif // _DEBUG

private:
	std::shared_ptr<ChromaticAberrationNode> chromaticAberrationNode;

	PostEffectState state;
	float animationTimer = 0;
	float cycle = 0;
};
