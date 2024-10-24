#include "GameOverCamera.h"

#include "Engine/Module/Camera/Camera3D.h"
#include "Engine/Application/WorldClock/WorldClock.h"
#include "Engine/Application/Scene/SceneManager.h"
#include "Engine/Application/Input/Input.h"
#include "Engine/Application/Input/InputEnum.h"

#include "Game/GameScene/Player/Player.h"
#include "Game/GameScene/Player/PlayerBullet.h"
#include "Game/GameScene/GameUI/Fade/Fade.h"
#include "Game/GameOverScene/GameOverScene.h"
#include "Game/TitleScene/TitleScene.h"
#include "Game/GameScene/GameScene.h"


GameOverCamera::GameOverCamera() = default;

GameOverCamera::~GameOverCamera()
{
	clickAudio_->finalize();
}

void GameOverCamera::initialize()
{
	globalValues.add_value<Vector3>("gameOverCamera", "CameraOffset", { 0.0f,2.0f,9.0f });

	gameOverSprite_ = std::make_unique<SpriteObject>("gameOver.png", Vector2{ 0.5f,0.5f });
	gameOverSprite_->set_translate({ 1920,300 });
	gameOverCommentSprite_ = std::make_unique<SpriteObject>("gameover_comment.png", Vector2{ 0.5f,0.5f });
	gameOverCommentSprite_->set_translate({ 1920,160 });

	state_ = CameraState::Stop;

	clickAudio_ = std::make_unique<AudioPlayer>();
	clickAudio_->initialize("click.wav");
	clickAudio_->set_volume(0.2f);

	isEsc_ = false;
}

void GameOverCamera::update()
{
	switch (state_)
	{
	case GameOverCamera::CameraState::Move:
		Move();
		break;
	case GameOverCamera::CameraState::Stop:
		Stop();
		/*if (Input::IsReleaseKey(KeyID::Escape) && !isEsc_ && WaveSprite::WaveState::Normal == waveSprite_->get_state()) {
			SceneManager::SetSceneChange(std::make_unique<TitleScene>(), 1, false);
			fadeSprite_->set_state(Fade::FadeState::FadeIN);
			clickAudio_->restart();
			clickAudio_->play();
			isEsc_ = true;
		}*/
		break;
	case GameOverCamera::CameraState::GameOverSprite:
		SpriteMove();
		if (cameraFrame_ > 3.0f) {
			if (Input::IsReleaseKey(KeyID::Space) || Input::IsReleasePad(PadID::A)) {
				SceneManager::SetSceneChange(std::make_unique<GameScene>(), 1, false);
				fadeSprite_->set_state(Fade::FadeState::FadeIN);
				clickAudio_->restart();
				clickAudio_->play();
				cameraFrame_ = 0;
			}
			if (cameraFrame_ > 8.0f) {
				SceneManager::SetSceneChange(std::make_unique<TitleScene>(), 1, false);
				fadeSprite_->set_state(Fade::FadeState::FadeIN);
				clickAudio_->restart();
				clickAudio_->play();
				cameraFrame_ = 0;
			}
		}
		break;
	default:
		break;
	}
}

void GameOverCamera::Move()
{
	if (isMoveCamera_) {
		cameraFrame_ += WorldClock::DeltaSeconds();
		float t = easeOutQuint(cameraFrame_ - 0.5f);
		t = std::clamp(t, 0.0f, 1.0f);
		if (cameraFrame_ >= 0.5f) {
			camera3d_->look_at(Vector3::Lerp(CVector3::BASIS_Y, player_->get_transform().get_translate() + CVector3::BASIS_Y, t));
			camera3d_->get_transform().set_translate(Vector3::Lerp(oldPos_, playerPos_, t));
		}
		if (t >= 1.0f) {
			state_ = CameraState::GameOverSprite;
			cameraFrame_ = 0.0f;
			oldSpritePos_ = gameOverSprite_->get_transform().get_translate();
		}
	}
}

void GameOverCamera::Stop()
{
	if (player_->get_state() == Player::State::Dead && !isMoveCamera_) {
		isMoveCamera_ = true;
		oldPos_ = camera3d_->world_position();
		playerPos_ = (globalValues.get_value<Vector3>("gameOverCamera", "CameraOffset")) * player_->get_transform().get_quaternion() + player_->world_position();
		state_ = CameraState::Move;
	}
	int playerHpCount = 0;
	for (const auto& bullet : player_->get_bullets()) {
		if (bullet->is_active()) {
			++playerHpCount;
		}
	}
	if (playerHpCount <= 1 && state_ == CameraState::Stop) {
		state_ = CameraState::GameOverSprite;
		cameraFrame_ = 0.0f;
		oldSpritePos_ = gameOverSprite_->get_transform().get_translate();
	}
}

void GameOverCamera::SpriteMove()
{
	cameraFrame_ += WorldClock::DeltaSeconds();
	float t = easeOutQuint(cameraFrame_ - 1.0f);
	t = std::clamp(t, 0.0f, 1.0f);
	if (cameraFrame_ >= 1.0f && !isMoveCamera_) {
		gameOverCommentSprite_->set_translate(Vector2::Lerp(Vector2{ 1920,160 }, Vector2{ 640,160 }, t));
	}
	t = easeOutQuint(cameraFrame_ - 2.0f);
	t = std::clamp(t, 0.0f, 1.0f);
	if (cameraFrame_ >= 2.0f) {
		gameOverSprite_->set_translate(Vector2::Lerp(oldSpritePos_, Vector2{ 640,300 }, t));
	}
}

void GameOverCamera::begin_rendering()
{
	if (state_ == CameraState::GameOverSprite) {
		gameOverSprite_->begin_rendering();
		gameOverCommentSprite_->begin_rendering();
	}
}

void GameOverCamera::draw()
{
	if (state_ == CameraState::GameOverSprite) {
		gameOverSprite_->draw();
		gameOverCommentSprite_->draw();
	}
}

float GameOverCamera::easeOutQuint(float t)
{
	return 1 - powf(1 - t, 5);
}
