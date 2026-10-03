#include <cmath>
#include <mtx.h>
#include "SSystem/SComponent/c_m3d.h"
#include "d/d_bg_s.h"
#include "d/d_bg_s_capt_poly.h"
#include "d/d_save.h"
#include "d/d_com_inf_game.h"
#include "d/actor/d_a_e_zs.h"

void dBgS::CaptPoly(dBgS_CaptPoly& capt) {
    cBgS_ChkElm* elm = m_chk_element;
    for (int i = 0; i < 0x100; i++) {
        if (elm->ChkUsed()) {
            elm->m_bgw_base_ptr->CaptPoly(capt);
        }
        elm++;
    }
}

void dSv_memBit_c::offDungeonItem(int i_no) {
    mDungeonItem &= (u8)~(u8)(1 << i_no);
}

void dSv_player_status_b_c::offTransformLV(int i_no) {
    mTransformLevelFlag &= (u8)~(u8)(1 << i_no);
}

void daE_ZS_c::setActionMode(int i_action, int i_mode) {
    mAction = i_action;
    mMode = i_mode;
}

bool cM3d_UpMtx_Base(const Vec& param_1, const Vec& param_2, Mtx param_3) {
    Vec local_3c;
    Vec auStack_48;

    if (cM3d_IsZero(PSVECMag(&param_2))) {
        PSMTXIdentity(param_3);
        return false;
    }

    PSVECNormalize(&param_2, &auStack_48);
    PSVECCrossProduct(&param_1, &auStack_48, &local_3c);

    if (cM3d_IsZero(PSVECMag(&local_3c))) {
        local_3c.x = 1.0f;
        local_3c.y = 0.0f;
        local_3c.z = 0.0f;
    }

    f32 dVar8 = PSVECDotProduct(&param_1, &auStack_48);
    if (dVar8 > 1.0f) {
        dVar8 = 1.0f;
    } else if (dVar8 < -1.0f) {
        dVar8 = -1.0f;
    }

    f32 angle = (f32)acos(dVar8);
    PSMTXRotAxisRad(param_3, &local_3c, angle);
    return true;
}

static Vec base_y = {0.0f, 1.0f, 0.0f};

bool cM3d_UpMtx(const Vec& param_1, Mtx param_2) {
    return cM3d_UpMtx_Base(base_y, param_1, param_2);
}
