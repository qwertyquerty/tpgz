#ifndef TPGZ_MODULES_BOOT_INCLUDE_GAME_STATE_H
#define TPGZ_MODULES_BOOT_INCLUDE_GAME_STATE_H
#include <types.h>
#include "c/c_damagereaction.h"
#include "m_Do/m_Do_graphic.h"

extern BOOL l_fopScnRq_IsUsingOfOverlap;

#ifdef WII_PLATFORM
#pragma push
#pragma dollar_identifiers on
extern bool l_dvdError$0;

inline bool GZ_isDvdErrorShown() {
    return l_dvdError$0;
}
#pragma pop
#endif

#if defined(WII_NTSCJ)
extern u8 lbl_8051E770;

inline u8& GZ_getSkipInfo() {
    return lbl_8051E770;
}
#elif defined(WII_PAL)
extern u8 lbl_805210B0;

inline u8& GZ_getSkipInfo() {
    return lbl_805210B0;
}
#else
inline u8& GZ_getSkipInfo() {
    return cDmr_SkipInfo;
}
#endif

#if WIDESCREEN_SUPPORT
#define isWidescreen (mDoGph_gInf_c::isWide())
#else
#define isWidescreen (false)
#endif

#endif
