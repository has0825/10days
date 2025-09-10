#include "GameScene.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <numbers>
#include <string>

using namespace KamataEngine;

// ランダム補助
static float RandomRange(float min, float max) { return min + (max - min) * (float(rand()) / float(RAND_MAX)); }

void GameScene::Initialize() {
	// カメラ（真上俯瞰）
	camera_.Initialize();
	camera_.translation_ = {0.0f, 40.0f, 0.0f};
	camera_.rotation_.x = ToRadians(90.0f);
	camera_.UpdateMatrix();
	cameraPtr_ = &camera_; // ★ Skydome に渡す用

	// モデル
	modelBase_ = Model::CreateFromOBJ("base");          // コア見た目
	modelBlockRing_ = Model::CreateFromOBJ("circle");   // リング
	modelBlockPaddle_ = Model::CreateFromOBJ("paddle"); // パドル
	modelShot_ = Model::CreateFromOBJ("PlayerBullet");  // 弾
	modelEnemy_ = Model::CreateFromOBJ("meteorite");    // 敵
	modelSkydome_ = Model::CreateFromOBJ("universedome");

	// HUD
	hud_.Initialize("Font.png");

	// 円環 WT
	ringSegWT_.reserve(kRingSegments);
	for (int i = 0; i < kRingSegments; ++i) {
		auto wt = std::make_unique<WorldTransform>();
		wt->Initialize();
		ringSegWT_.push_back(std::move(wt));
	}

	paddleSegWT_.reserve(kPaddleSegments);
	for (int i = 0; i < kPaddleSegments; ++i) {
		auto wt = std::make_unique<WorldTransform>();
		wt->Initialize();
		paddleSegWT_.push_back(std::move(wt));
	}

	// 2本目パドル用
	paddleSegWT2_.reserve(kPaddleSegments);
	for (int i = 0; i < kPaddleSegments; ++i) {
		auto wt = std::make_unique<WorldTransform>();
		wt->Initialize();
		paddleSegWT2_.push_back(std::move(wt));
	}

	coreWT_ = std::make_unique<WorldTransform>();
	coreWT_->Initialize();

	// コンテナ確保
	shots_.reserve(128);
	enemies_.reserve(128);

	// 初期配置計算
	RecomputePaddleHalfWidth();
	UpdateRingAndPaddle(0.0f);

	score_ = 0;
	skill_ = 0;
	timer_ = 0;
	life_ = 3;
	shield_ = 0;
	timerAcc_ = 0.0f;

	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, cameraPtr_);

	// Audio のインスタンス取得
	auto* audio = Audio::GetInstance();

	// BGM 読み込み（WAV形式）
	bgmHandle_ = audio->LoadWave("./BGM/EVOLUTION.wav");

	// ループ再生 (volume=0.5)
	bgmVoice_ = audio->PlayWave(bgmHandle_, true, 0.5f);

}

void GameScene::Update() {
	const float dt = 1.0f / 60.0f;

	// 背景
	if (skydome_)
		skydome_->Update();

	// タイマー（秒）
	timerAcc_ += dt;
	if (timerAcc_ >= 1.0f) {
		timerAcc_ -= 1.0f;
		timer_++;
	}

	// 強化・進化の段階をスコアで自動適用
	ApplyProgression();

	// クールダウン減少
	if (shotCooldownNow_ > 0.0f) {
		shotCooldownNow_ = (std::max)(0.0f, shotCooldownNow_ - dt);
	}

	// パドル回転
	if (Input::GetInstance()->PushKey(DIK_D)) {
		paddle_.angle -= paddle_.angularSpeed * dt;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		paddle_.angle += paddle_.angularSpeed * dt;
	}
	paddle_.angle = WrapAngle(paddle_.angle);

	// 発射（SPACE + クールダウン）
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && shotCooldownNow_ <= 0.0f) {
		SpawnShot();
		float cdMul = 1.0f - 0.1f * float(std::clamp(paddleLevel_, 0, 3));
		shotCooldownNow_ = baseShotCooldown_ * (std::max)(0.3f, cdMul); // 下限30%
	}

	// 自動砲台（コア進化）
	UpdateTurret(dt);

	// ---- 動的スポーン：時間＆スコアで毎秒出現数を増やす ----
	float spawnRate = enemySpawnBaseRate_ + enemySpawnRateGrowthPerSec_ * static_cast<float>(timer_) // 経過秒
	                  + enemySpawnRatePerScore_ * static_cast<float>(score_);                        // スコア

	// 上限でクランプ
	if (spawnRate > enemySpawnRateMax_)
		spawnRate = enemySpawnRateMax_;
	if (spawnRate < 0.0f)
		spawnRate = 0.0f;

	// 期待値を蓄積して、1.0 を超えるたびに 1体スポーン
	enemySpawnAcc_ += spawnRate * dt;
	while (enemySpawnAcc_ >= 1.0f) {
		enemySpawnAcc_ -= 1.0f;
		SpawnEnemy();
	}
	// ----

	// 連続反射のタイムアウト
	if (comboTimer_ > 0.0f) {
		comboTimer_ -= dt;
		if (comboTimer_ <= 0.0f) {
			paddleCombo_ = 0;
			scoreMul_ = 1.0f;
		}
	}

	// 更新
	UpdateRingAndPaddle(dt);
	UpdateShots(dt);
	UpdateEnemies(dt);

	camera_.UpdateMatrix();
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// === 3D ===
	Model::PreDraw(dxCommon->GetCommandList());
	if (skydome_)
		skydome_->Draw();
	DrawRingAndPaddle();
	DrawShots();
	DrawEnemies();
	Model::PostDraw();

	// === HUD ===
	Sprite::PreDraw(dxCommon->GetCommandList());
	hud_.DrawTimer(timer_);
	hud_.DrawScore(score_);
	hud_.DrawLife(life_);
	hud_.DrawSkill(skill_);
	Sprite::PostDraw();
}

// ==================== 円環 ====================
void GameScene::UpdateRingAndPaddle(float /*dt*/) {
	// リング
	const int N = kRingSegments;
	for (int i = 0; i < N; ++i) {
		float a = -PI + (2.0f * PI) * (i / static_cast<float>(N));
		float inner = ringR_ - ringThickness_ * 0.5f;
		float outer = ringR_ + ringThickness_ * 0.5f;
		float mid = (inner + outer) * 0.5f;

		auto& wt = *ringSegWT_[i];
		wt.translation_ = {ringC_.x + mid * std::cos(a), 0.0f, ringC_.z + mid * std::sin(a)};
		wt.rotation_.y = a + PI / 2.0f;
		wt.scale_ = {ringThickness_, 0.1f, (2.0f * PI * ringR_) / N};
		WorldTransformUpdate(wt);
	}

	// パドル（1本目）
	const int P = kPaddleSegments;
	for (int i = 0; i < P; ++i) {
		float a = paddle_.angle - paddle_.halfWidth + (2.0f * paddle_.halfWidth) * (i / static_cast<float>(P - 1));
		float inner = ringR_ - ringThickness_ * 0.5f;
		float outer = ringR_ + ringThickness_ * 0.5f;
		float mid = (inner + outer) * 0.5f;

		auto& wt = *paddleSegWT_[i];
		wt.translation_ = {ringC_.x + mid * std::cos(a), 0.0f, ringC_.z + mid * std::sin(a)};
		wt.rotation_.y = a + PI / 2.0f;
		wt.scale_ = {ringThickness_ * 1.2f, 0.2f, (2.0f * paddle_.halfWidth * ringR_) / P * 1.2f};
		WorldTransformUpdate(wt);
	}

	// パドル（2本目：反対側）
	if (doublePaddle_) {
		float angle2 = paddle_.angle + PI; // 180度反対
		const int Q = kPaddleSegments;
		for (int i = 0; i < Q; ++i) {
			float a = angle2 - paddle_.halfWidth + (2.0f * paddle_.halfWidth) * (i / static_cast<float>(Q - 1));
			float inner = ringR_ - ringThickness_ * 0.5f;
			float outer = ringR_ + ringThickness_ * 0.5f;
			float mid = (inner + outer) * 0.5f;

			auto& wt = *paddleSegWT2_[i];
			wt.translation_ = {ringC_.x + mid * std::cos(a), 0.0f, ringC_.z + mid * std::sin(a)};
			wt.rotation_.y = a + PI / 2.0f;
			wt.scale_ = {ringThickness_ * 1.2f, 0.2f, (2.0f * paddle_.halfWidth * ringR_) / Q * 1.2f};
			WorldTransformUpdate(wt);
		}
	}

	// コア（見た目は小さめ／当たりは coreR_ で管理）
	auto& cwt = *coreWT_;
	cwt.translation_ = ringC_;
	cwt.rotation_ = {0, 0, 0};
	cwt.scale_ = {coreR_ * 0.1f, 0.1f, coreR_ * 0.1f};
	WorldTransformUpdate(cwt);
}

// ==================== 弾 ====================
void GameScene::SpawnShot() {
	shots_.emplace_back();
	Shot& s = shots_.back();
	s.active = true;

	float a = paddle_.angle;
	float inner = ringR_ - ringThickness_ * 0.5f;
	float outer = ringR_ + ringThickness_ * 0.5f;
	float mid = (inner + outer) * 0.5f;

	s.pos = {ringC_.x + mid * std::cos(a), 0.0f, ringC_.z + mid * std::sin(a)};
	Vector3 dir = {ringC_.x - s.pos.x, 0.0f, ringC_.z - s.pos.z};
	float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
	if (len > 1e-5f) {
		dir.x /= len;
		dir.z /= len;
	}

	float speed = 10.0f;
	s.vel = {dir.x * speed * (1.0f / 60.0f), 0.0f, dir.z * speed * (1.0f / 60.0f)};

	s.wt = std::make_unique<WorldTransform>();
	s.wt->Initialize();
	s.wt->translation_ = s.pos;
	s.wt->scale_ = {kShotVisualScale, kShotVisualScale, kShotVisualScale};
	WorldTransformUpdate(*s.wt);

	// 当たり半径は見た目から算出して保持
	s.radius = kShotVisualScale * kShotCollisionFromVisual;
}

void GameScene::UpdateShots(float /*dt*/) {
	for (auto& s : shots_) {
		if (!s.active)
			continue;
		s.pos += s.vel;

		float dx = s.pos.x - ringC_.x;
		float dz = s.pos.z - ringC_.z;

		// コアとの当たり（弾半径を加味）
		float coreHitR = coreR_ + s.radius;
		if ((dx * dx + dz * dz) <= coreHitR * coreHitR) {
			s.active = false;
			continue;
		}

		if (s.wt) {
			s.wt->translation_ = s.pos;
			WorldTransformUpdate(*s.wt);
		}
	}
	if (!shots_.empty()) {
		shots_.erase(std::remove_if(shots_.begin(), shots_.end(), [](const Shot& s) { return !s.active; }), shots_.end());
	}
}

// ==================== 敵 ====================
void GameScene::SpawnEnemy() {
	enemies_.emplace_back();
	Enemy& e = enemies_.back();
	e.active = true;

	float angle = RandomRange(0.0f, 2.0f * PI);
	float radius = ringR_ * 2.5f;
	e.pos = {ringC_.x + radius * std::cos(angle), 0.0f, ringC_.z + radius * std::sin(angle)};

	Vector3 dir = {ringC_.x - e.pos.x, 0.0f, ringC_.z - e.pos.z};
	float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
	if (len > 1e-5f) {
		dir.x /= len;
		dir.z /= len;
	}

	float speed = 2.0f;
	e.vel = {dir.x * speed * (1.0f / 60.0f), 0.0f, dir.z * speed * (1.0f / 60.0f)};

	e.wt = std::make_unique<WorldTransform>();
	e.wt->Initialize();
	e.wt->translation_ = e.pos;
	e.wt->scale_ = {0.5f, 0.5f, 0.5f};
	WorldTransformUpdate(*e.wt);
}

void GameScene::UpdateEnemies(float dt) {
	for (auto& e : enemies_) {
		if (!e.active)
			continue;

		// 吸引（進化）
		if (attractActive_) {
			auto applyAttract = [&](float baseAngle) {
				float enemyA = std::atan2(e.pos.z - ringC_.z, e.pos.x - ringC_.x);
				float relA = WrapAngle(enemyA - baseAngle);
				bool inAngle = (std::abs(relA) <= (paddle_.halfWidth + attractAngleBonus_));
				float dist = std::sqrt((e.pos.x - ringC_.x) * (e.pos.x - ringC_.x) + (e.pos.z - ringC_.z) * (e.pos.z - ringC_.z));
				bool nearRing = (std::abs(dist - ringR_) <= attractBand_);
				if (inAngle && nearRing) {
					float radialDir = (dist > ringR_) ? -1.0f : +1.0f;
					Vector3 toC = {ringC_.x - e.pos.x, 0.0f, ringC_.z - e.pos.z};
					float L = std::sqrt(toC.x * toC.x + toC.z * toC.z);
					if (L > 1e-5f) {
						toC.x /= L;
						toC.z /= L;
					}
					e.vel.x += toC.x * attractPower_ * dt * radialDir * (1.0f / 60.0f);
					e.vel.z += toC.z * attractPower_ * dt * radialDir * (1.0f / 60.0f);
				}
			};
			applyAttract(paddle_.angle);
			if (doublePaddle_)
				applyAttract(paddle_.angle + PI);
		}

		// 移動
		e.pos += e.vel;

		float dx = e.pos.x - ringC_.x;
		float dz = e.pos.z - ringC_.z;
		float dist2 = dx * dx + dz * dz;
		float dist = std::sqrt(dist2);

		// ---- リング帯の“毎秒ベース”減速（弱め・範囲を狭める） ----
		float inner = ringR_ - ringThickness_ * 0.5f;
		float outer = ringR_ + ringThickness_ * 0.5f;
		float mid = (inner + outer) * 0.5f;
		float halfW = (outer - inner) * ringSlowBandScale_;
		if (dist >= mid - halfW && dist <= mid + halfW) {
			float k = (1.0f - ringSlowStrengthPerSec_ * dt);
			if (k < 0.0f)
				k = 0.0f;
			e.vel.x *= k;
			e.vel.z *= k;
		}

		// ---- 拠点減速帯（毎秒ベース）＋最小侵入加速で詰まり解消 ----
		if (slowActive_) {
			float slowR = (coreR_ + slowBand_);
			if (dist <= slowR && dist > coreR_) {
				float k = (1.0f - slowStrengthPerSec_ * dt);
				if (k < 0.0f)
					k = 0.0f;
				e.vel.x *= k;
				e.vel.z *= k;

				// 最低限コアへ近づく力を保証
				Vector3 toC = {-dx, 0.0f, -dz};
				float L = std::sqrt(toC.x * toC.x + toC.z * toC.z);
				if (L > 1e-5f) {
					toC.x /= L;
					toC.z /= L;
					float inwardSpeed = -(e.vel.x * toC.x + e.vel.z * toC.z); // コア向き成分を正に
					if (inwardSpeed < minInwardSpeed_) {
						e.vel.x += toC.x * minInwardAccel_ * dt;
						e.vel.z += toC.z * minInwardAccel_ * dt;
					}
				}
			}
		}

		// ===== コア到達 → ライフ or シールド処理（必ず消滅） =====
		float coreHit = coreR_ + kEnemyRadius; // 見た目と一致させる
		if (dist2 <= coreHit * coreHit) {
			e.active = false;
			if (shield_ > 0) {
				shield_--;
			} else {
				life_--;
			}
			continue;
		}

		// ===== 弾との衝突判定 =====
		for (auto& s : shots_) {
			if (!s.active)
				continue;
			float sx = s.pos.x - e.pos.x;
			float sz = s.pos.z - e.pos.z;
			float r = s.radius + kEnemyRadius;
			if (sx * sx + sz * sz <= r * r) {
				s.active = false;
				e.active = false;
				score_ += 100; // 弾撃破
				break;
			}
		}

		// ===== パドルとの衝突判定 =====
		if (e.active) {
			auto hitByPaddle = [&](float baseAngle) -> bool {
				float enemyAngle = std::atan2(e.pos.z - ringC_.z, e.pos.x - ringC_.x);
				auto NormalizeAngle = [](float a) {
					while (a > PI)
						a -= 2 * PI;
					while (a < -PI)
						a += 2 * PI;
					return a;
				};
				float startAngle = NormalizeAngle(baseAngle - paddle_.halfWidth);
				float endAngle = NormalizeAngle(baseAngle + paddle_.halfWidth);
				enemyAngle = NormalizeAngle(enemyAngle);

				bool inAngle = false;
				if (startAngle <= endAngle)
					inAngle = (enemyAngle >= startAngle && enemyAngle <= endAngle);
				else
					inAngle = (enemyAngle >= startAngle || enemyAngle <= endAngle);

				float inner = ringR_ - ringThickness_ * 0.5f;
				float outer = ringR_ + ringThickness_ * 0.5f;
				return (inAngle && dist >= inner && dist <= outer);
			};

			bool hit = hitByPaddle(paddle_.angle);
			if (!hit && doublePaddle_)
				hit = hitByPaddle(paddle_.angle + PI);

			if (hit) {
				e.active = false;
				// コンボと倍率
				paddleCombo_++;
				comboTimer_ = comboTimeout_;
				scoreMul_ = 1.0f + 0.2f * float(paddleCombo_);
				score_ += int(std::round(50.0f * scoreMul_));
			}
		}

		// Transform 更新
		if (e.active && e.wt) {
			e.wt->translation_ = e.pos;
			WorldTransformUpdate(*e.wt);
		}
	}

	// inactive の敵／弾を削除
	if (!enemies_.empty()) {
		enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](const Enemy& e) { return !e.active; }), enemies_.end());
	}
	if (!shots_.empty()) {
		shots_.erase(std::remove_if(shots_.begin(), shots_.end(), [](const Shot& s) { return !s.active; }), shots_.end());
	}
}

// ==================== 描画 ====================
void GameScene::DrawRingAndPaddle() {
	if (!modelBlockRing_ && !modelBlockPaddle_)
		return;

	for (auto& up : ringSegWT_)
		modelBlockRing_->Draw(*up, camera_);
	for (auto& up : paddleSegWT_)
		modelBlockPaddle_->Draw(*up, camera_);
	if (doublePaddle_) {
		for (auto& up : paddleSegWT2_)
			modelBlockPaddle_->Draw(*up, camera_);
	}

	// コア見た目
	if (modelBase_ && coreWT_)
		modelBase_->Draw(*coreWT_, camera_);
}

void GameScene::DrawShots() {
	if (!modelShot_)
		return;
	for (auto& s : shots_) {
		if (!s.active || !s.wt)
			continue;
		modelShot_->Draw(*s.wt, camera_);
	}
}

void GameScene::DrawEnemies() {
	if (!modelEnemy_)
		return;
	for (auto& e : enemies_) {
		if (!e.active || !e.wt)
			continue;
		modelEnemy_->Draw(*e.wt, camera_);
	}
}

// ==================== 強化・進化の適用 ====================
void GameScene::ApplyProgression() {
	// パドルLv（長さ+2° & CD-10%/Lv）
	int newLv = (score_ >= 2000) ? 3 : (score_ >= 1000) ? 2 : (score_ >= 500) ? 1 : 0;
	if (newLv != paddleLevel_) {
		paddleLevel_ = newLv;
		RecomputePaddleHalfWidth();
	}

	// 二重パドル
	doublePaddle_ = (score_ >= 1500);
	// 吸引進化
	attractActive_ = (score_ >= 2500);

	// コア拡大 / 減速帯（当たりは coreR_ + kEnemyRadius で最終消滅を保証）
	coreR_ = (score_ >= 1200) ? 2.6f : 2.0f;
	slowActive_ = (score_ >= 1400);

	// リング半径（成長）
	float newRingR = ringRBase_;
	if (score_ >= 800)
		newRingR += 0.5f;
	if (score_ >= 1200)
		newRingR += 0.5f; // 合計 +1.0
	if (score_ >= 2000)
		newRingR += 0.5f; // 合計 +1.5
	if (std::abs(newRingR - ringR_) > 1e-4f) {
		ringR_ = newRingR;
	}

	// ライフ回復・シールド付与（1回ずつ）
	static bool lifeUpDone = false;
	static bool shieldDone = false;
	if (!lifeUpDone && score_ >= 800) {
		life_ = (std::min)(life_ + 1, 9);
		lifeUpDone = true;
	}
	if (!shieldDone && score_ >= 1600) {
		shield_ = (std::min)(shield_ + 1, 3);
		shieldDone = true;
	}

	// 固定砲台（進化）
	turretActive_ = (score_ >= 3000);
}

void GameScene::RecomputePaddleHalfWidth() {
	// 長さ+2°/Lv → halfWidth = base + Lv * 2°
	float halfDeg = paddle_.baseHalfDeg + 2.0f * float(std::clamp(paddleLevel_, 0, 3));
	paddle_.halfWidth = ToRadians(halfDeg);
}

// ==================== コア固定砲台（進化） ====================
void GameScene::UpdateTurret(float dt) {
	if (!turretActive_)
		return;

	turretTimer_ += dt;
	if (turretTimer_ < turretInterval_)
		return;
	turretTimer_ = 0.0f;

	// 最も近い敵を探す
	float bestD2 = (std::numeric_limits<float>::max)();
	Vector3 target{};
	bool found = false;
	for (auto& e : enemies_) {
		if (!e.active)
			continue;
		float dx = e.pos.x - ringC_.x;
		float dz = e.pos.z - ringC_.z;
		float d2 = dx * dx + dz * dz;
		if (d2 < bestD2) {
			bestD2 = d2;
			target = e.pos;
			found = true;
		}
	}
	if (!found)
		return;

	// 中心から目標へ向けて弾を撃つ（既存ショットの仕組みを流用）
	shots_.emplace_back();
	Shot& s = shots_.back();
	s.active = true;
	s.pos = ringC_; // コア中心から

	Vector3 dir = {target.x - s.pos.x, 0.0f, target.z - s.pos.z};
	float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
	if (len > 1e-5f) {
		dir.x /= len;
		dir.z /= len;
	}
	s.vel = {dir.x * turretShotSpeed_ * (1.0f / 60.0f), 0.0f, dir.z * turretShotSpeed_ * (1.0f / 60.0f)};

	s.wt = std::make_unique<WorldTransform>();
	s.wt->Initialize();
	s.wt->translation_ = s.pos;
	s.wt->scale_ = {kShotVisualScale, kShotVisualScale, kShotVisualScale};
	WorldTransformUpdate(*s.wt);

	// 当たり半径
	s.radius = kShotVisualScale * kShotCollisionFromVisual;
}
