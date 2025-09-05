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
