#pragma once
#include "Fade.h"
#include <KamataEngine.h>

using namespace KamataEngine;

class TitleScene {
public:
	TitleScene() = default;
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();

	// main.cpp からの遷移判定に使う
	bool IsFinished() const;

private:
	// ステップ（フェードイン→待機→フェードアウト→終了）
	enum class Step {
		FadeIn,
		Idle,
		FadeOut,
		Done,
	} step_ = Step::FadeIn;

	// 表示物
	Camera camera_{};
	Model* modelTitle_ = nullptr;
	std::unique_ptr<WorldTransform> titleWT_;

	// フェード
	std::unique_ptr<Fade> fade_;
};
