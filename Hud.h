
#pragma once
#include <KamataEngine.h>
#include <array>
#include <string>
#include <vector>

    using namespace KamataEngine;

class Hud {
public:
	// UI用テクスチャの初期化
	void Initialize(const std::string& textureFile);

	// ラベル＋数値描画
	void DrawTimer(int seconds);
	void DrawScore(int score);

	// ラベルのみ描画（現状のまま）
	void DrawLife(int /*life*/);
	void DrawSkill(int /*skill*/);

private:
	// ===== テクスチャとレイアウト =====
	uint32_t texHandle_ = 0u;

	// ラベルスプライト
	Sprite* sprTimer = nullptr; // 左上
	Sprite* sprScore = nullptr; // 右上
	Sprite* sprLife = nullptr;  // 左下
	Sprite* sprSkill = nullptr; // 右下

	// ラベルの描画起点（左上）
	Vector2 posTimer_{0.0f, 0.0f};
	Vector2 posScore_{0.0f, 0.0f};
	Vector2 posLife_{0.0f, 0.0f};
	Vector2 posSkill_{0.0f, 0.0f};

	// ラベル実サイズ（スケール後ピクセル）
	Vector2 sizeTimer_{0.0f, 0.0f};
	Vector2 sizeScore_{0.0f, 0.0f};
	Vector2 sizeLife_{0.0f, 0.0f};
	Vector2 sizeSkill_{0.0f, 0.0f};

	// 数字スプライトのプール（必要数だけ確保・使い回し）
	std::vector<Sprite*> digitsTimer_;
	std::vector<Sprite*> digitsScore_;

	// ===== Font.png の割り付け =====
	// 上段ラベルの元サイズ
	static inline const int kLabelH = 53;
	// 下段数字のタイル（0〜9・「・」・「×」）
	static inline const int kDigitsY = 53;    // 下段の開始Y
	static inline const int kDigitW = 48;     // 1桁原画像幅
	static inline const int kDigitH = 59;     // 1桁原画像高さ
	static inline const int kDigitsCols = 12; // 0-9, 10:・, 11:×

	// 表示スケール
	static inline const float kLabelScale = 1.2f;
	static inline const float kDigitScale = 1.0f;

	// ラベルと数字の隙間(px)
	static inline const float kNumLeftMargin = 8.0f;

	// 内部ヘルパ
	void EnsureDigits(std::vector<Sprite*>& pool, size_t count);
	int GlyphIndexFromChar(char c) const; // 0-9, '.'->10, 'x'/'*'->11
	void DrawString(const std::string& text, const Vector2& anchorLeftTop, std::vector<Sprite*>& pool);
};
