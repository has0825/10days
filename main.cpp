#include "GameScene.h"
#include "Title.h"
#include <KamataEngine.h>
#include <Windows.h>
#include <memory>

using namespace KamataEngine;

enum class Scene {
	Title,
	Game,
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

	// メインループ
	while (true) {
		// エンジン更新
		if (KamataEngine::Update()) {
			break;
		}

		// ====== シーンごとの更新 ======
		switch (scene) {
		case Scene::Title:
			titleScene->Update();
			if (titleScene->IsFinished()) {
				titleScene.reset(); // delete不要で自動解放

				gameScene = std::make_unique<GameScene>();
				gameScene->Initialize();
				scene = Scene::Game;
			}
			break;

		case Scene::Game:
			gameScene->Update();
			break;
		}

		// ====== 描画 ======
		dxCommon->PreDraw();
		switch (scene) {
		case Scene::Title:
			if (titleScene) {
				titleScene->Draw();
			}
			break;
		case Scene::Game:
			if (gameScene) {
				gameScene->Draw();
			}
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
