#include "ReviveParticleMvements.h"

#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Utility/SmartPointer.h>
#include <Engine/Math/Definition.h>
#include <Engine/Math/Easing.h>
#include <Game/RandomEngine.h>

std::unique_ptr<BaseParticleMovements> ReviveParticleMvements::clone() {
	auto&& result = eps::CreateUnique<ReviveParticleMvements>();
	return result;
}

void ReviveParticleMvements::initialize(Particle* const particle) {
	lifeTime = 0.4f;
	velocity = CVector3::BASIS_Y * 8.0f;
}

void ReviveParticleMvements::move(Particle* const particle) {
	timer += WorldClock::DeltaSeconds();

	float parametric = timer / lifeTime;
	float sin = std::sin(parametric * PI + PI_H);
	Vector3 accel = Vector3::Lerp(CVector3::ZERO, CVector3::BASIS_Y, sin) * 15.0f;
	velocity += accel * WorldClock::DeltaSeconds();
	particle->get_transform().plus_translate(velocity * WorldClock::DeltaSeconds());
	particle->get_transform().set_scale(
		Vector3::Lerp(CVector3::BASIS * 2, Vector3{0,2,2}, Easing::In::Expo(parametric))
	);

	if (timer >= lifeTime) {
		particle->set_destroy();
	}
}
