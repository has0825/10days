#pragma once
#include "KamataEngine.h"

/// AL3サンプルプログラム用の数学ライブラリ。
/// MT3準拠で、KamataEngine内部の数学ライブラリと重複する。
/*
struct Matrix4x4 final {
    float m[4][4];
};

struct Vector4 final {
    float x;
    float y;
    float z;
    float w;
};

struct Vector3 final {
    float x;
    float y;
    float z;
};

struct Vector2 final {
    float x;
    float y;
};
*/
using namespace KamataEngine;

// 円周率
const float PI = 3.141592654f;

struct AABB {
	Vector3 min;
	Vector3 max;
};

// 02_14 29枚目 単項演算子オーバーロード
Vector3 operator+(const Vector3& v);
Vector3 operator-(const Vector3& v);

// 02_06のCameraControllerのUpdate/Reset関数で必要
const Vector3 operator+(const Vector3& lhv, const Vector3& rhv);

// 02_06のスライド24枚目のLerp関数
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// 02_06 スライド29枚目で追加
const Vector3 operator*(const Vector3& v1, const float f);

// 代入演算子オーバーロード
Vector3& operator+=(Vector3& lhs, const Vector3& rhv);
Vector3& operator-=(Vector3& lhs, const Vector3& rhv);
Vector3& operator*=(Vector3& v, float s);
Vector3& operator/=(Vector3& v, float s);

// 単位行列の作成
Matrix4x4 MakeIdentityMatrix();
// スケーリング行列の作成
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
// 回転行列の作成
Matrix4x4 MakeRotateXMatrix(float theta);
Matrix4x4 MakeRotateYMatrix(float theta);
Matrix4x4 MakeRotateZMatrix(float theta);
// 平行移動行列の作成
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
// アフィン変換行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

// 代入演算子オーバーロード
Matrix4x4& operator*=(Matrix4x4& lhm, const Matrix4x4& rhm);

// 2項演算子オーバーロード
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);

// ワールドトランスフォーム更新(02_03の最後)
void WorldTransformUpdate(WorldTransform& worldTransform);

float Lerp(float x1, float x2, float t);

float EaseIn(float x1, float x2, float t);

float EaseOut(float x1, float x2, float t);

float EaseInOut(float x1, float x2, float t);

bool IsCollision(const AABB& aabb1, const AABB& aabb2);

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// 02_15 で追加
inline float ToRadians(float degrees) { return degrees * (3.1415f / 180.0f); }
inline float ToDegrees(float radians) { return radians * (180.0f / 3.1415f); }
