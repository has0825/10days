#pragma once
#include "Hud.h"
#include "Math.h"
#include "Skydome.h"
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

	bool IsGameOver() const { return life_ <= 0; }

private:
	// ============ リソース ============
	Camera camera_;
	Model* modelBlock_ = nullptr;      // 既存のブロック（未使用）
	Model* modelBlockRing_ = nullptr; // リング
	Model* modelBlockPaddle_ = nullptr; // パドル / コア用
	Model* modelShot_ = nullptr;  // 弾（既存を流用）
	Model* modelEnemy_ = nullptr; // 敵

	Hud hud_;
	int score_ = 0;
	int skill_ = 0; // 使い道が決まっていないので将来用
	int timer_ = 0; // 経過秒
	float timerAcc_ = 0.0f;

	// ============ 円環・パドル ============
	static inline const int kRingSegments = 72;
	static inline const int kPaddleSegments = 24;
	Vector3 ringC_{0.0f, 0.0f, 0.0f};
	float ringR_ = 8.0f;
	float ringThickness_ = 0.8f;
	float coreR_ = 2.0f;
	int life_ = 3;   // 残機
	int shield_ = 0; // コアのシールド

	std::vector<std::unique_ptr<WorldTransform>> ringSegWT_;
	std::vector<std::unique_ptr<WorldTransform>> paddleSegWT_;  // 1本目
	std::vector<std::unique_ptr<WorldTransform>> paddleSegWT2_; // 二重パドル用（有効時のみ使用）
	std::unique_ptr<WorldTransform> coreWT_;

	struct Paddle {
		float angle = 0.0f;
		float baseHalfDeg = 20.0f;          // 基本角度（度）
		float halfWidth = ToRadians(20.0f); // 実効角度（rad）
		float angularSpeed = ToRadians(180.0f);
	} paddle_;

	// ====== パドル強化・進化（企画書） ======
	int paddleLevel_ = 0;                       // Lv0..3
	bool doublePaddle_ = false;                 // 二重パドル
	bool attractActive_ = false;                // 進化：周辺の隕石を吸い込む
	float attractBand_ = 1.5f;                  // リング近傍幅[m]
	float attractPower_ = 2.0f;                 // 吸引の強さ（加速[m/s^2]）
	float attractAngleBonus_ = ToRadians(6.0f); // パドル角に少し余裕

	// 連続反射コンボ
	int paddleCombo_ = 0;
	float comboTimer_ = 0.0f;   // 一定時間で切れる
	float comboTimeout_ = 3.0f; // 3秒でリセット
	float scoreMul_ = 1.0f;     // スコア倍率（1.0から加算）

	// 発射クールダウン（Lv毎に -10%）
	float baseShotCooldown_ = 0.25f; // 秒
	float shotCooldownNow_ = 0.0f;   // 残りクールタイム

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

	// ============ 拠点（コア）強化・進化 ============
	bool slowActive_ = false;       // 拠点内での隕石の減速
	float slowBand_ = 1.6f;         // コアからこの幅までは減速帯
	float slowFactor_ = 0.90f;      // 毎フレーム係数
	bool turretActive_ = false;     // 進化：固定砲台（自動射撃）
	float turretInterval_ = 0.8f;   // 発射間隔[s]
	float turretTimer_ = 0.0f;      // 経過カウンタ
	float turretShotSpeed_ = 11.0f; // m/s

	// ============ 内部処理 ============
	void UpdateRingAndPaddle(float dt);
	void SpawnShot();           // 既存：パドル発射（そのまま）
	void UpdateShots(float dt); // 既存：弾更新（そのまま）
	void DrawRingAndPaddle();   // 既存描画＋二重パドル時は2本目も描画
	void DrawShots();           // 既存

	// 敵
	void SpawnEnemy();
	void UpdateEnemies(float dt);
	void DrawEnemies();

	// 強化・進化の段階適用（スコア等で自動）
	void ApplyProgression();
	void RecomputePaddleHalfWidth();

	// 自動砲台（コア進化）
	void UpdateTurret(float dt);

	// 天球
	Model* modelSkydome_ = nullptr;
	Skydome* skydome_ = nullptr;

	// 弾＆敵の半径・見た目
	static inline const float kShotScale = 1.6f;
	static inline const float kShotRadius = 1.6f;
	static inline const float kEnemyRadius = 1.5f;
};
