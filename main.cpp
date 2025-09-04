#include "GameScene.h"
#include <KamataEngine.h>
#include <Windows.h>
using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジン初期化
	KamataEngine::Initialize(L"2048_パイ・パトロール");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	GameScene* gameScene = new GameScene();
	gameScene->Initialize();

	// メインループ
	while (true) {
		// エンジン更新
		if (KamataEngine::Update()) {
			break;
		}

		// 更新
		gameScene->Update();

		// 描画
		dxCommon->PreDraw();
		gameScene->Draw();
		dxCommon->PostDraw();
	}

	delete gameScene;
	gameScene = nullptr;

	// エンジン終了の処理
	KamataEngine::Finalize();

	return 0;
}
