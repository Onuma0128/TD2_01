#pragma once

enum class HP_State {
	None,
	Recovery,
	Damage,
};

class PlayerHPManager {
public:

public:
	void initialize();
	void set_state(HP_State state);
	int get_hp() { return hp_; }
	int get_max_hitpoint() { return maxHitpoint; }
	void reset_max_hp(int hitpoint);

private:
	int hp_ = 0;
	int maxHitpoint;
};