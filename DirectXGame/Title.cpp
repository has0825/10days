#include "Title.h"
#include <numbers>

using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete modelTitle_;
	modelTitle_ = nullptr;
}

void TitleScene::Initialize() {
	// カメラ（正面やや引き）
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -20.0f};
	camera_.rotation_ = {0.0f, 0.0f, 0.0f};
	camera_.UpdateMatrix();

	// タイトルのOBJ（titleFont フォルダ想定）
	modelTitle_ = Model::CreateFromOBJ("Title", true);

	// ワールドトランスフォーム
	titleWT_ = std::make_unique<WorldTransform>();
	titleWT_->Initialize();
	titleWT_->translation_ = {0.0f, 0.0f, 0.0f};
	titleWT_->rotation_ = {0.0f, 0.0f, 0.0f};
	titleWT_->scale_ = {1.0f, 1.0f, 1.0f};
	

	// フェード
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 0.6f); // 0.6秒でフェードイン
	step_ = Step::FadeIn;

	// Audio のインスタンス取得
	auto* audio = Audio::GetInstance();

	// BGM 読み込み（WAV形式）
	bgmHandle_ = audio->LoadWave("./BGM/Title.wav");

	// ループ再生 (volume=0.5)
	bgmVoice_ = audio->PlayWave(bgmHandle_, true, 0.5f);
}

void TitleScene::Update() {
	const auto* input = Input::GetInstance();

	// カメラ（必要なら演出で揺らしたい場合はここで）
	camera_.UpdateMatrix();

	// フェード更新
	if (fade_) {
		fade_->Update();
	}

	// ステップ制御
	switch (step_) {
	case Step::FadeIn:
		if (fade_ && fade_->IsFinished()) {
			fade_->Stop();
			step_ = Step::Idle;
		}
		break;

	case Step::Idle:
		// Space でゲームへ
		if (input->TriggerKey(DIK_SPACE)) {
			if (fade_) {
				fade_->Start(Fade::Status::FadeOut, 0.5f); // 0.5秒でフェードアウト
			}
			step_ = Step::FadeOut;

			// BGM 停止
			step_ = Step::FadeOut;
			auto* audio = Audio::GetInstance();
			audio->StopWave(bgmVoice_); // ← bgmVoice_ を渡す
			bgmStoppedOnGameOver_ = true;

		}
		break;

	case Step::FadeOut:
		if (fade_ && fade_->IsFinished()) {
			// ここでタイトル終了→main.cpp 側で GameScene を生成
			step_ = Step::Done;
		}
		break;

	case Step::Done:
		// 何もしない（main.cpp 側で破棄される）
		break;
	}
}

void TitleScene::Draw() {
	// 3Dモデル描画
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());
	if (modelTitle_ && titleWT_) {
		modelTitle_->Draw(*titleWT_, camera_);
	}
	Model::PostDraw();

	// フェード（黒板を Sprite レイヤで上描き）
	if (fade_) {
		fade_->Draw();
	}
}

bool TitleScene::IsFinished() const { return step_ == Step::Done; }
