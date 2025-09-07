#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

// 円周率
constexpr float PI = 3.141592654f;

struct AABB {
	Vector3 min;
	Vector3 max;
};

// ===== Vector3 演算子 =====
// 単項
Vector3 operator+(const Vector3& v);
Vector3 operator-(const Vector3& v);

// 二項
const Vector3 operator+(const Vector3& lhv, const Vector3& rhv);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v1, float f);

// 代入
Vector3& operator+=(Vector3& lhs, const Vector3& rhv);
Vector3& operator-=(Vector3& lhs, const Vector3& rhv);
Vector3& operator*=(Vector3& v, float s);
Vector3& operator/=(Vector3& v, float s);

// ===== 行列/変換 =====
Matrix4x4 MakeIdentityMatrix();
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeRotateXMatrix(float theta);
Matrix4x4 MakeRotateYMatrix(float theta);
Matrix4x4 MakeRotateZMatrix(float theta);
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);
Matrix4x4& operator*=(Matrix4x4& lhm, const Matrix4x4& rhm);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);

// WorldTransform → GPU へ転送までまとめた更新
void WorldTransformUpdate(WorldTransform& worldTransform);

// ===== 補間/イージング =====
float Lerp(float x1, float x2, float t);
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
float EaseIn(float x1, float x2, float t);
float EaseOut(float x1, float x2, float t);
float EaseInOut(float x1, float x2, float t);

// ===== 衝突/変換 =====
bool IsCollision(const AABB& aabb1, const AABB& aabb2);
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// ===== 角度ユーティリティ =====
inline float ToRadians(float degrees) { return degrees * (3.1415f / 180.0f); }
inline float ToDegrees(float radians) { return radians * (180.0f / 3.1415f); }

// ===== 円周ユーティリティ =====
float WrapAngle(float a);
float Clamp(float v, float lo, float hi);
float LenXZ(const Vector3& v);
float DotXZ(const Vector3& a, const Vector3& b);
Vector3 NormXZ(const Vector3& v);
Vector3 ReflectXZ(const Vector3& v, const Vector3& n);
