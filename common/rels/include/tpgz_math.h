#ifndef LIB_TP_MATH
#define LIB_TP_MATH

#include <cmath>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

inline float fcos(float v) {
    return (float)cos(v);
}

inline float fsin(float v) {
    return (float)sin(v);
}

extern int r0;
extern int r1;
extern int r2;

#endif  // !LIB_TP_MATH
