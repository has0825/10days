#include "Enemy.h"
#include "GameScene.h"
#include "Math.h"
#include "Player.h"
#include <cassert>
#include <numbers>

// 02_09 スライド5枚目
void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {

	// NULLチェック
	assert(model);

	// 02_09 7枚目
	model_ = model;
	// 02_09 7枚目
	camera_ = camera;
	// 02_09 7枚目
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// 02_09 7枚目 角度調整
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	// 02_09 16枚目
	velocity_ = {-kWalkSpeed, 0, 0};
	// 02_09 20枚目
	walkTimer = 0.0f;
}

// 02_09 スライド5枚目
void Enemy::Update() {

	// 変更リクエストがあったら
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_;

		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kDefeated:
		default:
			counter_ = 0;
			break;
		}

		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	// 02_15 13枚目
	switch (behavior_) {
	// 歩行
	case Behavior::kWalk:
		// 02_09 16枚目 移動
		worldTransform_.translation_ += velocity_;

		// 02_09 20枚目
		walkTimer += 1.0f / 60.0f;

		// 02_09 23枚目 回転アニメーション
		worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

		// 02_09 スライド8枚目 ワールド行列更新
		WorldTransformUpdate(worldTransform_);
		break;
	// やられ
	case Behavior::kDefeated:
		// 02_15 15枚目
		counter_ += 1.0f / 60.0f;

		worldTransform_.rotation_.y += 0.3f;
		worldTransform_.rotation_.x = EaseOut(ToRadians(kDefeatedMotionAngleStart), ToRadians(kDefeatedMotionAngleEnd), counter_ / kDefeatedTime);

		WorldTransformUpdate(worldTransform_);

		if (counter_ >= kDefeatedTime) {
			isDead_ = true;
		}
		break;
	}
}

// 02_09 スライド5枚目
void Enemy::Draw() {

	// 02_09 スライド9枚目  モデル描画
	model_->Draw(worldTransform_, *camera_);
}

// 02_10 スライド14枚目
AABB Enemy::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

// 02_10 スライド14枚目
Vector3 Enemy::GetWorldPosition() {

	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

// 02_10 スライド20枚目
void Enemy::OnCollision(const Player* player) {

	// 02_15 6枚目 → 14枚目で削除
	//	isDead_ = true;

	if (behavior_ == Behavior::kDefeated) {
		// 敵がやられているなら何もしない
		return;
	}

	// プレイヤーが攻撃中なら敵が死ぬ
	// player.hをインクルード
	if (player->IsAttack()) {
		if (gameScene_) {

			Vector3 pos = player->GetWorldPosition();

			// 敵と自キャラの中間位置にエフェクトを生成
			Vector3 effectPos;

			effectPos.x = (GetWorldPosition() + pos).x / 2.0f;
			effectPos.y = (GetWorldPosition() + pos).y / 2.0f;
			effectPos.z = (GetWorldPosition() + pos).z / 2.0f;
			gameScene_->CreateEffect(effectPos);
		}
		// 敵の振るまいをやられに変更
		behaviorRequest_ = Behavior::kDefeated;

		// 02_15 20枚目 衝突を無効化
		isCollisionDisabled_ = true;
	}
}