#include "GameScene.h"
#include "Title.h"
#include "GameOver.h" // ★ 追加
#include <KamataEngine.h>
#include <Windows.h>
#include <memory>

using namespace KamataEngine;

enum class Scene {
	Title,
	Game,
	GameOver, // ★ 追加
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジン初期化
	KamataEngine::Initialize(L"2048_パイ・パトロール");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Scene scene = Scene::Title;

	// unique_ptr による安全な管理
	std::unique_ptr<TitleScene> titleScene = std::make_unique<TitleScene>();
	titleScene->Initialize();

	std::unique_ptr<GameScene> gameScene = nullptr;
	std::unique_ptr<GameOverScene> gameOverScene = nullptr; // ★ ここ追加

	// メインループ
	while (true) {
		if (KamataEngine::Update())
			break;

		switch (scene) {
		case Scene::Title:
			titleScene->Update();
			if (titleScene->IsFinished()) {
				titleScene.reset();
				gameScene = std::make_unique<GameScene>();
				gameScene->Initialize();
				scene = Scene::Game;
			}
			break;
		case Scene::Game:
			gameScene->Update();
			if (gameScene->IsGameOver()) {
				gameScene.reset();
				gameOverScene = std::make_unique<GameOverScene>();
				gameOverScene->Initialize();
				scene = Scene::GameOver;
			}
			break;
		case Scene::GameOver:
			gameOverScene->Update();
			if (gameOverScene->IsFinished()) {
				gameOverScene.reset();
				titleScene = std::make_unique<TitleScene>();
				titleScene->Initialize();
				scene = Scene::Title;
			}
			break;
		}

		dxCommon->PreDraw();
		switch (scene) {
		case Scene::Title:
			if (titleScene)
				titleScene->Draw();
			break;
		case Scene::Game:
			if (gameScene)
				gameScene->Draw();
			break;
		case Scene::GameOver:
			if (gameOverScene)
				gameOverScene->Draw();
			break;
		}
		dxCommon->PostDraw();
	}

	// 終了処理 (unique_ptrなのでdelete不要)
	titleScene.reset();
	gameScene.reset();

	// エンジン終了の処理
	KamataEngine::Finalize();

	return 0;
}
