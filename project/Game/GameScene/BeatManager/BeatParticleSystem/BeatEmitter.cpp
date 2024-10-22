#include "BeatEmitter.h"

#include "Engine/Module/ParticleSystem/Particle/Particle.h"

#include <Game/GlobalValues/GlobalValues.h>

void BeatEmitter::initialize() {
	static GlobalValues& globalValues = GlobalValues::GetInstance();
	isLoop = false;
	duration = globalValues.get_value<float>("BeatParticle", "EmitterDuration");
	numEmits = globalValues.get_value<int>("BeatParticle", "EmitsNum");
	isEmit = true;
}

void BeatEmitter::on_emit(Particle* const particle) {
	particle->get_transform().set_translate(this->world_position());
}
