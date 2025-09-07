#pragma once
#include <KamataEngine.h>
#include <array>
#include <string>

using namespace KamataEngine;

class Hud {
public:
	// UI用テクスチャの初期化
	void Initialize(const std::string& textureFile);

	// 数値は未描画、ラベルだけ
	void DrawTimer(int /*seconds*/);
	void DrawScore(int /*score*/);
	void DrawLife(int /*life*/);
	void DrawSkill(int /*skill*/);

private:
	uint32_t texHandle_ = 0u;

	// ラベルスプライト
	Sprite* sprTimer_ = nullptr; // 左上
	Sprite* sprScore_ = nullptr; // 右上
	Sprite* sprLife_ = nullptr;  // 左下
	Sprite* sprSkill_ = nullptr; // 右下

	// 位置情報
	Vector2 posTimer_ = {0.0f, 0.0f}; // 左上
	Vector2 posScore_ = {0.0f, 0.0f}; // 右上
	Vector2 posLife_ = {0.0f, 0.0f};  // 左下
	Vector2 posSkill_ = {0.0f, 0.0f}; // 右下

};
