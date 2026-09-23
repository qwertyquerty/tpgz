#ifndef LIB_TP_MATH
#define LIB_TP_MATH

#ifndef M_PI
#define M_PI ((double)3.141592653589793238462643383279502884e+00)
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#include <stdint.h>



extern "C" {
double atan(double x);
double ceil(double x);
double copysign(double x, double y);
double cos(double x);
double floor(double x);
double frexp(double x, int* exponent);
double ldexp(double x, int exponent);
#ifndef WII_PLATFORM
double modf(double x, double* iptr);
#endif
double sin(double x);
double tan(double x);
double acos(double x);
double asin(double x);
double atan2(double x, double y);
double exp(double x);
double fmod(double x, double y);
double pow(double x, double y);
double fastSqrt(double x);
double sqrt(double x);
}

inline float fcos(float v) {
    return (float)cos(v);
}

inline float fsin(float v) {
    return (float)sin(v);
}

inline float fmodf(float f1, float f2) {
    return fmod(f1, f2);
}

#ifdef WII_PLATFORM
inline float std__fabsf(float x) {
    return (x < 0.0f) ? -x : x;
}
#else
extern "C" float fabsf__3stdFf(float);
inline float std__fabsf(float x) { return fabsf__3stdFf(x); }
#endif

extern int r0;
extern int r1;
extern int r2;

#endif  // !LIB_TP_MATH
