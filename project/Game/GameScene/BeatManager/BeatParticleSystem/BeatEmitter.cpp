#include "BeatEmitter.h"

#include "Engine/Module/ParticleSystem/Particle/Particle.h"

void BeatEmitter::initialize() {
	isLoop = false;
	duration = 0.1f;
	numEmits = 1;
	isEmit = true;
}

void BeatEmitter::on_emit(Particle* const particle) {
	particle->get_transform().set_translate(this->world_position());
}
