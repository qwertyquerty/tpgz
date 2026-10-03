#include "d/actor/d_a_alink.h"
#include "controller.h"
#include "features/moveactor/include/moveactor.h"
#include <cstdio>
#include "font.h"
#include "global_data.h"
#include "tpgz_math.h"
#include "settings.h"
#include "JSystem/JUtility/JUTGamePad.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_camera_mng.h"
#include "rels/include/defines.h"
#include "d/d_meter_HIO.h"
#include "f_pc/f_pc_name.h"

#define ROTATION_SPEED (30)
#define ROTATION_FAST_SPEED (80)
#define ROTATION_VERY_FAST_SPEED (800)
#define CAM_SPEED (1.0)
#define CAM_FAST_SPEED (2.5)
#define CAM_VERY_FAST_SPEED (10.0)
#define DIST_FROM_ACTOR (600)

#ifdef GCN_PLATFORM
#define CONTROL_Y (JUTGamePad::mPadStatus[0].stickY)
#define CONTROL_X (JUTGamePad::mPadStatus[0].stickX)
#define VERTICAL_DISPLACEMENT (JUTGamePad::mPadStatus[0].substickY)
#define HORIZONTAL_DISPLACEMENT -(JUTGamePad::mPadStatus[0].substickX)
#define SPEED_PREDICATE_1 (JUTGamePad::mPadButton[0].mButton & CButton::Z)
#define SPEED_PREDICATE_2 (JUTGamePad::mPadButton[0].mButton & CButton::R)
#define LOCK_CAMERA (JUTGamePad::mPadButton[0].mButton & CButton::L)
#endif

#ifdef WII_PLATFORM
#define CONTROL_Y ((GZ_getWiiPadStatus().hold & CButton::C) == 0 ? GZ_getWiiPadStatus().ex_status.fs.stick.y * 72 : 0)
#define CONTROL_X ((GZ_getWiiPadStatus().hold & CButton::C) == 0 ? -GZ_getWiiPadStatus().ex_status.fs.stick.x * 72 : 0)
#define VERTICAL_DISPLACEMENT ((GZ_getWiiPadStatus().hold & CButton::C) != 0 ? GZ_getWiiPadStatus().ex_status.fs.stick.y * 59 : 0)
#define HORIZONTAL_DISPLACEMENT ((GZ_getWiiPadStatus().hold & CButton::C) != 0 ? -GZ_getWiiPadStatus().ex_status.fs.stick.x * 59 : 0)
#define SPEED_PREDICATE_1 (GZ_getWiiPadStatus().hold & CButton::Z)
#define SPEED_PREDICATE_2 (GZ_getWiiPadStatus().hold & CButton::MINUS)
#define LOCK_CAMERA (GZ_getWiiPadStatus().hold & CButton::A)
#endif

#define WHITE_RGBA 0xFFFFFFFF
#define LINE_X_OFFSET 20.0f

namespace MoveActor {

double pitch = 0.0;
double yaw = 0.0;
float angle = 0.0f;
bool event_halt = false;

void move(fopAc_ac_c* actor) {
    // Fetch the camera position and target
    Vec& cam_target = dComIfGp_getCamera(0)->mCamera.mViewCache.mCenter;
    Vec& cam_pos = dComIfGp_getCamera(0)->mCamera.mViewCache.mEye;

    // Fetch the actor position and angles
    cXyz& actor_pos = actor->current.pos;
    s16& actor_horizontal_angle = actor->shape_angle.y;
    s16& actor_verticle_angle = actor->shape_angle.x;

    // Set Link momentum to 0
    cXyz tmp(0.0f, 0.0f, 0.0f);
    dComIfGp_getPlayer(0)->speed = tmp;

    if (!LOCK_CAMERA) {
        angle = (float)actor_horizontal_angle / 65536.f * (2 * M_PI);
    }

    // Fix Camera behind link
    cam_target.x = actor_pos.x;
    cam_target.y = actor_pos.y + 200.f;
    cam_target.z = actor_pos.z;
    cam_pos.z = actor_pos.z - DIST_FROM_ACTOR * cos(angle);
    cam_pos.x = actor_pos.x - DIST_FROM_ACTOR * sin(angle);
    cam_pos.y = actor_pos.y + 200.f;

    // Calculate the pitch and yaw
    yaw = atan2(cam_target.z - cam_pos.z, cam_target.x - cam_pos.x);
    double horizontal = sqrt((cam_target.x - cam_pos.x) * (cam_target.x - cam_pos.x) +
                                (cam_target.z - cam_pos.z) * (cam_target.z - cam_pos.z));
    pitch = atan2(cam_target.y - cam_pos.y, horizontal);

    // Calculate the translation
    double dy = LOCK_CAMERA ? 0.0f : VERTICAL_DISPLACEMENT;
    double dx = CONTROL_Y * cos(yaw) * cos(pitch) - CONTROL_X * sin(yaw);
    double dz = CONTROL_Y * sin(yaw) * cos(pitch) + CONTROL_X * cos(yaw);

    double move_speed = SPEED_PREDICATE_1 != 0 ? SPEED_PREDICATE_2 != 0 ? CAM_VERY_FAST_SPEED : CAM_FAST_SPEED : CAM_SPEED;
    int cam_speed = SPEED_PREDICATE_1 != 0 ? SPEED_PREDICATE_2 != 0 ? ROTATION_VERY_FAST_SPEED : ROTATION_FAST_SPEED : ROTATION_SPEED;

    // Apply the translation with a speed factor
    actor_pos.x += move_speed * dx;
    actor_pos.y += move_speed * dy;
    actor_pos.z += move_speed * dz;

    // Change facing angle with c stick
    if (LOCK_CAMERA) { 
        actor_verticle_angle -= -VERTICAL_DISPLACEMENT * cam_speed;
        actor_horizontal_angle -= -HORIZONTAL_DISPLACEMENT * cam_speed;
    } else {
        actor_horizontal_angle -= HORIZONTAL_DISPLACEMENT * cam_speed;
    }

}

KEEP_FUNC void execute() {
    if (g_actorViewEnabled || g_moveLinkEnabled) {
        // Hide HUD
        g_drawHIO.mParentAlpha = 0.0f;

        // Lock the camera to allow for its movement
        dComIfGp_getPEvtManager()->setCameraPlay(1);

        // Special case for Link (this needs to be refactored to be more generic)
        if (dComIfGp_getPlayer(0) && (g_moveLinkEnabled || g_currentActor->base.base.name == fpcNm_ALINK_e )) {
            ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.SetGrndNone();
            ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.SetWallNone();
            ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.SetRoofNone();
            ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.OnLineCheckNone();

            dComIfGp_getEvent()->mEventStatus = true;
            event_halt = true;

            move(dComIfGp_getPlayer(0));
        } else {
            dComIfGp_getEvent()->mEventStatus = false;
            event_halt = false;

            move(g_currentActor);
        }
    } else {
        if (event_halt) {
            if (dComIfGp_getPlayer(0)) {
                ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.ClrGrndNone();
                ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.ClrWallNone();
                ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.ClrRoofNone();
                ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.OffLineCheckNone();
            }

            dComIfGp_getEvent()->mEventStatus = false;
            event_halt = false;

            dComIfGp_getPEvtManager()->setCameraPlay(0);
            g_drawHIO.mParentAlpha = 1.0f;
        }


    }
}
}  // namespace MoveActor
