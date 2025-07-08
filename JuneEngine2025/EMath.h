#ifndef EMATH
#define EMATH

#include <math.h>

#define PI  3.14159265359f
#define TAU 6.28318530718f

#define RAD2DEG 57.2957795131f
#define DEG2RAD 0.01745329251f

#define CLAMP(a,b,c) a = fmaxf(b, fminf(a,c))
#define LERP(a,b,t) a + (b - a) * t

#endif