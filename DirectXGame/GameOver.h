#pragma once
#include "Fade.h"
#include "Hud.h"
#include <KamataEngine.h>
#include <memory>                   // ★ unique_ptr 使うので追加

using namespace KamataEngine;

class GameOverScene {
public:
	GameOverScene() = default;
	~GameOverScene();

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const; // Space押したらタイトルに戻る
	//スコア取得
	void SetScore(int s) { finalScore_ = s; } // ★ リザルト用スコア受け取り

private:
	enum class Step {
		FadeIn,
		Idle,
		FadeOut,
		Done,
	} step_ = Step::FadeIn;

	Camera camera_{};
	Model* modelGameOver_ = nullptr;
	std::unique_ptr<WorldTransform> wt_;

	std::unique_ptr<Fade> fade_;

	// ★ 追加
	int finalScore_ = 0;

	// ★ ポインタではなく値で持つ（これがエラーの主因）
	Hud hud_;
};
