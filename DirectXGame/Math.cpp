#include "Math.h"
#include <cmath>
#include <numbers>


// ===== Vector3 演算子 =====
Vector3 operator+(const Vector3& v) { return v; }
Vector3 operator-(const Vector3& v) { return Vector3(-v.x, -v.y, -v.z); }

const Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 t(v1);
	return t += v2;
}
const Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 t(v1);
	return t -= v2;
}
const Vector3 operator*(const Vector3& v1, float f) {
	Vector3 t(v1);
	return t *= f;
}

Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {
	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;
	return lhv;
}
Vector3& operator-=(Vector3& lhv, const Vector3& rhv) {
	lhv.x -= rhv.x;
	lhv.y -= rhv.y;
	lhv.z -= rhv.z;
	return lhv;
}
Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}
Vector3& operator/=(Vector3& v, float s) {
	v.x /= s;
	v.y /= s;
	v.z /= s;
	return v;
}

// ===== 行列作成 =====
Matrix4x4 MakeIdentityMatrix() {
	static const Matrix4x4 result{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	return result;
}
Matrix4x4 MakeScaleMatrix(const Vector3& s) {
	Matrix4x4 r{s.x, 0, 0, 0, 0, s.y, 0, 0, 0, 0, s.z, 0, 0, 0, 0, 1};
	return r;
}
Matrix4x4 MakeRotateXMatrix(float t) {
	float s = std::sin(t), c = std::cos(t);
	Matrix4x4 r{1, 0, 0, 0, 0, c, s, 0, 0, -s, c, 0, 0, 0, 0, 1};
	return r;
}
Matrix4x4 MakeRotateYMatrix(float t) {
	float s = std::sin(t), c = std::cos(t);
	Matrix4x4 r{c, 0, -s, 0, 0, 1, 0, 0, s, 0, c, 0, 0, 0, 0, 1};
	return r;
}
Matrix4x4 MakeRotateZMatrix(float t) {
	float s = std::sin(t), c = std::cos(t);
	Matrix4x4 r{c, s, 0, 0, -s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	return r;
}
Matrix4x4 MakeTranslateMatrix(const Vector3& t) {
	Matrix4x4 r{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, t.x, t.y, t.z, 1};
	return r;
}
Matrix4x4 MakeAffineMatrix(const Vector3& s, const Vector3& r, const Vector3& t) {
	Matrix4x4 ms = MakeScaleMatrix(s);
	Matrix4x4 rx = MakeRotateXMatrix(r.x);
	Matrix4x4 ry = MakeRotateYMatrix(r.y);
	Matrix4x4 rz = MakeRotateZMatrix(r.z);
	Matrix4x4 mr = rz * rx * ry;
	Matrix4x4 mt = MakeTranslateMatrix(t);
	return ms * mr * mt;
}
Matrix4x4& operator*=(Matrix4x4& l, const Matrix4x4& r) {
	Matrix4x4 o{};
	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			for (size_t k = 0; k < 4; k++)
				o.m[i][j] += l.m[i][k] * r.m[k][j];
	l = o;
	return l;
}
Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 r = a;
	return r *= b;
}

// ===== WorldTransform 更新 =====
void WorldTransformUpdate(WorldTransform& wt) {
	wt.matWorld_ = MakeAffineMatrix(wt.scale_, wt.rotation_, wt.translation_);
	wt.TransferMatrix(); // KamataEngine の定数バッファへ転送
}

// ===== 補間/イージング =====
float Lerp(float x1, float x2, float t) { return (1.0f - t) * x1 + t * x2; }
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) { return Vector3(Lerp(v1.x, v2.x, t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t)); }
float EaseIn(float x1, float x2, float t) { return Lerp(x1, x2, t * t); }
float EaseOut(float x1, float x2, float t) {
	float e = 1.0f - std::powf(1.0f - t, 3.0f);
	return Lerp(x1, x2, e);
}
float EaseInOut(float x1, float x2, float t) {
	float e = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
	return Lerp(x1, x2, e);
}

// ===== 衝突/変換 =====
bool IsCollision(const AABB& a, const AABB& b) { return (a.min.x <= b.max.x && a.max.x >= b.min.x) && (a.min.y <= b.max.y && a.max.y >= b.min.y) && (a.min.z <= b.max.z && a.max.z >= b.min.z); }
Vector3 Transform(const Vector3& v, const Matrix4x4& m) {
	Vector3 r;
	r.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + 1.0f * m.m[3][0];
	r.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + 1.0f * m.m[3][1];
	r.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + 1.0f * m.m[3][2];
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + 1.0f * m.m[3][3];
	r.x /= w;
	r.y /= w;
	r.z /= w;
	return r;
}

// ===== 円周ユーティリティ =====
float WrapAngle(float a) { return std::atan2(std::sinf(a), std::cosf(a)); }
float Clamp(float v, float lo, float hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }
float LenXZ(const Vector3& v) { return std::sqrt(v.x * v.x + v.z * v.z); }
float DotXZ(const Vector3& a, const Vector3& b) { return a.x * b.x + a.z * b.z; }
Vector3 NormXZ(const Vector3& v) {
	float l = LenXZ(v);
	return (l > 1e-6f) ? Vector3{v.x / l, 0.0f, v.z / l} : Vector3{0.0f, 0.0f, 0.0f};
}
// v' = v - 2(n・v) n（XZ 平面）
Vector3 ReflectXZ(const Vector3& v, const Vector3& n) {
	float d = DotXZ(v, n);
	return Vector3{v.x - 2.0f * d * n.x, 0.0f, v.z - 2.0f * d * n.z};
}
