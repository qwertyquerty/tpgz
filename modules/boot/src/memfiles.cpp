#include "memfiles.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_scene_req.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_camera_mng.h"
#include "rels/include/defines.h"

KEEP_VAR PositionData memfile_posdata;

KEEP_FUNC void GZ_setLinkPosition() {
    dComIfGp_getPlayer(0)->current.pos = memfile_posdata.link;
    dComIfGp_getCamera(0)->mCamera.mViewCache.mCenter = memfile_posdata.cam.target;
    dComIfGp_getCamera(0)->mCamera.mViewCache.mEye = memfile_posdata.cam.pos;
    dComIfGp_getPlayer(0)->shape_angle.y = memfile_posdata.angle;
}
