#include "Hud.h"

void Hud::Initialize(const std::string& textureFile) {
	texHandle_ = TextureManager::Load(textureFile);

	// --- 画面サイズ想定値 ---


	// 縮小率
	const float scale = 1.2f;

	// --- Timer (左上) ---
	posTimer_ = {0.0f, 0.0f}; // 完全に端
	sprTimer_ = Sprite::Create(texHandle_, posTimer_);
	sprTimer_->SetTextureRect({0, 0}, {136, 53});
	sprTimer_->SetSize({136 * scale, 53 * scale});

	// --- life (左下) ---
	posLife_ = {0, 650}; // 下端にピッタリ
	sprLife_ = Sprite::Create(texHandle_, posLife_);
	sprLife_->SetTextureRect({144, 0}, {84, 53});
	sprLife_->SetSize({84 * scale, 53 * scale});

	// --- Score (右上) ---
	posScore_ = {900,0}; // 右端にピッタリ
	sprScore_ = Sprite::Create(texHandle_, posScore_);
	sprScore_->SetTextureRect({288, 0}, {136, 53});
	sprScore_->SetSize({136 * scale, 53 * scale});

	// --- Skill (右下) ---
	posSkill_ = {1000,650}; // 右下端にピッタリ
	sprSkill_ = Sprite::Create(texHandle_, posSkill_);
	sprSkill_->SetTextureRect({432, 0}, {100, 53});
	sprSkill_->SetSize({100 * scale, 53 * scale});
}

void Hud::DrawTimer(int /*seconds*/) {
	if (sprTimer_)
		sprTimer_->Draw();
}
void Hud::DrawScore(int /*score*/) {
	if (sprScore_)
		sprScore_->Draw();
}
void Hud::DrawLife(int /*life*/) {
	if (sprLife_)
		sprLife_->Draw();
}
void Hud::DrawSkill(int /*skill*/) {
	if (sprSkill_)
		sprSkill_->Draw();
}
