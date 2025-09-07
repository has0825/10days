#include "Hud.h"
#include <sstream>

Hud::~Hud() {}

bool Hud::Initialize(const std::string& texturePath) {
	texHandle_ = TextureManager::Load(texturePath);
	return texHandle_ != 0;
}

// ========== ラベル描画（上段） ==========

void Hud::DrawLabel(int x, int y, int index) {
// index: 0=Timer,1=Life,2=Score,3=Skill
int u = index * 128;
int v = 0;
int w = 128;
int h = kCharH;
Sprite* sp = Sprite::Create(texHandle_, {(float)x, (float)y});
sp->SetTextureRect(Vector2{(float)u, (float)v}, Vector2{(float)w, (float)h}); // Adjusted to match the expected arguments
sp->Draw();
delete sp;
}

void Hud::DrawNumber(int x, int y, int value) {
std::string s = std::to_string(value);
int dx = 0;
for (char c : s) {
	int idx = c - '0';
	int u = idx * kCharW;
	int v = kCharH; // 下段が数字列
	Sprite* sp = Sprite::Create(texHandle_, {(float)x + dx, (float)y});
	sp->SetTextureRect(Vector2{(float)u, (float)v}, Vector2{(float)kCharW, (float)kCharH}); // Adjusted to match the expected arguments
	sp->Draw();
	delete sp;
	dx += kCharW;
}
}

// ========== 各項目描画 ==========
void Hud::DrawTimer(int value) {
	DrawLabel(10, 10, 0); // Timer
	DrawNumber(150, 10, value);
}

void Hud::DrawLife(int value) {
	DrawLabel(10, 50, 1); // Life
	DrawNumber(150, 50, value);
}

void Hud::DrawScore(int value) {
	DrawLabel(10, 90, 2); // Score
	DrawNumber(150, 90, value);
}

void Hud::DrawSkill(int value) {
	DrawLabel(10, 130, 3); // Skill
	DrawNumber(150, 130, value);
}
