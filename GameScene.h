#pragma once
#include "Math.h"
#include <KamataEngine.h>
#include <algorithm>
#include <memory>
#include <vector>

using namespace KamataEngine;

class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	// ============ リソース ============
	Camera camera_;
	Model* modelBlock_ = nullptr; // リング/パドル/コア用（薄板）
	Model* modelShot_ = nullptr;  // 弾の見た目

	// ============ 円環・パドルのパラメータ ============
	static inline const int kRingSegments = 72;
	static inline const int kPaddleSegments = 24;
	Vector3 ringC_{0.0f, 0.0f, 0.0f};
	float ringR_ = 8.0f;
	float ringThickness_ = 0.8f;
	float coreR_ = 2.0f;

	// WT は unique_ptr で保持（エンジン型を値で持たない）
	std::vector<std::unique_ptr<WorldTransform>> ringSegWT_;
	std::vector<std::unique_ptr<WorldTransform>> paddleSegWT_;
	std::unique_ptr<WorldTransform> coreWT_;

	struct Paddle {
		float angle = 0.0f;
		float halfWidth = ToRadians(20.0f);
		float angularSpeed = ToRadians(180.0f);
	} paddle_;

	// ============ 弾 ============
	struct Shot {
		bool active = false;
		Vector3 pos{};
		Vector3 vel{};
		std::unique_ptr<WorldTransform> wt; // ムーブ可能に
		Shot() = default;
		Shot(const Shot&) = delete;
		Shot& operator=(const Shot&) = delete;
		Shot(Shot&&) noexcept = default;
		Shot& operator=(Shot&&) noexcept = default;
	};
	std::vector<Shot> shots_;

	// ============ 内部処理 ============
	void UpdateRingAndPaddle(float dt);
	void SpawnShot();
	void UpdateShots(float dt);
	void DrawRingAndPaddle();
	void DrawShots();
};
