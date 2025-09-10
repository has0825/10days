#pragma once
#include <KamataEngine.h>
#include <array>
#include <string>
#include <vector>

class Hud {
public:
	// UI用テクスチャの初期化
	void Initialize(const std::string& textureFile);

	// ラベル＋数値描画
	void DrawTimer(int seconds);
	void DrawScore(int score);

	// ラベル＋アイコン描画
	void DrawLife(int life);
	void DrawSkill(int /*skill*/);

private:
	// ===== テクスチャとレイアウト =====
	uint32_t texHandle_ = 0u;

	// ラベルスプライト
	KamataEngine::Sprite* sprTimer_ = nullptr; // 左上
	KamataEngine::Sprite* sprScore_ = nullptr; // 右上
	KamataEngine::Sprite* sprLife_ = nullptr;  // 左下
	KamataEngine::Sprite* sprSkill_ = nullptr; // 右下

	// ラベルの描画起点（左上）
	KamataEngine::Vector2 posTimer_{0.0f, 0.0f};
	KamataEngine::Vector2 posScore_{0.0f, 0.0f};
	KamataEngine::Vector2 posLife_{0.0f, 0.0f};
	KamataEngine::Vector2 posSkill_{0.0f, 0.0f};

	// ラベル実サイズ（スケール後ピクセル）
	KamataEngine::Vector2 sizeTimer_{0.0f, 0.0f};
	KamataEngine::Vector2 sizeScore_{0.0f, 0.0f};
	KamataEngine::Vector2 sizeLife_{0.0f, 0.0f};
	KamataEngine::Vector2 sizeSkill_{0.0f, 0.0f};

	// 数字スプライトのプール（必要数だけ確保・使い回し）
	std::vector<KamataEngine::Sprite*> digitsTimer_;
	std::vector<KamataEngine::Sprite*> digitsScore_;

	// ===== Font.png の割り付け =====
	static inline const int kLabelH = 53;
	static inline const int kDigitsY = 53;    // 下段の開始Y
	static inline const int kDigitW = 48;     // 1桁原画像幅
	static inline const int kDigitH = 59;     // 1桁原画像高さ
	static inline const int kDigitsCols = 12; // 0-9, 10:・, 11:×

	// 表示スケール
	static inline const float kLabelScale = 1.2f;
	static inline const float kDigitScale = 1.0f;

	// ラベルと数字/アイコンの隙間(px)
	static inline const float kNumLeftMargin = 8.0f;
	static inline const float kLifeLeftMargin = 10.0f;

	// ===== Life アイコン =====
	uint32_t texLifeIcon_ = 0u;                                  // Life.png
	std::array<KamataEngine::Sprite*, 3> sprLifeIcons_{nullptr}; // 最大3つ
	KamataEngine::Vector2 posLifeIconsBase_{0.0f, 0.0f};         // 1個目の基準位置
	KamataEngine::Vector2 sizeLifeIcon_{0.0f, 0.0f};             // 実サイズ（スケール後）
	static inline const float kLifeIconScale = 0.9f;             // ラベル高さに対する比率
	static inline const float kLifeIconSpacing = 6.0f;           // アイコン間隔

	// 内部ヘルパ
	void EnsureDigits(std::vector<KamataEngine::Sprite*>& pool, size_t count);
	int GlyphIndexFromChar(char c) const; // 0-9, '.'->10, 'x'/'*'->11
	void DrawString(const std::string& text, const KamataEngine::Vector2& anchorLeftTop, std::vector<KamataEngine::Sprite*>& pool);
};
