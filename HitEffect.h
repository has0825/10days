#pragma once
#include <KamataEngine.h>
#include <cstdint>

using namespace KamataEngine;

// 02_16 5枚目
class HitEffect {
public:
	enum class State {
		kSpread, // 拡大中
		kFade,   // フェードアウト中
		kDead    // 死亡
	};

	// 02_16 10枚目(SetModel,SetCamera)
	static void SetModel(KamataEngine::Model* model) { model_ = model; }

	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	static HitEffect* Create(const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	bool IsDead() const { return state_ == State::kDead; }

private:
	HitEffect() = default;

	void Initialize(const KamataEngine::Vector3& position);

	// 拡大アニメーションの時間
	static inline const uint32_t kSpreadTime = 10;

	// フェードアウトアニメーションの時間
	static inline const uint32_t kFadeTime = 20;

	// エフェクトの寿命
	static inline const uint32_t kLifetime = kSpreadTime + kFadeTime;

	// 02_16 8枚目
	static Model* model_;
	static Camera* camera_;

	// 楕円エフェクトの数
	static const inline uint32_t kellipseEffectNum = 2;

	// 楕円のワールドトランスフォーム
	std::array<WorldTransform, kellipseEffectNum> ellipseWorldTransforms_;

	// 円のワールドトランスフォーム
	WorldTransform circleWorldTransform_;

	State state_ = State::kSpread;

	// カウンター
	uint32_t counter_ = 0;

	ObjectColor objectColor_;
};
