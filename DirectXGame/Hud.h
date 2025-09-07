#pragma once
#include <KamataEngine.h>
#include <string>

using namespace KamataEngine;

class Hud {
public:
	Hud() = default;
	~Hud();

	// 初期化（フォント画像ロード）
	bool Initialize(const std::string& texturePath);

	// 各描画関数
	void DrawTimer(int value);
	void DrawLife(int value);
	void DrawScore(int value);
	void DrawSkill(int value);

private:
	uint32_t texHandle_ = 0;

	// 1文字のサイズ
	static const int kCharW = 32;
	static const int kCharH = 32;

	// ラベル描画
	void DrawLabel(int x, int y, int index);

	// 数字描画
	void DrawNumber(int x, int y, int value);
};
