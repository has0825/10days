

#include "Hud.h"

void Hud::Initialize(const std::string& textureFile) {
	texHandle = TextureManager::Load(textureFile);

	// --- 画面サイズ想定値 ---

	// 縮小率
	const float scale = 1.2f;

	// --- Timer (左上) ---
	posTimer = {0.0f, 0.0f}; // 完全に端
	sprTimer = Sprite::Create(texHandle, posTimer);
	sprTimer->SetTextureRect({0, 0}, {136, 53});
	sprTimer->SetSize({136 * scale, 53 * scale});

	// --- life (左下) ---
	posLife = {0, 650}; // 下端にピッタリ
	sprLife = Sprite::Create(texHandle, posLife);
	sprLife->SetTextureRect({144, 0}, {84, 53});
	sprLife->SetSize({84 * scale, 53 * scale});

	// --- Score (右上) ---
	posScore = {900, 0}; // 右端にピッタリ
	sprScore = Sprite::Create(texHandle, posScore);
	sprScore->SetTextureRect({288, 0}, {136, 53});
	sprScore->SetSize({136 * scale, 53 * scale});

	// --- Skill (右下) ---
	posSkill = {1000, 650}; // 右下端にピッタリ
	sprSkill = Sprite::Create(texHandle, posSkill);
	sprSkill->SetTextureRect({432, 0}, {100, 53});
	sprSkill->SetSize({100 * scale, 53 * scale});
}

void Hud::DrawTimer(int /*seconds*/) {
	if (sprTimer)
		sprTimer->Draw();
}
void Hud::DrawScore(int /*score*/) {
	if (sprScore)
		sprScore->Draw();
}
void Hud::DrawLife(int /*life*/) {
	if (sprLife)
		sprLife->Draw();
}
void Hud::DrawSkill(int /*skill*/) {
	if (sprSkill)
		sprSkill->Draw();
}
