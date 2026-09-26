#include "controller.h"
#include "features/free_cam/include/free_cam.h"
#include "global_data.h"
#include "JSystem/JUtility/JUTGamePad.h"
#include "tpgz_math.h"
#include "menus/menu.h"
#include "d/d_com_inf_game.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_camera_mng.h"
#include "rels/include/defines.h"

#define ROTATION_SPEED (0.002)
#define FREECAM_FAST_SPEED (2.0)
#define FREECAM_SPEED (0.2)

#ifdef GCN_PLATFORM
#define CONTROL_Y (JUTGamePad::mPadStatus[0].stickY)
#define CONTROL_X (JUTGamePad::mPadStatus[0].stickX)
#define TRIG_ADJUST(trig) (JUTGamePad::mPadStatus[0].trig >= 10 ? JUTGamePad::mPadStatus[0].trig : 0)  // adjust sensitivity
#define VERTICAL_DISPLACEMENT (TRIG_ADJUST(triggerLeft) - TRIG_ADJUST(triggerRight))
#define SPEED_PREDICATE (JUTGamePad::mPadButton[0].mButton & CButton::Z)
#define PITCH_CONTROL (JUTGamePad::mPadStatus[0].substickY)
#define YAW_CONTROL (JUTGamePad::mPadStatus[0].substickX)
#endif

#ifdef WII_PLATFORM
#define CONTROL_Y ((GZ_getWiiPadStatus().hold & CButton::C) == 0 ? GZ_getWiiPadStatus().ex_status.fs.stick.y * 0x48 : 0)
#define CONTROL_X ((GZ_getWiiPadStatus().hold & CButton::C) == 0 ? -GZ_getWiiPadStatus().ex_status.fs.stick.x * 0x48 : 0)
#define VERTICAL_DISPLACEMENT                                                                      \
    ((GZ_getWiiPadStatus().hold & CButton::DPAD_UP ? 75 : 0) -                                              \
     (GZ_getWiiPadStatus().hold & CButton::DPAD_DOWN ? 75 : 0))
#define SPEED_PREDICATE (GZ_getWiiPadStatus().hold & CButton::Z)
#define PITCH_CONTROL ((GZ_getWiiPadStatus().hold & CButton::C) != 0 ? GZ_getWiiPadStatus().ex_status.fs.stick.y * 0x3B : 0)
#define YAW_CONTROL ((GZ_getWiiPadStatus().hold & CButton::C) != 0 ? -GZ_getWiiPadStatus().ex_status.fs.stick.x * 0x3B : 0)
#endif

namespace FreeCam {
bool init_once = false;
double pitch = 0.0;
double yaw = 0.0;

KEEP_FUNC void execute() {
    if (g_freeCamEnabled) {
        Vec& cam_target = dComIfGp_getCamera(0)->mCamera.mViewCache.mCenter;
        Vec& cam_pos = dComIfGp_getCamera(0)->mCamera.mViewCache.mEye;
        // Freeze the game to prevent control stick inputs to move link
        dComIfGp_getEvent()->mEventStatus = true;
        // Lock the camera to allow for its movement
        dComIfGp_getPEvtManager()->setCameraPlay(1);

        if (!init_once) {
            // Initialize the pitch and yaw to the current angle of the camera
            yaw = atan2(cam_target.z - cam_pos.z, cam_target.x - cam_pos.x);
            double horizontal = sqrt((cam_target.x - cam_pos.x) * (cam_target.x - cam_pos.x) +
                                     (cam_target.z - cam_pos.z) * (cam_target.z - cam_pos.z));
            pitch = atan2(cam_target.y - cam_pos.y, horizontal);
            init_once = true;
        }

        // Calculate the translation
        double dy = CONTROL_Y * sin(pitch) + VERTICAL_DISPLACEMENT;
        double dx = CONTROL_Y * cos(yaw) * cos(pitch) - CONTROL_X * sin(yaw);
        double dz = CONTROL_Y * sin(yaw) * cos(pitch) + CONTROL_X * cos(yaw);

        double speed = SPEED_PREDICATE != 0 ? FREECAM_FAST_SPEED : FREECAM_SPEED;
        // Apply the translation with a speed factor
        cam_pos.x += speed * dx;
        cam_pos.y += speed * dy;
        cam_pos.z += speed * dz;

        // Setup the target to correspond to the pitch and yaw
        cam_target.x = cam_pos.x + cos(yaw) * cos(pitch);
        cam_target.z = cam_pos.z + sin(yaw) * cos(pitch);
        cam_target.y = cam_pos.y + sin(pitch);

        // Update the pitch and yaw
        yaw += YAW_CONTROL * ROTATION_SPEED;
        yaw = fmod(yaw + 2 * M_PI, 2 * M_PI);
        pitch = MIN(MAX((pitch + PITCH_CONTROL * ROTATION_SPEED), -M_PI / 2 + 0.1), M_PI / 2 - 0.1);
    } else {
        if (init_once) {
            dComIfGp_getEvent()->mEventStatus = false;
            dComIfGp_getPEvtManager()->setCameraPlay(0);
            init_once = false;
        }
    }
}
}  // namespace FreeCam
