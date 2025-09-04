#include "GameScene.h"
#include <cmath>

using namespace KamataEngine;

void GameScene::Initialize() {
	// カメラ
	camera_.Initialize();
	camera_.translation_ = {0.0f, 12.0f, -22.0f};
	camera_.rotation_.x = ToRadians(20.0f);
	camera_.UpdateMatrix();

	// モデル
	modelBlock_ = Model::CreateFromOBJ("block");
	modelShot_ = Model::CreateFromOBJ("attack_effect");

	// 円環 WT（unique_ptr で生成）
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

	// 弾は事前確保
	shots_.reserve(128);

	// 一度初期配置を計算
	UpdateRingAndPaddle(0.0f);
}

void GameScene::Update() {
	const float dt = 1.0f / 60.0f;

	// パドル回転：A/D
	if (Input::GetInstance()->PushKey(DIK_A)) {
		paddle_.angle -= paddle_.angularSpeed * dt;
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		paddle_.angle += paddle_.angularSpeed * dt;
	}
	paddle_.angle = WrapAngle(paddle_.angle);

	// 発射：SPACE
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SpawnShot();
	}

	// 更新
	UpdateRingAndPaddle(dt);
	UpdateShots(dt);

	camera_.UpdateMatrix();
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	DrawRingAndPaddle();
	DrawShots();

	Model::PostDraw();
}

// ==================== 円環：更新 ====================
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

	// パドル
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

	// コア
	auto& cwt = *coreWT_;
	cwt.translation_ = ringC_;
	cwt.rotation_ = {0, 0, 0};
	cwt.scale_ = {coreR_ * 2.0f, 0.1f, coreR_ * 2.0f};
	WorldTransformUpdate(cwt);
}

// ==================== 弾：生成 ====================
void GameScene::SpawnShot() {
	shots_.emplace_back();
	Shot& s = shots_.back();
	s.active = true;

	// 発射位置（リング幅中央）
	float a = paddle_.angle;
	float inner = ringR_ - ringThickness_ * 0.5f;
	float outer = ringR_ + ringThickness_ * 0.5f;
	float mid = (inner + outer) * 0.5f;

	s.pos = {ringC_.x + mid * std::cos(a), 0.0f, ringC_.z + mid * std::sin(a)};

	// 方向は内側
	Vector3 dir = {ringC_.x - s.pos.x, 0.0f, ringC_.z - s.pos.z};
	float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
	if (len > 1e-5f) {
		dir.x /= len;
		dir.z /= len;
	}

	float speed = 10.0f; // m/s（簡易に 1/60 を乗算）
	s.vel = {dir.x * speed * (1.0f / 60.0f), 0.0f, dir.z * speed * (1.0f / 60.0f)};

	s.wt = std::make_unique<WorldTransform>();
	s.wt->Initialize();
	s.wt->translation_ = s.pos;
	s.wt->rotation_ = {0, 0, 0};
	s.wt->scale_ = {0.2f, 0.2f, 0.2f};
	WorldTransformUpdate(*s.wt);
}

// ==================== 弾：更新 ====================
void GameScene::UpdateShots(float /*dt*/) {
	for (auto& s : shots_) {
		if (!s.active)
			continue;

		s.pos += s.vel;

		// コア内に入ったら消滅
		float dx = s.pos.x - ringC_.x;
		float dz = s.pos.z - ringC_.z;
		if ((dx * dx + dz * dz) <= coreR_ * coreR_) {
			s.active = false;
			continue;
		}

		if (s.wt) {
			s.wt->translation_ = s.pos;
			s.wt->rotation_ = {0, 0, 0};
			s.wt->scale_ = {0.2f, 0.2f, 0.2f};
			WorldTransformUpdate(*s.wt);
		}
	}

	// 非アクティブ弾の間引き
	if (!shots_.empty()) {
		shots_.erase(std::remove_if(shots_.begin(), shots_.end(), [](const Shot& s) { return !s.active; }), shots_.end());
	}
}

// ==================== 描画 ====================
void GameScene::DrawRingAndPaddle() {
	if (!modelBlock_)
		return;

	for (auto& up : ringSegWT_) {
		modelBlock_->Draw(*up, camera_);
	}
	for (auto& up : paddleSegWT_) {
		modelBlock_->Draw(*up, camera_);
	}
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
