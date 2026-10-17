#include "d/actor/d_a_alink.h"
#include "game_state.h"
#include "tpgz_utils.h"
#include "defines.h"
#include <cstdio>
#include "fs.h"
#include "os/OSCache.h"
#include "settings.h"
#include "gz_flags.h"
#include "practice.h"
#include "memfiles.h"
#include "save_manager.h"
#include "menus/menu.h"
#include "utils/loading.h"
#include "fifo_queue.h"
#include "utils/card.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_scene_req.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_camera_mng.h"
#include "menus/utils/menu_mgr.h"
#include "m_Do/m_Do_printf.h"

static char l_filename[80];
SaveManager gSaveManager;

KEEP_VAR bool SaveManager::s_injectSave = false;
KEEP_VAR bool SaveManager::s_injectMemfile = false;

void SaveManager::injectSave(void* buffer) {
    memcpy((void*)&g_dComIfG_gameInfo, buffer, 0x9F8);
    dComIfGs_getSave(*reinterpret_cast<s8*>(&g_dComIfG_gameInfo.info.getDan()));
}

void SaveManager::injectMemfile(void* buffer) {
    memcpy((void*)&g_dComIfG_gameInfo, buffer, sizeof(dSv_info_c));
    dComIfGs_getSave(*reinterpret_cast<s8*>(&g_dComIfG_gameInfo.info.getDan()));
}

void SaveManager::injectDefault_before() {
    dSv_restart_c& restart = g_dComIfG_gameInfo.info.getRestart();
    restart.setLastSceneInfo(0.0f, 0, restart.getLastAngleY());
    setNextStageWipe(13);  // instant load
}

void SaveManager::injectDefault_during() {}

void SaveManager::injectDefault_after() {}

void SaveManager::defaultLoad() {
    gSaveManager.mPracticeFileOpts.inject_options_during_load = SaveManager::injectDefault_during;
    gSaveManager.mPracticeFileOpts.inject_options_after_load = SaveManager::injectDefault_after;
    g_fifoVisible = true;
    g_menuMgr->hide();
}

void SaveManager::loadSave(uint32_t id, const char* category, special i_specials[], int size) {
    SaveManager::injectDefault_before();

    // Load the corresponding file path and properties
    snprintf(l_filename, sizeof(l_filename), "tpgz/save_files/%s.bin", category);
    loadFile(l_filename, &gSaveManager.mPracticeSaveInfo, sizeof(gSaveManager.mPracticeSaveInfo),
             id * sizeof(gSaveManager.mPracticeSaveInfo));
    snprintf(l_filename, sizeof(l_filename), "tpgz/save_files/%s/%s.bin", category,
             gSaveManager.mPracticeSaveInfo.filename);

    // 0xFF is used to identify a call from file reload, which doesn't need to run the default load
    if (size != 0xFF) {
        SaveManager::defaultLoad();
    } else {
        size = last_special_size;
    }

    if (gSaveManager.mPracticeSaveInfo.requirements) {
        gSaveManager.mPracticeFileOpts.inject_options_after_load =
            gSaveManager.mPracticeSaveInfo.requirements & REQ_CAM ? setPositionCamera : setLinkInfo;
        // gSaveManager.mPracticeFileOpts.inject_options_after_counter =
        //     gSaveManager.mPracticeSaveInfo.counter;
    }

    GZ_getSkipInfo() = 0;

    // If the selected file was a special, run the special callbacks
    if (i_specials) {
        for (int i = 0; i < size; ++i) {
            if (id == i_specials[i].idx) {
                if (i_specials[i].CallbackDuring) {
                    gSaveManager.mPracticeFileOpts.inject_options_during_load =
                        i_specials[i].CallbackDuring;
                }
                if (i_specials[i].CallbackAfter) {
                    gSaveManager.mPracticeFileOpts.inject_options_after_load =
                        i_specials[i].CallbackAfter;
                }

                last_special = i_specials[i];
                gSaveManager.mPracticeFileOpts.inject_options_after_counter = i_specials[i].extraDelay;
                break;
            }
        }
    }

    // Store all the info in case file reload is used
    last_save_index = id;
    strncpy(last_category, category, sizeof(last_category));
}

void SaveManager::loadSavefile(const char* l_filename) {
    loadFile(l_filename, MEMFILE_BUF, 2400, 0);
}

KEEP_FUNC void SaveManager::triggerLoad(uint32_t id, const char* category, special i_specials[],
                                        int size) {
    loadSave(id, category, i_specials, size);
 
    SaveManager::loadSavefile(l_filename);
    dSv_save_c* save = (dSv_save_c*)MEMFILE_BUF;

    dSv_player_return_place_c& returnPlace = save->getPlayer().getPlayerReturnPlace();
    const char* stageName = returnPlace.getName();
    const s8 roomNo = returnPlace.getRoomNo();
    const s16 point = returnPlace.getPlayerStatus();
    const s8 layer = dComIfG_play_c::getLayerNo_common_common(stageName, roomNo, 0xFF);

    g_dComIfG_gameInfo.info.getRestart().setStartPoint(point);
    g_dComIfG_gameInfo.play.mNextStage.dStage_startStage_c::set(stageName, roomNo, point, layer);

    // inject options after initial stage set since some options change stage loc
    if (gSaveManager.mPracticeFileOpts.inject_options_during_load) {
        gSaveManager.mPracticeFileOpts.inject_options_during_load();
    }

    enableNextStage();
    s_injectSave = true;
}

KEEP_FUNC void SaveManager::triggerMemfileLoad() {
    // GZ_readMemfile already puts the savedata in g_tmpBuf
    dSv_save_c* save = (dSv_save_c*)MEMFILE_BUF;

    dSv_player_return_place_c& returnPlace = save->getPlayer().getPlayerReturnPlace();
    const char* stageName = returnPlace.getName();
    const s8 roomNo = returnPlace.getRoomNo();
    const s16 point = returnPlace.getPlayerStatus();
    const s8 layer = dComIfG_play_c::getLayerNo_common_common(stageName, roomNo, 0xFF);

    g_dComIfG_gameInfo.info.getRestart().setStartPoint(point);
    g_dComIfG_gameInfo.play.mNextStage.dStage_startStage_c::set(stageName, roomNo, point, layer);

    gSaveManager.mPracticeFileOpts.inject_options_after_load = GZ_setLinkPosition;

    enableNextStage();
    s_injectMemfile = true;
}

// runs at the beginning of phase_1 of dScnPly_c load sequence
KEEP_FUNC void SaveManager::loadData() {
    if (s_injectMemfile) {
        SaveManager::injectMemfile(MEMFILE_BUF);
    } else {
        SaveManager::injectSave(MEMFILE_BUF);
    }

// swap equip logic
if (GZ_checkSwapEquips() && !s_injectMemfile) { // don't swap for memfiles

#ifdef GCN_PLATFORM
    uint8_t tmp = dComIfGs_getSelectItemIndex(SELECT_ITEM_X);
    uint8_t tmp_mix = dComIfGs_getMixItemIndex(SELECT_ITEM_X);

    dComIfGs_setSelectItemIndex(SELECT_ITEM_X, dComIfGs_getSelectItemIndex(SELECT_ITEM_Y));
    dComIfGs_setSelectItemIndex(SELECT_ITEM_Y, tmp);
    dComIfGs_setMixItemIndex(SELECT_ITEM_X, dComIfGs_getMixItemIndex(SELECT_ITEM_Y));
    dComIfGs_setMixItemIndex(SELECT_ITEM_Y, tmp_mix);
#endif

#ifdef WII_PLATFORM
    uint8_t tmp = dComIfGs_getSelectItemIndex(SELECT_ITEM_LEFT);
    uint8_t tmp_mix = dComIfGs_getMixItemIndex(SELECT_ITEM_LEFT);

    dComIfGs_setSelectItemIndex(SELECT_ITEM_LEFT,
                                dComIfGs_getSelectItemIndex(SELECT_ITEM_RIGHT));
    dComIfGs_setSelectItemIndex(SELECT_ITEM_RIGHT, tmp);
    dComIfGs_setMixItemIndex(SELECT_ITEM_LEFT, dComIfGs_getMixItemIndex(SELECT_ITEM_RIGHT));
    dComIfGs_setMixItemIndex(SELECT_ITEM_RIGHT, tmp_mix);
#endif
    }
}

void SaveManager::setLinkInfo() {
    if (dComIfGp_getPlayer(0)) {
        ((daAlink_c*)dComIfGp_getPlayer(0))->shape_angle.y = gSaveManager.mPracticeSaveInfo.angle;
        cXyz tmp(gSaveManager.mPracticeSaveInfo.position.x,
                 gSaveManager.mPracticeSaveInfo.position.y,
                 gSaveManager.mPracticeSaveInfo.position.z);
        ((daAlink_c*)dComIfGp_getPlayer(0))->current.pos = tmp;
    }
}

void SaveManager::setPositionCamera() {
    if (dComIfGp_getCamera(0)) {
        dComIfGp_getCamera(0)->mCamera.mViewCache.mCenter = gSaveManager.mPracticeSaveInfo.cam_target;
        dComIfGp_getCamera(0)->mCamera.mViewCache.mEye = gSaveManager.mPracticeSaveInfo.cam_pos;
    }

    if (dComIfGp_getPlayer(0)) {
        ((daAlink_c*)dComIfGp_getPlayer(0))->shape_angle.y = gSaveManager.mPracticeSaveInfo.angle;
        cXyz tmp(gSaveManager.mPracticeSaveInfo.position.x,
                 gSaveManager.mPracticeSaveInfo.position.y,
                 gSaveManager.mPracticeSaveInfo.position.z);
        ((daAlink_c*)dComIfGp_getPlayer(0))->current.pos = tmp;
    }
}

// set time of day to save time, needed to fix auto setting to midnight after non-twilight save is loaded in twilight
void SaveManager::updateTime() {
    dSv_save_c* save = (dSv_save_c*)MEMFILE_BUF;
    g_dComIfG_gameInfo.info.getPlayer().getPlayerStatusB().setTime(save->getPlayer().getPlayerStatusB().getTime());
}