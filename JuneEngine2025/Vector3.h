#pragma once

#ifndef VECTOR3

#define VECTOR3

#include "EMath.h"
#include <stdio.h>

typedef struct vec3_t {
	float x, y, z, w;
} vec3_t;

static inline vec3_t vec3_Add(const vec3_t a, const vec3_t b) {
	return (vec3_t) { a.x + b.x, a.y + b.y, a.z + b.z };
}

static inline vec3_t vec3_Sub(const vec3_t a, const vec3_t b) {
	return (vec3_t) { a.x - b.x, a.y - b.y, a.z - b.z };
}

static inline vec3_t vec3_Mult(const vec3_t a, const float scalar) {
	return (vec3_t) { a.x * scalar, a.y * scalar, a.z * scalar };
}

static inline vec3_t vec3_Div(const vec3_t a, const float scalar) {
	return (vec3_t) { a.x / scalar, a.y / scalar, a.z / scalar };
}

static inline float vec3_Mag(const vec3_t a) {
	return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

static inline float vec3_Dot(const vec3_t a, const vec3_t b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static inline vec3_t vec3_Normalize(const vec3_t t) {
	float mag = vec3_Mag(t);
	if (mag == 0) return (vec3_t) { 0,0,0 };

	return vec3_Div(t, mag);
}

static inline vec3_t vec3_Cross(const vec3_t a, const vec3_t b) {
	return (vec3_t) { (a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x) };
}

static inline vec3_t vec3_Lerp(const vec3_t a, const vec3_t b, const float t) {
	return (vec3_t) { (float)LERP(a.x, b.x, t), (float)LERP(a.y, b.y, t), (float)LERP(a.z, b.z, t) };
}

// Keeps giving me a stupid fucking warning STFU MSVC!!!
//inline float vec3_Angle(const vec3_t a, const vec3_t b) {
//	vec3_t dir = vec3_Sub(a, b);
//	return RAD2DEG * (float)atan2((float)dir.y, (float)sqrt((float)((dir.x * dir.x) + (dir.z * dir.z))));
//}

// Love this post so much
// https://stackoverflow.com/questions/5188561/signed-angle-between-two-3d-vectors-with-same-origin-within-the-same-plane
static inline float vec3_SignedAngle(const vec3_t a, const vec3_t b, const vec3_t n) {
	return (vec3_Dot(vec3_Cross(a, b), n)) / vec3_Dot(a, b);
}

static inline float vec3_Distance(const vec3_t a, const vec3_t b) {
	return vec3_Mag(vec3_Sub(a, b));
}

static inline void vec3_Print(const vec3_t a) {
	printf("(%f, %f, %f)", a.x, a.y, a.z);
}

#endif