#pragma once
#include "Hud.h"
#include "Math.h"
#include <KamataEngine.h>
#include <algorithm>
#include <memory>
#include <vector>
#include "Skydome.h"



using namespace KamataEngine;

class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();

	bool IsGameOver() const { return life_ <= 0; }

private:
	// ============ リソース ============
	Camera camera_;
	Model* modelBlock_ = nullptr; // リング/パドル/コア用
	Model* modelShot_ = nullptr;  // 弾
	Model* modelEnemy_ = nullptr; // 敵

  Hud hud_;
	int score_ = 0;
	int skill_ = 0;
	int timer_ = 0;
	float timerAcc_ = 0.0f; // 秒数加算用

	// ============ 円環・パドル ============
	static inline const int kRingSegments = 72;
	static inline const int kPaddleSegments = 24;
	Vector3 ringC_{0.0f, 0.0f, 0.0f};
	float ringR_ = 8.0f;
	float ringThickness_ = 0.8f;
	float coreR_ = 2.0f;
	int life_ = 3;

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
		std::unique_ptr<WorldTransform> wt;
		Shot() = default;
		Shot(const Shot&) = delete;
		Shot& operator=(const Shot&) = delete;
		Shot(Shot&&) noexcept = default;
		Shot& operator=(Shot&&) noexcept = default;
	};
	std::vector<Shot> shots_;

	// ============ 敵 ============
	struct Enemy {
		bool active = false;
		Vector3 pos{};
		Vector3 vel{};
		std::unique_ptr<WorldTransform> wt;
		Enemy() = default;
		Enemy(const Enemy&) = delete;
		Enemy& operator=(const Enemy&) = delete;
		Enemy(Enemy&&) noexcept = default;
		Enemy& operator=(Enemy&&) noexcept = default;
	};
	std::vector<Enemy> enemies_;

	// ============ 内部処理 ============
	void UpdateRingAndPaddle(float dt);
	void SpawnShot();
	void UpdateShots(float dt);
	void DrawRingAndPaddle();
	void DrawShots();

	// 敵
	void SpawnEnemy();
	void UpdateEnemies(float dt);
	void DrawEnemies();


	// 天球
	Model *modelSkydome_ = nullptr;
	Skydome *skydome_ = nullptr;
	WorldTransform worldTransformSkydome_;


};
