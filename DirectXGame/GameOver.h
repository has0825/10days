#pragma once
#include "Fade.h"
#include <KamataEngine.h>

using namespace KamataEngine;

class GameOverScene {
public:
	GameOverScene() = default;
	~GameOverScene();

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const; // Space押したらタイトルに戻る

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

	// ▼ BGM用
	uint32_t bgmHandle_ = 0u;  // 読み込んだBGMデータ
	uint32_t bgmVoice_ = 0u;  // 再生中のハンドル
	bool bgmStoppedOnGameOver_ = false;
};
