#pragma once

#include "Engine/Module/ParticleSystem/Emitter/BaseEmitter.h"

class ReviveEmitter : public BaseEmitter {
public: // Constructor/Destructor
	ReviveEmitter() = default;
	~ReviveEmitter() = default;

public:
	void initialize() override;
	void update() override;
	void on_emit(Particle* const particle);

	void restart() override {}; // 使わない

private:
	float emitTimer;
	float emitTime;
	float radius;
};
