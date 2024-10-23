#include "PostEffectManager.h"

#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Math/Definition.h>

#include <Game/RandomEngine.h>

void PostEffectManager::initialize(const std::shared_ptr<ChromaticAberrationNode>& chromaticAberrationNode_) {
	chromaticAberrationNode = chromaticAberrationNode_;
}

void PostEffectManager::update() {
	animationTimer += WorldClock::DeltaSeconds();
	switch (state) {
	case PostEffectState::NANE:
		chromaticAberrationNode->set_length(0);
		break;
	case PostEffectState::DAMANGE:
		damange_update();
		break;
	case PostEffectState::BEATING:
		beating_update();
		break;
	default:
		break;
	}
}

void PostEffectManager::damange_update() {
	chromaticAberrationNode->set_length(std::lerp(0.0f, 0.03f, RandomEngine::Random01MOD()));
	if (animationTimer > 0.5f) {
		state = PostEffectState::NANE;
	}
}

void PostEffectManager::beating_update() {
	float parametric = (animationTimer / cycle);
	float sinParametric = std::sin(parametric * PI);
	chromaticAberrationNode->set_length(
		std::lerp(0.0f, 0.02f, sinParametric) +
		std::lerp(0.0f, 0.01f, RandomEngine::Random01MOD() * sinParametric)
	);
	if (animationTimer > cycle) {
		state = PostEffectState::NANE;
	}
}

void PostEffectManager::set_reaction(PostEffectState state_) {
	state = state_;
	animationTimer = 0;
}

void PostEffectManager::beat_cycle(float cycle_) {
	cycle = cycle_;
}

#ifdef _DEBUG
#include <externals/imgui/imgui.h>
void PostEffectManager::debug_gui() {
	ImGui::Begin("ChromaticAberration");
	chromaticAberrationNode->debug_gui();
	ImGui::End();
}
#endif // _DEBUG
