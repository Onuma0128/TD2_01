#include "ReviveEmitter.h"

#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Module/ParticleSystem/Particle/Particle.h>

#include "Game/GlobalValues/GlobalValues.h"
#include <Game/RandomEngine.h>

void ReviveEmitter::initialize() {
	static GlobalValues& globalValues = GlobalValues::GetInstance();
	isLoop = false;
	duration = globalValues.get_value<float>("ReviveEmitter", "EmitterDuration");
	emitTime = globalValues.get_value<float>("ReviveEmitter", "EmitTime");
	numEmits = globalValues.get_value<int>("ReviveEmitter", "EmitsNum");
	radius = globalValues.get_value<float>("ReviveEmitter", "EmitsRadius");
	isEmit = true;
}

void ReviveEmitter::update() {
	isEmit = false;
	timer += WorldClock::DeltaSeconds();
	emitTimer += WorldClock::DeltaSeconds();
	if (emitTimer > emitTime) {
		emitTimer = std::fmod(emitTimer, emitTime);
		isEmit = true;
	}
}

void ReviveEmitter::on_emit(Particle* const particle) {
	float distance = std::sqrt(RandomEngine::Random01Closed()) * radius;
	float theta = RandomEngine::RandomPIClosed();
	Vector3 offset = {
		distance * std::cos(theta),
		RandomEngine::Random01Closed() * 0.2f + 0.2f ,
		distance * std::sin(theta)
	};
	particle->get_transform().set_translate(this->world_position() + offset);
}
