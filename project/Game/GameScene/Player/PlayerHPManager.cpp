#include "PlayerHPManager.h"

#include "Game/GlobalValues/GlobalValues.h"

void PlayerHPManager::initialize() {
	GlobalValues::GetInstance().add_value<int>("Player", "NumBullets", 10);
	hp_ = GlobalValues::GetInstance().get_value<int>("Player", "NumBullets");
}

void PlayerHPManager::set_state(HP_State state) {
	switch (state) {
	case HP_State::None:
		break;
	
	case HP_State::Recovery:
		++hp_;
		break;

	case HP_State::Damage:
		--hp_;
		break;
	
	default:
		break;
	}
}
