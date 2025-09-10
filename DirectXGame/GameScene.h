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

	int GetScore() const { return score_; }
	bool IsGameOver() const { return life_ <= 0; }

private:
	// ============ リソース ============
	Camera* cameraPtr_ = nullptr;       // Skydome が参照するのでポインタでも持つ
	Camera camera_;                     // 実体
	Model* modelBase_ = nullptr;        // コア見た目用
	Model* modelBlockRing_ = nullptr;   // リング用
	Model* modelBlockPaddle_ = nullptr; // パドル用
	Model* modelShot_ = nullptr;        // 弾
	Model* modelEnemy_ = nullptr;       // 敵
	Model* modelSkydome_ = nullptr;     // 天球

	Hud hud_;
	int score_ = 0;
	int skill_ = 0; // 予備
	int timer_ = 0; // 経過秒
	float timerAcc_ = 0.0f;

	// ============ 円環・パドル ============
	static inline const int kRingSegments = 72;
	static inline const int kPaddleSegments = 24;
	Vector3 ringC_{0.0f, 0.0f, 0.0f};
	float ringR_ = 8.0f; // 現在のリング半径（成長により変化）
	float ringThickness_ = 0.8f;
	float coreR_ = 2.0f; // 当たり用半径（表示スケールとは別）
	int life_ = 3;
	int shield_ = 0;

	std::vector<std::unique_ptr<WorldTransform>> ringSegWT_;
	std::vector<std::unique_ptr<WorldTransform>> paddleSegWT_;  // 1本目
	std::vector<std::unique_ptr<WorldTransform>> paddleSegWT2_; // 2本目（有効時のみ描画）
	std::unique_ptr<WorldTransform> coreWT_;

	struct Paddle {
		float angle = 0.0f;
		float baseHalfDeg = 20.0f;          // 基本角度（度）
		float halfWidth = ToRadians(20.0f); // 実効（rad）
		float angularSpeed = ToRadians(180.0f);
	} paddle_;

	// ====== パドル強化・進化 ======
	int paddleLevel_ = 0;        // Lv0..3（長さ+2°/Lv & CD-10%/Lv）
	bool doublePaddle_ = false;  // 上下レーン
	bool attractActive_ = false; // 吸引進化
	float attractBand_ = 1.5f;   // リング近傍幅
	float attractPower_ = 2.0f;  // 吸引強さ
	float attractAngleBonus_ = ToRadians(6.0f);

	// 連続反射コンボ
	int paddleCombo_ = 0;
	float comboTimer_ = 0.0f;
	float comboTimeout_ = 3.0f;
	float scoreMul_ = 1.0f;

	// 発射クールダウン（Lv毎に -10%）
	float baseShotCooldown_ = 0.25f; // 秒
	float shotCooldownNow_ = 0.0f;

	// ============ 弾 ============
	static inline const float kShotVisualScale = 0.5f;         // 見た目スケール
	static inline const float kShotCollisionFromVisual = 0.5f; // 当たり半径 = VisualScale * 係数
	static inline const float kEnemyRadius = 1.5f;             // 敵の当たり半径
	static inline const float kPlayerShotSpeed = 10.0f;        // プレイヤー弾速（共有）

	struct Shot {
		bool active = false;
		Vector3 pos{};
		Vector3 vel{};
		float radius = 0.0f; // 当たり半径（見た目から算出）
		std::unique_ptr<WorldTransform> wt;

		// ★ホーミング用（固定砲台の弾だけ true）
		bool homing = false;                      // ホーミングするか
		float speed = 0.0f;                       // m/s（一定）
		float homingTurnRate = ToRadians(540.0f); // 旋回角速度（rad/s）

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
	bool slowActive_ = false; // 減速帯オン/オフ
	float slowBand_ = 1.6f;   // コア半径からの幅

	// “毎秒ベース”の減速＆滞留防止
	float slowStrengthPerSec_ = 0.50f; // 1秒あたり50%減速
	float minInwardAccel_ = 1.2f;      // コアへ向かう最小加速[m/s^2]
	float minInwardSpeed_ = 0.6f;      // コア帯での最低接近速度[m/s]

	// 進化：固定砲台（スコアで解禁）
	bool turretActive_ = false;
	float turretInterval_ = 0.8f;
	float turretTimer_ = 0.0f;
	float turretShotSpeed_ = 11.0f; // m/s

	// ============ スキル固定砲台（timer==60で出現） ============
	struct SkillCannon {
		bool active = false;
		float interval = 0.6f; // 発射間隔
		float timer = 0.0f;
		// 見た目（HUDアイコン）
		KamataEngine::Sprite* sprite = nullptr;
	} skillCannon_;
	uint32_t texSkillCannon_ = 0u;

	// ============ 天球 ============
	Skydome* skydome_ = nullptr;

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

	// 強化・進化の段階適用（スコア等）
	void ApplyProgression();
	void RecomputePaddleHalfWidth();

	// 自動砲台（コア進化）
	void UpdateTurret(float dt);

	// スキル砲台
	void SpawnSkillCannon(); // timer==60 で生成
	void UpdateSkillCannon(float dt);
	void DrawSkillCannon();

	// 近傍探索
	bool FindNearestEnemy(const Vector3& from, Vector3& outPos) const;

	// ====== リング帯の“毎秒ベース”減速 ======
	float ringSlowStrengthPerSec_ = 0.70f; // 1秒あたりの減速率
	float ringSlowBandScale_ = 0.35f;      // リング厚の±35%を緩く減速

	// リング基本半径（成長の基点）
	float ringRBase_ = 8.0f;

	// ============ 敵出現スケーリング ============
	float enemySpawnBaseRate_ = 1.0f;          // 初期の毎秒スポーン数
	float enemySpawnRateGrowthPerSec_ = 0.05f; // 時間(秒)ごとの増分
	float enemySpawnRatePerScore_ = 0.0005f;   // スコアによる増分
	float enemySpawnRateMax_ = 10.0f;          // 上限
	float enemySpawnAcc_ = 0.0f;               // 蓄積


	// ▼ BGM用
	uint32_t bgmHandle_ = 0u;  // 読み込んだBGMデータ
	uint32_t bgmVoice_ = 0u;  // 再生中のハンドル
	bool bgmStoppedOnGameOver_ = false;

};
