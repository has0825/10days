#include "Hud.h"
#include <string>

void Hud::Initialize(const std::string& textureFile) {
	texHandle_ = TextureManager::Load(textureFile);

	// 表示スケール
	const float s = kLabelScale;

	// --- Timer (左上) ---
	posTimer_ = {0.0f, 0.0f};
	sprTimer_ = Sprite::Create(texHandle_, posTimer_);
	sprTimer_->SetTextureRect({0.0f, 0.0f}, {136.0f, static_cast<float>(kLabelH)});
	sprTimer_->SetSize({136.0f * s, static_cast<float>(kLabelH) * s});
	sprTimer_->SetAnchorPoint({0.0f, 0.0f}); // ★ 左上基準に固定
	sizeTimer_ = {136.0f * s, static_cast<float>(kLabelH) * s};

	// --- Life (左下) ---
	posLife_ = {0.0f, 650.0f};
	sprLife_ = Sprite::Create(texHandle_, posLife_);
	sprLife_->SetTextureRect({144.0f, 0.0f}, {84.0f, static_cast<float>(kLabelH)});
	sprLife_->SetSize({84.0f * s, static_cast<float>(kLabelH) * s});
	sprLife_->SetAnchorPoint({0.0f, 0.0f}); // ★
	sizeLife_ = {84.0f * s, static_cast<float>(kLabelH) * s};

	// --- Score (右上) ---
	posScore_ = {900.0f, 0.0f};
	sprScore_ = Sprite::Create(texHandle_, posScore_);
	sprScore_->SetTextureRect({288.0f, 0.0f}, {136.0f, static_cast<float>(kLabelH)});
	sprScore_->SetSize({136.0f * s, static_cast<float>(kLabelH) * s});
	sprScore_->SetAnchorPoint({0.0f, 0.0f}); // ★
	sizeScore_ = {136.0f * s, static_cast<float>(kLabelH) * s};

	// --- Skill (右下) ---
	posSkill_ = {900.0f, 650.0f};
	sprSkill_ = Sprite::Create(texHandle_, posSkill_);
	sprSkill_->SetTextureRect({432.0f, 0.0f}, {100.0f, static_cast<float>(kLabelH)});
	sprSkill_->SetSize({100.0f * s, static_cast<float>(kLabelH) * s});
	sprSkill_->SetAnchorPoint({0.0f, 0.0f}); // ★
	sizeSkill_ = {100.0f * s, static_cast<float>(kLabelH) * s};

	// 事前確保（最大桁数の想定：Timer 5桁、Score 7桁など）
	EnsureDigits(digitsTimer_, 5);
	EnsureDigits(digitsScore_, 7);
}

void Hud::EnsureDigits(std::vector<Sprite*>& pool, size_t count) {
	while (pool.size() < count) {
		Sprite* sp = Sprite::Create(texHandle_, {0.0f, 0.0f});
		sp->SetTextureRect({0.0f, static_cast<float>(kDigitsY)}, {static_cast<float>(kDigitW), static_cast<float>(kDigitH)});
		sp->SetSize({static_cast<float>(kDigitW) * kDigitScale, static_cast<float>(kDigitH) * kDigitScale});
		sp->SetAnchorPoint({0.0f, 0.0f}); // ★ 左上基準に固定
		// sp->SetColor({1.0f, 1.0f, 1.0f, 1.0f}); // 必要なら明示
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
	return 0;      // デフォルトは '0'
}

void Hud::DrawString(const std::string& text, const Vector2& anchorLeftTop, std::vector<Sprite*>& pool) {
	// 必要分だけ確保
	EnsureDigits(pool, text.size());

	// 1桁の表示サイズ
	const Vector2 glyphSize = {static_cast<float>(kDigitW) * kDigitScale, static_cast<float>(kDigitH) * kDigitScale};

	// 左から右へ並べて描画
	for (size_t i = 0; i < text.size(); ++i) {
		int gi = GlyphIndexFromChar(text[i]);
		float tileXf = static_cast<float>(gi * kDigitW);

		Sprite* sp = pool[i];
		Vector2 pos = {anchorLeftTop.x + static_cast<float>(i) * glyphSize.x, anchorLeftTop.y};
		sp->SetPosition(pos);
		sp->SetTextureRect({tileXf, static_cast<float>(kDigitsY)}, {static_cast<float>(kDigitW), static_cast<float>(kDigitH)});
		sp->SetSize(glyphSize);
		sp->Draw();
	}
}

void Hud::DrawTimer(int seconds) {
	if (sprTimer_)
		sprTimer_->Draw();

	// そのまま秒を表示（mm:ss にしたい場合はここで文字列を組み立て）
	std::string s = std::to_string(seconds);

	// ラベルの縦中央に数字を配置
	float numH = static_cast<float>(kDigitH) * kDigitScale;
	float y = posTimer_.y + (sizeTimer_.y - numH) * 0.5f;

	// ラベル右端 + マージン
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

void Hud::DrawLife(int /*life*/) {
	if (sprLife_)
		sprLife_->Draw();
}

void Hud::DrawSkill(int /*skill*/) {
	if (sprSkill_)
		sprSkill_->Draw();
}
