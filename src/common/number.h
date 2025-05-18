#pragma once

#ifdef PRECISION_128
#include <quadmath.h>
typedef long double number;

#define LOG(X) logq(X)
#define LOG2(X) log2q(X)

#else
typedef double number;
#include <math.h>

#define LOG(X) log(X)
#define LOG2(X) log2(X)

#endif