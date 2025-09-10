#pragma once
#include <KamataEngine.h>
#include <array>
#include <string>
#include <vector>

class Hud {
public:
	void Initialize(const std::string& textureFile);

	void DrawTimer(int seconds);
	void DrawScore(int score);
	void DrawLife(int life);
	void DrawSkill(int /*skill*/);

	// ★ 追加：任意位置に数値文字列描画 / ランキング表示
	void DrawNumberString(const std::string& text, const KamataEngine::Vector2& pos);
	void DrawRankingTop3(const std::array<int, 3>& hs, const KamataEngine::Vector2& topLeft);

	// 既存
	const KamataEngine::Vector2& GetSkillLabelPos() const { return posSkill_; }
	const KamataEngine::Vector2& GetSkillLabelSize() const { return sizeSkill_; }
	const KamataEngine::Vector2& GetScoreLabelPos() const { return posScore_; }
	const KamataEngine::Vector2& GetScoreLabelSize() const { return sizeScore_; }

private:
	uint32_t texHandle_ = 0u;
	KamataEngine::Sprite* sprTimer_ = nullptr;
	KamataEngine::Sprite* sprScore_ = nullptr;
	KamataEngine::Sprite* sprLife_ = nullptr;
	KamataEngine::Sprite* sprSkill_ = nullptr;

	KamataEngine::Vector2 posTimer_{0.0f, 0.0f};
	KamataEngine::Vector2 posScore_{0.0f, 0.0f};
	KamataEngine::Vector2 posLife_{0.0f, 0.0f};
	KamataEngine::Vector2 posSkill_{0.0f, 0.0f};

	KamataEngine::Vector2 sizeTimer_{0.0f, 0.0f};
	KamataEngine::Vector2 sizeScore_{0.0f, 0.0f};
	KamataEngine::Vector2 sizeLife_{0.0f, 0.0f};
	KamataEngine::Vector2 sizeSkill_{0.0f, 0.0f};

	std::vector<KamataEngine::Sprite*> digitsTimer_;
	std::vector<KamataEngine::Sprite*> digitsScore_;

	// ▼ ランキングや任意文字列描画用の一時プール
	std::vector<KamataEngine::Sprite*> digitsScratch_; // ★ 追加

	static inline const int kLabelH = 53;
	static inline const int kDigitsY = 53;
	static inline const int kDigitW = 48;
	static inline const int kDigitH = 59;
	static inline const int kDigitsCols = 12;

	static inline const float kLabelScale = 1.2f;
	static inline const float kDigitScale = 1.0f;

	static inline const float kNumLeftMargin = 8.0f;
	static inline const float kLifeLeftMargin = 10.0f;

	uint32_t texLifeIcon_ = 0u;
	std::array<KamataEngine::Sprite*, 3> sprLifeIcons_{nullptr};
	KamataEngine::Vector2 posLifeIconsBase_{0.0f, 0.0f};
	KamataEngine::Vector2 sizeLifeIcon_{0.0f, 0.0f};
	static inline const float kLifeIconScale = 0.9f;
	static inline const float kLifeIconSpacing = 6.0f;

	void EnsureDigits(std::vector<KamataEngine::Sprite*>& pool, size_t count);
	int GlyphIndexFromChar(char c) const;
	void DrawString(const std::string& text, const KamataEngine::Vector2& anchorLeftTop, std::vector<KamataEngine::Sprite*>& pool);
};
