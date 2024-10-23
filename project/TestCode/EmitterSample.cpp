#include "EmitterSample.h"

#include <Game/RandomEngine.h>

void EmitterSample::initialize() {
	isLoop = true;
	duration = 3;
	numEmits = 3;
	isEmit = false;
}

void EmitterSample::update() {
	isEmit = false;
	BaseEmitter::update();
	//if (std::fmod(timer, 1.0f) < WorldClock::DeltaSeconds()) {
	if (true) {
		isEmit = true;
	}
}

void EmitterSample::restart() {
	timer = 0;
}

void EmitterSample::on_emit(Particle* const particle) {
	Vector3 offset = {
		std::lerp(-10.0f, 10.0f, RandomEngine::Random01Closed()),
		0,
		std::lerp(-10.0f, 10.0f, RandomEngine::Random01Closed())
	};
	particle->get_transform().set_translate(
		this->world_position() + offset
	);
}
