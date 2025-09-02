#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

class MapChipField;

// 02_10 21枚目
class Enemy;

class Player {
public:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 角 02_07スライド16枚目
	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

	// 02_14 11枚目 振るまい
	enum class Behavior {
		kUnknown = -1,
		kRoot,   // 通常状態
		kAttack, // 攻撃中
	};

	// 02_14 24枚目 攻撃フェーズ
	enum class AttackPhase {
		kUnknown = -1, // 無効な状態

		kAnticipation, // 予備動作
		kAction,       // 前進動作
		kRecovery,     // 余韻動作
	};

	/// 初期化
	void Initialize(Model* model, Model* modelAttack, Camera* camera, const Vector3& position);

	/// 更新
	void Update();

	/// 描画
	void Draw();

	// getter(02_06スライド11枚目で追加)
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// 02_06スライド28枚目で追加
	const Vector3& GetVelocity() const { return velocity_; }

	// 02_07 スライド4枚目
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 02_10 10枚目 ワールド座標を取得
	Vector3 GetWorldPosition() const;

	// 02_10 13枚目
	AABB GetAABB();

	// 02_10 21枚目 衝突応答
	void OnCollision(const Enemy* enemy);

	// 02_12 11枚目 デスフラグ
	bool IsDead() const { return isDead_; }

	// 02_14 6枚目 通常行動更新
	void BehaviorRootUpdate();

	// 02_14 8枚目 攻撃行動更新
	void BehaviorAttackUpdate();

	// 02_14 16枚目 通常行動初期化
	void BehaviorRootInitialize();

	// 02_14 16枚目 攻撃行動初期化
	void BehaviorAttackInitialize();

	// 02_15 14枚目
	bool IsAttack() const { return behavior_ == Behavior::kAttack && attackPhase_ == AttackPhase::kAction; }

	// 02_15
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	Camera* camera_ = nullptr;
	// 02_05 移動量
	Vector3 velocity_ = {};
	// 02_05  フレームごとの加速度
	static inline const float kAcceleration = 0.1f;
	// 02_05 非入力時の摩擦係数
	static inline const float kAttenuation = 0.05f;
	// 02_05 最高速度
	static inline const float kLimitRunSpeed = 0.3f;
	// 02_05 顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// 02_05 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 02_05 旋回タイマー
	float turnTimer_ = 0.0f;
	// 02_05 旋回時間 <秒>
	static inline const float kTimeTurn = 0.3f;
	// 02_05 着地フラグ
	bool onGround_ = true;
	// 02_05 ジャンプ定数もろもろ
	static inline const float kJumpAcceleration = 20.0f;
	static inline const float kGravityAcceleration = 0.98f;
	static inline const float kLimitFallSpeed = 0.5f;

	// 02_07 マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	// 02_07 キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	// 02_07スライド34枚目
	static inline const float kBlank = 0.04f;

	// 02_07スライド10枚目 移動入力
	void InputMove();
	// 02_07 スライド12枚目
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};
	// 02_07 スライド13枚目
	void CheckMapCollision(CollisionMapInfo& info);
	// 02_07 スライド14枚目
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	// 02_07 スライド17枚目
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// 02_08スライド14枚目 設置状態の切り替え処理
	void UpdateOnGround(const CollisionMapInfo& info);

	// 02_08 スライド27枚目 壁接触している場合の処理
	void UpdateOnWall(const CollisionMapInfo& info);

	// 02_08スライド16枚目 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.0f;
	// 02_08スライド21枚目 微小な数値
	static inline const float kGroundSearchHeight = 0.06f;
	// 02_08スライド27枚目 着地時の速度減衰率
	static inline const float kAttenuationWall = 0.2f;
	// 02_12 11枚目 デスフラグ
	bool isDead_ = false;

	// 02_14 11枚目 振るまい
	Behavior behavior_ = Behavior::kRoot;

	// 02_14 14枚目 次の振るまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 02_14 19枚目 攻撃ギミックの経過時間カウンター
	uint32_t attackParameter_ = 0;

	// 02_14 24枚目 攻撃フェーズ
	AttackPhase attackPhase_ = AttackPhase::kUnknown;

	// 02_14 26枚目 予備動作の時間
	static inline const uint32_t kAnticipationTime = 8;
	// 02_14 26枚目 前進動作の時間
	static inline const uint32_t kActionTime = 5;
	// 02_14 26枚目 余韻動作の時間
	static inline const uint32_t kRecoveryTime = 12;
	// 02_14 34枚目 攻撃エフェクト
	Model* modelAttack_ = nullptr;
	WorldTransform worldTransformAttack_;

	// 02_15 20枚目
	bool isCollisionDisabled_ = false; // 衝突無効化
};
