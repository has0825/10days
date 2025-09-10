#include "GameOver.h"
#include <numbers>

using namespace KamataEngine;

GameOverScene::~GameOverScene() {
	delete modelGameOver_;
	modelGameOver_ = nullptr;
}

void GameOverScene::Initialize() {
	// カメラ
	camera_.Initialize();
	camera_.translation_ = {2.0f, 0.0f, -30.0f};
	camera_.rotation_ = {0.0f, 0.0f, 0.0f};
	camera_.UpdateMatrix();

	// GameOver OBJ
	modelGameOver_ = Model::CreateFromOBJ("GameOverFont", true);

	// Transform
	wt_ = std::make_unique<WorldTransform>();
	wt_->Initialize();
	wt_->translation_ = {0, 0, 0};
	wt_->scale_ = {1, 1, 1};
	

	// フェード
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 0.6f);

	step_ = Step::FadeIn;

	// Audio のインスタンス取得
	auto* audio = Audio::GetInstance();

	// BGM 読み込み（WAV形式）
	bgmHandle_ = audio->LoadWave("./BGM/EVOLUTION.wav");

	// ループ再生 (volume=0.5)
	bgmVoice_ = audio->PlayWave(bgmHandle_, true, 0.5f);

}

void GameOverScene::Update() {
	const auto* input = Input::GetInstance();
	camera_.UpdateMatrix();
	if (fade_)
		fade_->Update();

	switch (step_) {
	case Step::FadeIn:
		if (fade_->IsFinished()) {
			fade_->Stop();
			step_ = Step::Idle;
		}
		break;
	case Step::Idle:
		if (input->TriggerKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 0.5f);
			step_ = Step::FadeOut;

			// BGM停止（安全版）
			if (!bgmStoppedOnGameOver_ && bgmVoice_ != 0) {
				try {
					Audio::GetInstance()->StopWave(bgmVoice_);
				} catch (...) {
					// 無視
				}
				bgmVoice_ = 0;
				bgmStoppedOnGameOver_ = true;
			}

		}
		break;
	case Step::FadeOut:
		if (fade_->IsFinished()) {
			step_ = Step::Done;
		}
		break;
	case Step::Done:
		break;
	}
}

void GameOverScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());
	if (modelGameOver_ && wt_) {
		modelGameOver_->Draw(*wt_, camera_);
	}
	Model::PostDraw();

	if (fade_)
		fade_->Draw();
}

bool GameOverScene::IsFinished() const { return step_ == Step::Done; }
