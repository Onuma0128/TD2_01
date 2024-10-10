#pragma once
class PlayerHPManager
{
public:

	enum class HP_State {
		None,
		Recovery,
		Damage,
	};

public:

	void initialize();

	void update();

	HP_State get_state() { return state_; }
	void set_state(HP_State state) { state_ = state; }

	int get_hp() { return hp_; }

private:

	int hp_ = 0;

	HP_State state_;

};