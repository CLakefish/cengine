#pragma once

#include "Vector3.h"
#include <stdio.h>

// https://www.emathhelp.net/calculators/linear-algebra/matrix-multiplication-calculator/?a=%5B%5B2%2C4%2C2%2C1%5D%2C%5B2%2C4%2C2%2C4%5D%2C%5B4%2C1%2C4%2C4%5D%2C%5B4%2C1%2C4%2C4%5D%5D&b=%5B%5B2%5D%2C%5B2%5D%2C%5B4%5D%2C%5B1%5D%5D
// ROW MAJOR

#ifndef MATRIX
#define MATRIX

typedef struct mat4x4_t {
	float m[16];
} mat4x4_t;

static inline void mat_Print(const mat4x4_t a) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			printf("%f,", a.m[(i * 4) + j]);
		}

		printf("\n");
	}
}

static inline mat4x4_t mat_Identity(void) {
	mat4x4_t mat = { {0} };
	mat.m[0] = 1;
	mat.m[5] = 1;
	mat.m[10] = 1;
	mat.m[15] = 1;

	return mat;
}

static inline mat4x4_t mat_Perspective(const float fov, const float aspect, const float zNear, const float zFar)
{
	mat4x4_t mat = mat_Identity();

	float t = tanf((fov / 2.0f) * DEG2RAD);

	mat.m[0] = 1.0f / (aspect * t);
	mat.m[5] = 1.0f / t;
	mat.m[10] = -(zFar + zNear) / (zFar - zNear);
	mat.m[11] = -(2.0f * zFar * zNear) / (zFar - zNear);
	mat.m[14] = -1;
	mat.m[15] = 0;

	return mat;
}

static inline mat4x4_t mat_Multiply(const mat4x4_t a, const mat4x4_t b) {
	mat4x4_t mat;

	for (int x = 0; x < 4; ++x) {
		for (int y = 0; y < 4; ++y) {
			float sum = 0;

			for (int r = 0; r < 4; ++r) {
				sum += a.m[(x * 4) + r] * b.m[(r * 4) + y];
			}

			mat.m[(x * 4) + y] = sum;
		}
	}

	return mat;
}

static inline vec3_t mat_VecMultiply(const vec3_t a, const mat4x4_t b) {
	vec3_t vec;

	vec.x = b.m[0] * a.x +
		b.m[1] * a.y +
		b.m[2] * a.z +
		b.m[3] * a.w;

	vec.y = b.m[4] * a.x +
		b.m[5] * a.y +
		b.m[6] * a.z +
		b.m[7] * a.w;

	vec.z = b.m[8] * a.x +
		b.m[9] * a.y +
		b.m[10] * a.z +
		b.m[11] * a.w;

	vec.w = b.m[12] * a.x +
		b.m[13] * a.y +
		b.m[14] * a.z +
		b.m[15] * a.w;

	return vec;
}

static inline mat4x4_t mat_Translate(const mat4x4_t m, const vec3_t pos) {
	mat4x4_t mat = m;
	mat.m[3] += pos.x;
	mat.m[7] += pos.y;
	mat.m[11] += pos.z;
	return mat;
}

static inline mat4x4_t mat_Transpose(const mat4x4_t m) {
	mat4x4_t mat;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			mat.m[j * 4 + i] = m.m[i * 4 + j];
		}
	}
	return mat;
}

static inline mat4x4_t mat_Scale(const mat4x4_t m, const vec3_t scale) {
	mat4x4_t mat = m;
	mat.m[0] += scale.x;
	mat.m[5] += scale.y;
	mat.m[10] += scale.z;
	mat.m[15] += scale.w;
	return mat;
}

static inline mat4x4_t mat_XRot(const mat4x4_t m, float angle) {
	mat4x4_t mat = m;
	mat.m[5] = cosf(angle);
	mat.m[6] = -sinf(angle);
	mat.m[9] = sinf(angle);
	mat.m[10] = cosf(angle);
	return mat;
}

static inline mat4x4_t mat_YRot(const mat4x4_t m, float angle) {
	mat4x4_t mat = m;
	mat.m[0] = cosf(angle);
	mat.m[2] = sinf(angle);
	mat.m[8] = -sinf(angle);
	mat.m[10] = cosf(angle);
	return mat;
}

static inline mat4x4_t mat_ZRot(const mat4x4_t m, float angle) {
	mat4x4_t mat = m;
	mat.m[0] = cosf(angle);
	mat.m[1] = -sinf(angle);
	mat.m[4] = sinf(angle);
	mat.m[5] = cosf(angle);
	return mat;
}

static inline mat4x4_t mat_Rot(const mat4x4_t m, const vec3_t dir) {
	mat4x4_t identity = mat_Identity();

	mat4x4_t matY = mat_YRot(identity, dir.y);
	mat4x4_t matX = mat_XRot(identity, dir.x);
	mat4x4_t matZ = mat_ZRot(identity, dir.z);

	mat4x4_t t = mat_Multiply(matY, matX);
	t = mat_Multiply(t, matZ);

	return mat_Multiply(t, m);
}

static inline mat4x4_t mat_LookAt(const vec3_t pos, const vec3_t dir, const vec3_t right, const vec3_t up)
{
	mat4x4_t a = mat_Identity();
	a.m[0] = right.x;
	a.m[1] = right.y;
	a.m[2] = right.z;

	a.m[4] = up.x;
	a.m[5] = up.y;
	a.m[6] = up.z;

	a.m[8] = dir.x;
	a.m[9] = dir.y;
	a.m[10] = dir.z;

	mat4x4_t b = mat_Identity();
	b = mat_Translate(b, (vec3_t) { -pos.x, -pos.y, -pos.z });

	return mat_Multiply(a, b);
}

#endif // !MATRIX