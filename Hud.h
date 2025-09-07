#pragma once
#include <KamataEngine.h>
#include <array>
#include <string>

using namespace KamataEngine;

class Hud {
public:
    // フォント or 任意のUIテクスチャ。一枚でOK（ここでは "Font.png" 前提）
    void Initialize(const std::string& textureFile);

    // 数字はとりあえず描かず、ラベルのみ（安定確認用）
    void DrawTimer(int /*seconds*/);
    void DrawScore(int /*score*/);
    void DrawLife(int /*life*/);
    void DrawSkill(int /*skill*/);

private:
    uint32_t texHandle_ = 0u;

    // ラベル専用スプライト（必要に応じて増やす）
    Sprite* sprTimer_ = nullptr;
    Sprite* sprScore_ = nullptr;
    Sprite* sprLife_  = nullptr;
    Sprite* sprSkill_ = nullptr;

    // 配置
    Vector2 posTimer_ = {  40.0f,  40.0f };
    Vector2 posScore_ = {  40.0f,  80.0f };
    Vector2 posLife_  = {  40.0f, 120.0f };
    Vector2 posSkill_ = {  40.0f, 160.0f };
};
