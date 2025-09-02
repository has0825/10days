#include "skydome.h"

/// <summary>
/// 初期化
/// </summary>
void Skydome::Initialize(Model* model, Camera* camera) {

	assert(model);

	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
}

/// <summary>
/// 更新
/// </summary>
void Skydome::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

/// <summary>
/// 描画
/// </summary>
void Skydome::Draw() {

	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}