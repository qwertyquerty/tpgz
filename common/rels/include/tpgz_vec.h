#ifndef TPGZ_COMMON_RELS_INCLUDE_TPGZ_VEC_H
#define TPGZ_COMMON_RELS_INCLUDE_TPGZ_VEC_H
#include <mtx.h>
#ifdef WII_PLATFORM
#include <kpad.h>
#else
typedef struct Vec2 {
    f32 x;
    f32 y;
} Vec2;
#endif

inline Vec2 makeVec2(f32 x, f32 y) {
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

struct CameraMatrix { Vec target; Vec pos; };

#endif
