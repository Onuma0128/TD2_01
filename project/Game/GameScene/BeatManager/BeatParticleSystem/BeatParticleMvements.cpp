#include "BeatParticleMvements.h"

#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Utility/SmartPointer.h>
#include <Engine/Math/Definition.h>
#include <Engine/Math/Easing.h>

#include <Game/RandomEngine.h>

#include "Game/WorldSystemValue.h"
#include "../BeatManager.h"

Vector3 ConeRandomBias(float angle, const Vector3& baseDireciton) {
	float xAngle = std::lerp(-angle, angle, RandomEngine::Random01MOD());
	float yAngle = RandomEngine::RandomPIClosed();
	Vector3 direction = CVector3::BASIS_Y * Quaternion::EulerRadian(xAngle, yAngle, 0);
	Quaternion rotation = Quaternion::FromToRotation(CVector3::BASIS_Y, baseDireciton);
	return direction * rotation;
}

std::unique_ptr<BaseParticleMovements> BeatParticleMvements::clone() {
	auto&& result = eps::CreateUnique<BeatParticleMvements>();
	result->emitEnemy = this->emitEnemy;
	return result;
}

void BeatParticleMvements::initialize(Particle* const particle) {
	if (beatManager) {
		beatManager->make_collider(particle, emitEnemy, this);
	}

	float InitialAngle = BeatManager::globalValues.get_value<float>("BeatParticle", "InitialMaxAngle") * ToRadian;
	float InitialSpeed = BeatManager::globalValues.get_value<float>("BeatParticle", "InitialMaxSpeed");
	velocity = ConeRandomBias(InitialAngle, CVector3::BASIS_Y) * InitialSpeed;
	lifeTime = BeatManager::globalValues.get_value<float>("BeatParticle", "LifeTime");;
}

void BeatParticleMvements::move(Particle* const particle) {
	timer += WorldClock::DeltaSeconds();
	velocity += GRAVITY * WorldClock::DeltaSeconds();
	particle->get_transform().plus_translate(velocity * WorldClock::DeltaSeconds());
	if (particle->get_transform().get_translate().y <= 0) {
		particle->get_transform().set_translate_y(0.0f);
		float coefficientOfRestitution = BeatManager::globalValues.get_value<float>("BeatParticle", "CoefficientOfRestitution");
		float MinSpeed = BeatManager::globalValues.get_value<float>("BeatParticle", "MinSpeed");
		velocity.y = std::max(MinSpeed, velocity.y * coefficientOfRestitution);
	}
	particle->get_transform().set_scale(
		Lerp(CVector3::BASIS, CVector3::ZERO, Easing::In::Quint(std::clamp(timer / lifeTime, 0.0f, 1.0f)))
	);
	if (timer >= lifeTime || isDestroy) {
		beatManager->destroy_collider(particle);
		particle->set_destroy();
	}
}
