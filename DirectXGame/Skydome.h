#pragma once
#include <KamataEngine.h>

using namespace KamataEngine;

class Skydome {
public:

	void Initialize(Model* model, Camera*);

	void Update();

	void Draw();


private:

	//ワールド変換データ
	WorldTransform worldTransform_;

	//モデル
	Model* model_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

};

