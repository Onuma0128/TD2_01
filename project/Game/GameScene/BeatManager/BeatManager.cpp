#include "BeatManager.h"

#include "Game/GameScene/Player/PlayerBullet.h"
#include "Game/GameScene/Enemy/BaseEnemy.h"

#include <Engine/Utility/SmartPointer.h>
#include <Engine/Module/Collision/Collider/SphereCollider.h>
#include <Engine/Module/ParticleSystem/Particle/Particle.h>
#include <Engine/Module/Collision/CollisionManager.h>
#include <Engine/Application/WorldClock/WorldClock.h>
#include <Engine/Module/GameObject/GameObject.h>

#include "BeatParticleSystem/BeatEmitter.h"
#include "BeatParticleSystem/BeatParticleMvements.h"

#include <ranges>

BeatManager::~BeatManager()
{
	damageAudio_->finalize();
}

void BeatManager::initalize() {
	// ParticleMovementsに登録
	BeatParticleMvements::beatManager = this;

	globalValues.add_value<Vector3>("BeatParticle", "EmitOffset", CVector3::BASIS_Y);
	globalValues.add_value<float>("BeatParticle", "InitialMaxSpeed", 3.0f);
	globalValues.add_value<float>("BeatParticle", "InitialMaxAngle", 45.0f);
	globalValues.add_value<float>("BeatParticle", "LifeTime", 2.0f);
	globalValues.add_value<float>("BeatParticle", "CoefficientOfRestitution", -0.8f);
	globalValues.add_value<float>("BeatParticle", "MinSpeed", 4.5f);
	globalValues.add_value<float>("BeatParticle", "ColliderRadius", 0.24f);
	globalValues.add_value<float>("BeatParticle", "EmitterDuration", 0.1f);
	globalValues.add_value<int>("BeatParticle", "EmitsNum", 1);
	globalValues.add_value<float>("BeatEffect", "FrameInterval", 0.05f);
	globalValues.add_value<float>("BeatEffect", "AnimationTime", 0.5f);

	damageAudio_ = std::make_unique<AudioPlayer>();
	damageAudio_->initialize("enemydamage.wav");
	damageAudio_->set_volume(0.4f);
}

void BeatManager::update() {
	// 1エミッターあたりの更新
	for (ParticleSystemModel& particleSystem : particleSystems) {
		particleSystem.update();
	}

	// パーティクルシステムの処理ががすべて終了していたら削除する
	std::erase_if(particleSystems,
		[&](ParticleSystemModel& ps) {
		if (ps.is_end_all()) {
			unregister_particle(ps);
			ps.finalize();
			return true;
		}
		return false;
	});

	constexpr int NumFrame = 2;
	float FrameInterval = globalValues.get_value<float>("BeatEffect", "FrameInterval");
	float AnimationTime = globalValues.get_value<float>("BeatEffect", "AnimationTime");
	for (BeatEffect& beatEffect : beatEffects) {
		beatEffect.timer += WorldClock::DeltaSeconds();

		float offsetUV = 0.25f * beatEffect.useFrame;
		beatEffect.mesh->get_materials()[0].uvTransform.set_translate_y(
			static_cast<float>(beatEffect.baseUV) + offsetUV
		);

		if (beatEffect.timer - beatEffect.frameCount * FrameInterval > FrameInterval) {
			beatEffect.useFrame ^= 1;
			++beatEffect.frameCount;
		}
	}
	std::erase_if(beatEffects, [AnimationTime](const BeatEffect& beatEffect) {
		return beatEffect.timer >= AnimationTime;
	});
}

void BeatManager::begin_rendering() {
	// 描画準備
	for (ParticleSystemModel& particleSystem : particleSystems) {
		particleSystem.begin_rendering();
	}
	for (BeatEffect& beatEffect : beatEffects) {
		beatEffect.mesh->begin_rendering();
	}
}

void BeatManager::draw() const {
	for (const BeatEffect& beatEffect : beatEffects) {
		beatEffect.mesh->draw();
	}
}

void BeatManager::draw_particle() const {
	// パーティクル描画
	for (const ParticleSystemModel& particleSystem : particleSystems) {
		particleSystem.draw();
	}
}


// ハートとエネミーが衝突した際に記録
void BeatManager::set_next_enemy(BaseEnemy* enemy) {
	nextPair.enemy = enemy;
	check_make_pair();
}

// ハートとエネミーが衝突した際に記録2
void BeatManager::set_next_heart(PlayerBullet* heart) {
	nextPair.heart = heart;
	check_make_pair();
}

bool BeatManager::empty_pair() {
	return enemyBeatPair.empty(); // 片方emptyならどっちもそうなるはず。そうならないとおかしい。
}

// ビート攻撃開始通知
void BeatManager::start_beat() {
	for (auto& pair : enemyBeatPair) {
		pair.first->start_beat();
		pair.second->BeatAttack();
	}
}

// ビート発生タイミングで呼ばれる
void BeatManager::beating() {
	BaseEnemy* prev = nullptr;
	for (BaseEnemy* enemy : enemyBeatPair | std::views::elements<0>) {
		// 重複してびーとさせないようにする
		if (enemy == prev) {
			continue;
		}
		// エネミーにビートしたことを通知
		enemy->beating();
		prev = enemy;

		Vector3 deadTranslate = enemy->world_position();

		// 攻撃パーティクルの初期化
		// エミッター初期化
		auto&& emitter = eps::CreateUnique<BeatEmitter>();
		Vector3 emitoffset = globalValues.get_value<Vector3>("BeatParticle", "EmitOffset");
		emitter->get_transform().set_translate(deadTranslate + emitoffset);
		emitter->update_matrix();
		// Mvements初期化
		auto&& movement = eps::CreateUnique<BeatParticleMvements>();
		movement->set_emit_enemy(enemy);
		// パーティクル追加
		ParticleSystemModel& newParticleSystem = particleSystems.emplace_back();
		newParticleSystem.initialize(32);
		newParticleSystem.set_mesh("beat-particle.obj");
		newParticleSystem.set_emitter(std::move(emitter));
		newParticleSystem.set_particle_movements(std::move(movement));

		// ビートエフェクトの追加
		auto& effect = beatEffects.emplace_back(
			eps::CreateUnique<GameObject>("beating.obj"),
			0.0f, 0.0f, 0, 0
		);
		effect.mesh->initialize();
		effect.mesh->get_transform().set_translate(deadTranslate);
	}
	damageAudio_->restart();
	damageAudio_->play();
}

// ビート攻撃止める
void BeatManager::pause_beat() {
	for (auto& pair : enemyBeatPair) {
		pair.first->pause_beat();
		pair.second->StopBeat();
	}
}

// 敵が気絶した場合に呼び出す
void BeatManager::enemy_down(BaseEnemy* enemy) {
	// ペアの要素に存在するなら削除処理
	if (enemyBeatPair.contains(enemy)) {
		// 削除するポインタの取得
		auto eraseHearts = enemyBeatPair.equal_range(enemy);
		for (auto& itr = eraseHearts.first; itr != eraseHearts.second; ++itr) {
			PlayerBullet*& eraseHeart = itr->second;
			// ハートは戻す
			eraseHeart->ComeBack();
			// 登録解除
			heartBeatPair.erase(eraseHeart);
		}
		// 最後にエネミーキーの方も削除
		enemyBeatPair.erase(enemy);
	}
}

// 保持されて10秒経過したら呼び出す
void BeatManager::recovery(BaseEnemy* enemy) {
	// ペアの要素に存在するなら削除処理
	if (enemyBeatPair.contains(enemy)) {
		// 削除するポインタの取得
		auto eraseHearts = enemyBeatPair.equal_range(enemy);
		for (auto& itr = eraseHearts.first; itr != eraseHearts.second; ++itr) {
			PlayerBullet*& eraseHeart = itr->second;
			// ハートのロスト処理
			eraseHeart->lost();
			// 登録解除
			heartBeatPair.erase(eraseHeart);
		}
		enemyBeatPair.erase(enemy);
	}
}

// エネミーと衝突したパーティクルが、自身が出したものか判定
bool BeatManager::is_self_particle(BaseEnemy* const enemy, const BaseCollider* const other) {
	bool result = false;
	// Colliderが存在し
	if (involeder.contains(other)) {
		// Colliderの発生元Enemyポインタと引数のエネミーが等しい場合true
		result = involeder.at(other).emitEnemy == enemy;
	}
	return result;
}

// パーティクルシステムごと削除
void BeatManager::unregister_particle(const ParticleSystemModel& particleSystem) {
	auto& particles = particleSystem.get_particles();
}

// ParticleにParentするColliderの生成
void BeatManager::make_collider(Particle* const particle, BaseEnemy* const enemy, BeatParticleMvements* const movements_) {
	// メモリ確保
	auto&& newCollider = eps::CreateShared<SphereCollider>();
	newCollider->initialize();
	// パーティクルにColliderをペアレント
	newCollider->set_parent(*particle);
	// Callback関数の設定
	newCollider->set_on_collision_enter(
		std::bind(
			&BeatManager::particle_on_collision,
			this,
			std::placeholders::_1,
			newCollider.get(),
			enemy->get_hit_collider().lock().get()
		)
	);
	// 半径設定
	newCollider->set_radius(globalValues.get_value<float>("BeatParticle", "ColliderRadius"));
	// CollisionManagerに登録
	collisionManager->register_collider("BeatParticle", newCollider);

	// BeatManager側で記録するようの処理
	// 関係物の記録
	auto&& newInvoleder = involeder.emplace(newCollider.get(), BeatColliderInvolved{ enemy, particle , movements_ });
	// 最後にCollider配列に追加
	beatCollider.emplace(particle, newCollider);
}

// Colliderの削除(Particle側から呼び出す用)
void BeatManager::destroy_collider(const Particle* particle) {
	// 削除する要素を検索
	auto&& destroyCollider = beatCollider.at(particle);
	// 順番に削除
	involeder.erase(destroyCollider.get());
	beatCollider.erase(particle);
}

// ハートと敵のペアを作成
void BeatManager::check_make_pair() {
	if (nextPair.enemy && nextPair.heart) {
		enemyBeatPair.emplace(nextPair.enemy, nextPair.heart);
		heartBeatPair.emplace(nextPair.heart, nextPair.enemy);

		nextPair.enemy = nullptr;
		nextPair.heart = nullptr;
	}
}

// コールバック関数
void BeatManager::particle_on_collision(const BaseCollider* const other, SphereCollider* thisCollider, const BaseCollider* const enemyCollider) {
	// 当たったColliderと発生元Colliderが等しかったら判定しない
	if (other == enemyCollider) {
		return;
	}
	// 非アクティブ状態にして、2回判定が発生しないようにする
	thisCollider->set_active(false);
	// movementsに次ループ時に削除するよう要請
	// movements側からColliderの削除申請を行うためこういう処理
	involeder.at(thisCollider).movements->set_next_destroy();
}
