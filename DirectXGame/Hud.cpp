#include "Hud.h"
#include <string>

using KamataEngine::Sprite;
using KamataEngine::TextureManager;
using KamataEngine::Vector2;

void Hud::Initialize(const std::string& textureFile) {
	texHandle_ = TextureManager::Load(textureFile);

	// 表示スケール
	const float s = kLabelScale;

	// --- Timer (左上) ---
	posTimer_ = {0.0f, 0.0f};
	sprTimer_ = Sprite::Create(texHandle_, posTimer_);
	sprTimer_->SetTextureRect({0.0f, 0.0f}, {136.0f, static_cast<float>(kLabelH)});
	sprTimer_->SetSize({136.0f * s, static_cast<float>(kLabelH) * s});
	sprTimer_->SetAnchorPoint({0.0f, 0.0f}); // 左上基準
	sizeTimer_ = {136.0f * s, static_cast<float>(kLabelH) * s};

	// --- Life (左下) ---
	posLife_ = {0.0f, 650.0f};
	sprLife_ = Sprite::Create(texHandle_, posLife_);
	sprLife_->SetTextureRect({144.0f, 0.0f}, {84.0f, static_cast<float>(kLabelH)});
	sprLife_->SetSize({84.0f * s, static_cast<float>(kLabelH) * s});
	sprLife_->SetAnchorPoint({0.0f, 0.0f});
	sizeLife_ = {84.0f * s, static_cast<float>(kLabelH) * s};

	// --- Score (右上) ---
	posScore_ = {900.0f, 0.0f};
	sprScore_ = Sprite::Create(texHandle_, posScore_);
	sprScore_->SetTextureRect({288.0f, 0.0f}, {136.0f, static_cast<float>(kLabelH)});
	sprScore_->SetSize({136.0f * s, static_cast<float>(kLabelH) * s});
	sprScore_->SetAnchorPoint({0.0f, 0.0f});
	sizeScore_ = {136.0f * s, static_cast<float>(kLabelH) * s};

	// --- Skill (右下) ---
	posSkill_ = {900.0f, 650.0f};
	sprSkill_ = Sprite::Create(texHandle_, posSkill_);
	sprSkill_->SetTextureRect({432.0f, 0.0f}, {100.0f, static_cast<float>(kLabelH)});
	sprSkill_->SetSize({100.0f * s, static_cast<float>(kLabelH) * s});
	sprSkill_->SetAnchorPoint({0.0f, 0.0f});
	sizeSkill_ = {100.0f * s, static_cast<float>(kLabelH) * s};

	// 事前確保（数字）
	EnsureDigits(digitsTimer_, 5);
	EnsureDigits(digitsScore_, 7);

	// ===== Lifeアイコンの準備 =====
	texLifeIcon_ = TextureManager::Load("Life.png");

	// アイコンサイズは「Lifeラベル高さの 90%」で正方形
	float iconH = sizeLife_.y * kLifeIconScale;
	float iconW = iconH;
	sizeLifeIcon_ = {iconW, iconH};

	// アイコン基準位置（Lifeラベルの右横・垂直中央）
	float y = posLife_.y + (sizeLife_.y - iconH) * 0.5f;
	posLifeIconsBase_ = {posLife_.x + sizeLife_.x + kLifeLeftMargin, y};

	// 3つ分のスプライト生成
	for (auto& sp : sprLifeIcons_) {
		sp = Sprite::Create(texLifeIcon_, posLifeIconsBase_);
		sp->SetAnchorPoint({0.0f, 0.0f}); // 左上
		sp->SetSize(sizeLifeIcon_);
	}
}

void Hud::EnsureDigits(std::vector<Sprite*>& pool, size_t count) {
	while (pool.size() < count) {
		Sprite* sp = Sprite::Create(texHandle_, {0.0f, 0.0f});
		sp->SetTextureRect({0.0f, static_cast<float>(kDigitsY)}, {static_cast<float>(kDigitW), static_cast<float>(kDigitH)});
		sp->SetSize({static_cast<float>(kDigitW) * kDigitScale, static_cast<float>(kDigitH) * kDigitScale});
		sp->SetAnchorPoint({0.0f, 0.0f}); // 左上基準
		pool.push_back(sp);
	}
}

int Hud::GlyphIndexFromChar(char c) const {
	if (c >= '0' && c <= '9')
		return static_cast<int>(c - '0'); // 0-9
	if (c == 'x' || c == 'X' || c == '*')
		return 11; // ×
	if (c == '.')
		return 10; // 中黒・
	return 0;      // デフォルト '0'
}

void Hud::DrawString(const std::string& text, const Vector2& anchorLeftTop, std::vector<Sprite*>& pool) {
	EnsureDigits(pool, text.size());

	const Vector2 glyphSize = {static_cast<float>(kDigitW) * kDigitScale, static_cast<float>(kDigitH) * kDigitScale};

	for (size_t i = 0; i < text.size(); ++i) {
		int gi = GlyphIndexFromChar(text[i]);
		float tx = static_cast<float>(gi * kDigitW);

		Sprite* sp = pool[i];
		Vector2 pos = {anchorLeftTop.x + static_cast<float>(i) * glyphSize.x, anchorLeftTop.y};
		sp->SetPosition(pos);
		sp->SetTextureRect({tx, static_cast<float>(kDigitsY)}, {static_cast<float>(kDigitW), static_cast<float>(kDigitH)});
		sp->SetSize(glyphSize);
		sp->Draw();
	}
}

void Hud::DrawTimer(int seconds) {
	if (sprTimer_)
		sprTimer_->Draw();

	std::string s = std::to_string(seconds);

	float numH = static_cast<float>(kDigitH) * kDigitScale;
	float y = posTimer_.y + (sizeTimer_.y - numH) * 0.5f;

	Vector2 anchor = {posTimer_.x + sizeTimer_.x + kNumLeftMargin, y};
	DrawString(s, anchor, digitsTimer_);
}

void Hud::DrawScore(int score) {
	if (sprScore_)
		sprScore_->Draw();

	std::string s = std::to_string(score);

	float numH = static_cast<float>(kDigitH) * kDigitScale;
	float y = posScore_.y + (sizeScore_.y - numH) * 0.5f;

	Vector2 anchor = {posScore_.x + sizeScore_.x + kNumLeftMargin, y};
	DrawString(s, anchor, digitsScore_);
}

void Hud::DrawLife(int life) {
	// ラベル本体
	if (sprLife_)
		sprLife_->Draw();

	// 表示するアイコン数（0〜3にクランプ）
	int n = life;
	if (n < 0)
		n = 0;
	if (n > 3)
		n = 3;

	// アイコンを左から n 個だけ描画
	for (int i = 0; i < n; ++i) {
		if (!sprLifeIcons_[i])
			continue;
		Vector2 pos = {posLifeIconsBase_.x + static_cast<float>(i) * (sizeLifeIcon_.x + kLifeIconSpacing), posLifeIconsBase_.y};
		sprLifeIcons_[i]->SetPosition(pos);
		sprLifeIcons_[i]->SetSize(sizeLifeIcon_);
		sprLifeIcons_[i]->Draw();
	}
}

void Hud::DrawSkill(int /*skill*/) {
	if (sprSkill_)
		sprSkill_->Draw();
}
