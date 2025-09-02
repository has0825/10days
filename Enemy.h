#pragma once

#include "KamataEngine.h"
#include "Math.h"
#include "Player.h"

using namespace KamataEngine;

// 02_10 20枚目
// class Player;
class GameScene;

// 02_09 スライド4枚目
class Enemy {

public:
	// 02_15 13枚目 振るまい
	enum class Behavior {
		kUnknown = -1, // 無効な状態
		kWalk,         // 歩行状態
		kDefeated,     // やられ状態
	};

	// 02_09 スライド5枚目
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 02_09 スライド5枚目
	void Update();
	// 02_09 スライド5枚目
	void Draw();
	// 02_10 スライド14枚目
	AABB GetAABB();
	// 02_10 スライド14枚目 ワールド座標を取得
	Vector3 GetWorldPosition();
	// 02_10 スライド20枚目 衝突応答
	void OnCollision(const Player* player);
	// 02_15 6枚目
	bool IsDead() const { return isDead_; }
	// 02_15 20枚目
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }
	// 02_16 19
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

private:
	// 02_09 6枚目 ザ・ワールド
	WorldTransform worldTransform_;
	// 02_09 6枚目 モデル
	Model* model_ = nullptr;
	// 02_09 6枚目 カメラ
	Camera* camera_ = nullptr;

	// 02_09 15枚目
	static inline const float kWalkSpeed = 0.02f;
	// 02_09 15枚目
	Vector3 velocity_ = {};

	// 02_09 19枚目
	static inline const float kWalkMotionAngleStart = 0.0f;
	// 02_09 19枚目
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// 02_09 19枚目
	static inline const float kWalkMotionTime = 1.0f;
	// 02_09 20枚目
	float walkTimer = 0.0f;

	// 02_10 14枚目 当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 02_15 6枚目
	bool isDead_ = false;

	// 02_15 13枚目
	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 02_15 15枚目
	static inline const float kDefeatedTime = 0.6f;
	static inline const float kDefeatedMotionAngleStart = 0.0f;
	static inline const float kDefeatedMotionAngleEnd = -60.0f;
	float counter_ = 0.0f; // カウンター

	// 02_15 20枚目
	bool isCollisionDisabled_ = false;
	// 02_16 19
	GameScene* gameScene_ = nullptr;
};
