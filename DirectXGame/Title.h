#pragma once
#include "Fade.h"
#include "Skydome.h"
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

	//model
	Model* modelSkydome_ = nullptr;     // 天球
	Camera* cameraPtr_ = nullptr;       // ★ Skydome が参照するのでポインタでも持つ

	// ▼ BGM用
	uint32_t bgmHandle_ = 0u;  // 読み込んだBGMデータ
	uint32_t bgmVoice_ = 0u;  // 再生中のハンドル
	bool bgmStoppedOnGameOver_ = false;

	// ============ 天球 ============
	Skydome* skydome_ = nullptr;
};
