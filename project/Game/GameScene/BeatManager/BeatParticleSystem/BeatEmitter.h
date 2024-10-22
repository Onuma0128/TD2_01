#pragma once

#include "Engine/Module/ParticleSystem/Emitter/BaseEmitter.h"

class BeatEmitter : public BaseEmitter {
public: // Constructor/Destructor
	BeatEmitter() = default;
	~BeatEmitter() = default;

public:
	void initialize() override;
	void on_emit(Particle* const particle);

	void restart() override {}; // 使わない
};
