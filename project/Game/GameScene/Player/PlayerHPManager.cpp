#include "PlayerHPManager.h"

void PlayerHPManager::initialize()
{
	hp_ = 10;

	state_ = HP_State::None;
}

void PlayerHPManager::update()
{
	switch (state_)
	{
	case PlayerHPManager::HP_State::None:
		break;
	case PlayerHPManager::HP_State::Recovery:

		hp_++;

		state_ = HP_State::None;

		break;
	case PlayerHPManager::HP_State::Damage:

		hp_--;

		state_ = HP_State::None;

		break;
	default:
		break;
	}

}
