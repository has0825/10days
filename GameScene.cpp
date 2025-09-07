#include "GameScene.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <numbers>
#include <string>

using namespace KamataEngine;

// ランダム補助
static float RandomRange(float min, float max) { return min + (max - min) * (float(rand()) / float(RAND_MAX)); }

void GameScene::Initialize() {
	// カメラ
	camera_.Initialize();
	camera_.translation_ = {0.0f, 40.0f, 0.0f};
	camera_.rotation_.x = ToRadians(90.0f);
	camera_.UpdateMatrix();

	// モデル
	modelBlock_ = Model::CreateFromOBJ("block");
	modelShot_ = Model::CreateFromOBJ("attack_effect");
	modelEnemy_ = Model::CreateFromOBJ("enemy");
	modelSkydome_ = Model::CreateFromOBJ("universedome");

	// HUD 初期化（プロジェクトに HUD.png を入れておくこと）
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

	coreWT_ = std::make_unique<WorldTransform>();
	coreWT_->Initialize();

	// コンテナ確保
	shots_.reserve(128);
	enemies_.reserve(128);

	// 初期配置計算
	UpdateRingAndPaddle(0.0f);

	score_ = 0;
	skill_ = 0;
	timer_ = 0;
	life_ = 3;
	timerAcc_ = 0.0f;

	// 天球
	//  skydome生成
	skydome_ = new Skydome();
	// 初期化
	skydome_->Initialize(modelSkydome_, &camera_);
}

void GameScene::Update() {
	const float dt = 1.0f / 60.0f;

	skydome_->Update();

	// タイマー (秒)
	timerAcc_ += dt;
	if (timerAcc_ >= 1.0f) {
		timerAcc_ -= 1.0f;
		timer_++;
	}

	// パドル回転
	if (Input::GetInstance()->PushKey(DIK_D)) {
		paddle_.angle -= paddle_.angularSpeed * dt;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		paddle_.angle += paddle_.angularSpeed * dt;
	}
	paddle_.angle = WrapAngle(paddle_.angle);

	// 発射
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SpawnShot();
	}

	// 敵の定期出現（1秒ごと）
	static float enemyTimer = 0.0f;
	enemyTimer += dt;
	if (enemyTimer >= 1.0f) {
		enemyTimer = 0.0f;
		SpawnEnemy();
	}

	// 更新
	UpdateRingAndPaddle(dt);
	UpdateShots(dt);
	UpdateEnemies(dt);

	camera_.UpdateMatrix();
}

void GameScene::Draw() {
	// モデル描画
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());
	skydome_->Draw();
	DrawRingAndPaddle();
	DrawShots();
	DrawEnemies();
	Model::PostDraw();

	// HUD描画
	Sprite::PreDraw(dxCommon->GetCommandList());
	hud_.DrawTimer(timer_);
	hud_.DrawScore(score_);
	hud_.DrawLife(life_);
	hud_.DrawSkill(skill_);
	Sprite::PostDraw();
}
// ==================== 円環 ====================
void GameScene::UpdateRingAndPaddle(float /*dt*/) {
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

	auto& cwt = *coreWT_;
	cwt.translation_ = ringC_;
	cwt.rotation_ = {0, 0, 0};
	cwt.scale_ = {coreR_ * 2.0f, 0.1f, coreR_ * 2.0f};
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
	s.wt->scale_ = {0.2f, 0.2f, 0.2f};
	WorldTransformUpdate(*s.wt);
}

void GameScene::UpdateShots(float /*dt*/) {
	for (auto& s : shots_) {
		if (!s.active)
			continue;
		s.pos += s.vel;

		float dx = s.pos.x - ringC_.x;
		float dz = s.pos.z - ringC_.z;
		if ((dx * dx + dz * dz) <= coreR_ * coreR_) {
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

void GameScene::UpdateEnemies(float /*dt*/) {
	for (auto& e : enemies_) {
		if (!e.active)
			continue;
		e.pos += e.vel;

		// ===== コアに到達したら消滅 =====
		float dx = e.pos.x - ringC_.x;
		float dz = e.pos.z - ringC_.z;
		if ((dx * dx + dz * dz) <= coreR_ * coreR_) {
			e.active = false;
			// ライフを減らす
			life_--;
			if (life_ <= 0) {
				// GameOver 遷移は main.cpp 側で判定
			}
			continue;
		}

		// ===== 弾との衝突判定 =====
		for (auto& s : shots_) {
			if (!s.active)
				continue;

			float sx = s.pos.x - e.pos.x;
			float sz = s.pos.z - e.pos.z;
			float dist2 = sx * sx + sz * sz;

			// 半径の和で判定（弾0.2, 敵0.5くらい）
			float r = 0.2f + 0.5f;
			if (dist2 <= r * r) {
				s.active = false;
				e.active = false;

				// ***** スコア加算（弾で倒した） *****
				score_ += 100;

				break; // この敵は消えたので弾チェック終了
			}
		}

		// ===== パドルとの衝突判定（今のまま） =====
		if (e.active) {
			float enemyAngle = std::atan2(e.pos.z - ringC_.z, e.pos.x - ringC_.x);

			auto NormalizeAngle = [](float a) {
				while (a > PI)
					a -= 2 * PI;
				while (a < -PI)
					a += 2 * PI;
				return a;
			};
			float startAngle = NormalizeAngle(paddle_.angle - paddle_.halfWidth);
			float endAngle = NormalizeAngle(paddle_.angle + paddle_.halfWidth);
			enemyAngle = NormalizeAngle(enemyAngle);

			bool inAngle = false;
			if (startAngle <= endAngle) {
				inAngle = (enemyAngle >= startAngle && enemyAngle <= endAngle);
			} else {
				inAngle = (enemyAngle >= startAngle || enemyAngle <= endAngle);
			}

			float dist = std::sqrt(dx * dx + dz * dz);
			float inner = ringR_ - ringThickness_ * 0.5f;
			float outer = ringR_ + ringThickness_ * 0.5f;

			if (inAngle && dist >= inner && dist <= outer) {
				e.active = false; // 当たったので敵を消す

				// ***** スコア加算（パドルで倒した） *****
				score_ += 50;
			}
		}

		// ===== Transform更新 =====
		if (e.active && e.wt) {
			e.wt->translation_ = e.pos;
			WorldTransformUpdate(*e.wt);
		}
	}

	// inactive の敵を削除
	if (!enemies_.empty()) {
		enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](const Enemy& e) { return !e.active; }), enemies_.end());
	}

	// inactive の弾も削除
	if (!shots_.empty()) {
		shots_.erase(std::remove_if(shots_.begin(), shots_.end(), [](const Shot& s) { return !s.active; }), shots_.end());
	}
}

// ==================== 描画 ====================
void GameScene::DrawRingAndPaddle() {
	if (!modelBlock_)
		return;
	for (auto& up : ringSegWT_)
		modelBlock_->Draw(*up, camera_);
	for (auto& up : paddleSegWT_)
		modelBlock_->Draw(*up, camera_);
	modelBlock_->Draw(*coreWT_, camera_);
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
