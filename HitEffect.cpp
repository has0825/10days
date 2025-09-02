#include "HitEffect.h"
#include "Math.h"
#include <cassert>
#include <numbers>
#include <random>

using namespace KamataEngine;

// 02_16 9枚目
Model* HitEffect::model_ = nullptr;
Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(const KamataEngine::Vector3& position) {

	HitEffect* instance = new HitEffect();
	assert(instance);

	instance->Initialize(position);

	return instance;
}

void HitEffect::Initialize(const KamataEngine::Vector3& position) {

	std::random_device seedGenerator;
	std::mt19937_64 randomEngine;
	randomEngine.seed(seedGenerator());
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	// 楕円エフェクト
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.rotation_ = {0.0f, 0.0f, rotationDistribution(randomEngine)};
		worldTransform.translation_ = position;
		worldTransform.translation_.z = -1.0f;
		worldTransform.Initialize();
	}

	// 円形エフェクト
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_.z = -1.0f;
	objectColor_.Initialize();
}

void HitEffect::Update() {

	if (IsDead()) {
		return; // 既に消滅している場合は更新しない
	}

	switch (state_) {
	case State::kSpread: {
		++counter_;
		float scale = 0.5f + static_cast<float>(counter_) / kSpreadTime * 0.5f;
		const float slashScale = 2.0f;
		for (auto& slashWorldTransform : ellipseWorldTransforms_) {
			slashWorldTransform.scale_ = {0.1f, scale * slashScale, 1.0f};
		}

		const float circleScale = 1.0;

		circleWorldTransform_.scale_ = {scale * circleScale, scale * circleScale, 1.0f};

		if (counter_ >= kSpreadTime) {
			state_ = State::kFade;
			counter_ = 0; // カウンターをリセット
		}
		break;
	}
	case State::kFade: {
		++counter_;
		objectColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, 1.0f - static_cast<float>(counter_) / kFadeTime});

		if (++counter_ >= kFadeTime) {
			state_ = State::kDead;
		}

		break;
	}
	default:
		break;
	}

	for (auto& slashWorldTransform : ellipseWorldTransforms_) {
		WorldTransformUpdate(slashWorldTransform);
	}

	WorldTransformUpdate(circleWorldTransform_);
}

void HitEffect::Draw() {
	assert(model_);
	assert(camera_);

	if (IsDead()) {
		return; // 既に消滅している場合は描画しない
	}

	for (auto& slashWorldTransform : ellipseWorldTransforms_) {
		model_->Draw(slashWorldTransform, *camera_, &objectColor_);
	}

	model_->Draw(circleWorldTransform_, *camera_, &objectColor_);
}