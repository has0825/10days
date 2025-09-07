#include "Hud.h"

void Hud::Initialize(const std::string& textureFile) {
	// テクスチャは一度だけロード
	texHandle_ = TextureManager::Load(textureFile);

	// ラベル用スプライトを一度だけ作成（※ここで Create して、以後は Draw するだけ）
	sprTimer_ = Sprite::Create(texHandle_, posTimer_);
	sprScore_ = Sprite::Create(texHandle_, posScore_);
	sprLife_ = Sprite::Create(texHandle_, posLife_);
	sprSkill_ = Sprite::Create(texHandle_, posSkill_);

	// フォントテクスチャを使ってる場合、UV を割って文字出ししたくなるけど、
	// まずは安定確認のため絵（アイコン/プレースホルダ）だけ出す。
	// 本格的な数値描画はこの後に拡張しましょう（数字用スプライト配列など）。
}

void Hud::DrawTimer(int /*seconds*/) {
	if (sprTimer_) {
		sprTimer_->Draw();
	}
}
void Hud::DrawScore(int /*score*/) {
	if (sprScore_) {
		sprScore_->Draw();
	}
}
void Hud::DrawLife(int /*life*/) {
	if (sprLife_) {
		sprLife_->Draw();
	}
}
void Hud::DrawSkill(int /*skill*/) {
	if (sprSkill_) {
		sprSkill_->Draw();
	}
}
