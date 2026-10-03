#include "d/d_meter2_info.h"
#include "f_op/f_op_actor_tag.h"
#include <cstdio>
#include "menus/menu_actor_list/include/actor_list_menu.h"
#include "memory_editor.h"
#include "settings.h"
#include "d/d_com_inf_game.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_camera_mng.h"
#include "f_pc/f_pc_name.h"
#include "d/d_menu_window.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"
#include "fs.h"
#include "global_data.h"
#include "boot/include/collision_view.h"
#include "m_Do/m_Do_printf.h"
#include "d/d_meter_HIO.h"

static u8& GZ_getMenuStatus(dMw_c* menu) {
    return *reinterpret_cast<u8*>(reinterpret_cast<uintptr_t>(menu) + 0x146);
}

#ifdef GCN_PLATFORM
#define CONTROLLER_RIGHT DPAD_RIGHT
#define CONTROLLER_LEFT DPAD_LEFT
#define CONTROLLER_A A
#define MEM_SWITCH_BTN Z
#define SLOW_INC_BTN X
#define FAST_INC_BTN Y
#define MEM_TEXT "Z"
#define SLOW_INC_TEXT "X"
#define FAST_INC_TEXT "Y"
#define DELETE_TEXT "START"
#define DELETE_BUTTON START
#endif
#ifdef WII_PLATFORM
#define CONTROLLER_RIGHT DPAD_RIGHT
#define CONTROLLER_LEFT DPAD_LEFT
#define CONTROLLER_A A
#define MEM_SWITCH_BTN ONE
#define SLOW_INC_BTN Z
#define FAST_INC_BTN C
#define MEM_TEXT "1"
#define SLOW_INC_TEXT "Z"
#define FAST_INC_TEXT "C"
#define DELETE_TEXT "PLUS"
#define DELETE_BUTTON PLUS
#endif

/**
 * @brief Used for storing entries from procs.bin
 */
procBinData l_procData;

/**
 * @brief Checks and closes any menu that's currently open.
 * 
 * @details This function is used to close any menu that's currently open. 
 * It stores the current menu status and then closes the menu by setting the status to none and running the currently open menu's closer function via move_proc function table.
 * 
 */
void ActorListMenu::checkAndCloseMenu() {
    if (g_meter2_info.mMenuWindowClass) {
        switch (GZ_getMenuStatus(g_meter2_info.mMenuWindowClass)) {
        case dMw_c::NO_MENU:
            dComIfGp_offPauseFlag();
            break;
        default:
            l_menuStatus = GZ_getMenuStatus(g_meter2_info.mMenuWindowClass);
            l_windowStatus = g_meter2_info.mWindowStatus;
            g_meter2_info.offMenuInForce(GZ_getMenuStatus(g_meter2_info.mMenuWindowClass));
            GZ_getMenuStatus(g_meter2_info.mMenuWindowClass) = dMw_c::NO_MENU;
            g_meter2_info.mWindowStatus = dMw_c::NO_MENU;
            dComIfGp_offPauseFlag();
            break;
        }
    }       
}

/**
 * @brief Checks and restores any menu that was previously open.
 * 
 * @details This function is used to restore any menu that was previously open. 
 * It restores the menu by setting the status to the stored status and setting the currently stored menu status's corresponding window status.
 * 
 * @note There doesn't seem to be the need to run the opener function like there is with the checkAndCloseMenu function.
 * Just restoring the menu status and window status seems to be enough.
 * 
 */
void ActorListMenu::checkAndRestoreMenu() {
    if (l_menuStatus != dMw_c::NO_MENU) {
        dComIfGp_onPauseFlag();
        g_meter2_info.mWindowStatus = l_windowStatus;
        GZ_getMenuStatus(g_meter2_info.mMenuWindowClass) = l_menuStatus;
    }
}

static Line lines[ACTOR_LIST_LINE_COUNT] = {
    {"", ACTOR_NAME_INDEX, "A: freeze actor, " DELETE_TEXT ": delete actor, " MEM_TEXT " view memory", false},
    {"", ACTOR_POSITION_X_INDEX, "dpad: +/-100.0, " SLOW_INC_TEXT "+dpad: +/-1.0, " FAST_INC_TEXT "+dpad: +/-1000.0", false},
    {"", ACTOR_POSITION_Y_INDEX, "dpad: +/-100.0, " SLOW_INC_TEXT "+dpad: +/-1.0, " FAST_INC_TEXT "+dpad: +/-1000.0", false},
    {"", ACTOR_POSITION_Z_INDEX, "dpad: +/-100.0, " SLOW_INC_TEXT "+dpad: +/-1.0, " FAST_INC_TEXT "+dpad: +/-1000.0", false},
    {"", ACTOR_ANGLE_X_INDEX, "dpad: +/-100, " SLOW_INC_TEXT "+dpad: +/-1, " FAST_INC_TEXT "+dpad: +/-1000", false},
    {"", ACTOR_ANGLE_Y_INDEX, "dpad: +/-100, " SLOW_INC_TEXT "+dpad: +/-1, " FAST_INC_TEXT "+dpad: +/-1000", false},
    {"", ACTOR_ANGLE_Z_INDEX, "dpad: +/-100, " SLOW_INC_TEXT "+dpad: +/-1, " FAST_INC_TEXT "+dpad: +/-1000", false},
    {"", ACTOR_ADDRESS_INDEX, "current actor address", false},
    {"", ACTOR_PROC_INDEX, "current actor proc id", false},
    {"", ACTOR_PARAMS_INDEX, "current actor parameters", false},
};

KEEP_FUNC ActorListMenu::ActorListMenu(Cursor& cursor, ActorListData& data)
        : Menu(cursor),
          l_index(data.l_index) {
            // store camera position and target
            l_cameraPos = dComIfGp_getCamera(0)->mCamera.mViewCache.mEye;
            l_cameraTarget = dComIfGp_getCamera(0)->mCamera.mViewCache.mCenter;

            // remove any currently open menus
            checkAndCloseMenu();

            // initial data load from procs.bin
            updateActorData();
            loadActorName();
        }

ActorListMenu::~ActorListMenu() {
    // restore any previously open menu
    checkAndRestoreMenu();

    // restore camera position and target
    dComIfGp_getCamera(0)->mCamera.mViewCache.mEye = l_cameraPos;
    dComIfGp_getCamera(0)->mCamera.mViewCache.mCenter = l_cameraTarget;

    // restore evt manager camera play & HUD
    dComIfGp_getPEvtManager()->setCameraPlay(0);
    g_drawHIO.mParentAlpha = 1.0f;
}

template <typename T>
void ActorListMenu::updateValue(T* value, bool increase) {
    if (value != NULL) {
        f32 change;
        GZ_getButtonPressed(FAST_INC_BTN) ? change = 1000.0f : GZ_getButtonPressed(SLOW_INC_BTN) ? change = 1.0f : change = 100.0f;

        *value += (increase ? 1 : -1) * change;
    }
}

void ActorListMenu::updateActorData() {
    node_class* node = g_fopAcTg_Queue.mpHead;
    fopAc_ac_c* actorData = NULL;

    for (int i = 0; i < g_fopAcTg_Queue.mSize; i++) {
        if (i == l_index && node != NULL) {
            create_tag_class* tag = (create_tag_class*)node;
            actorData = (fopAc_ac_c*)tag->mpTagData;
            break;
        }

        node = node->mpNextNode;
    }

    g_currentActor = actorData;
}

void ActorListMenu::loadActorName() {
    if (g_currentActor) {
        int offset = (g_currentActor->base.base.name*32);
        loadFile("tpgz/procs.bin", &l_procData, sizeof(l_procData), offset);
    }
}

void ActorListMenu::draw() {
    g_actorViewEnabled = true;
    cursor.setMode(Cursor::MODE_LIST);

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        // disable gadget and close menu
        g_actorViewEnabled = false;
        g_menuMgr->pop();
        return;
    }

    bool rightPressed = GZ_getButtonRepeat(CONTROLLER_RIGHT,1);
    bool leftPressed = GZ_getButtonRepeat(CONTROLLER_LEFT,1);
    

    switch (cursor.y) {
    case ACTOR_NAME_INDEX:
        if (GZ_getButtonRepeat(CONTROLLER_RIGHT)) {
            l_index++;
            if (l_index > g_fopAcTg_Queue.mSize - 1)
                l_index = 0;

            updateActorData();
            loadActorName();
        } 
        
        if (GZ_getButtonRepeat(CONTROLLER_LEFT)) {
            l_index--;
            if (l_index > g_fopAcTg_Queue.mSize - 1)
                l_index = g_fopAcTg_Queue.mSize - 1;

            updateActorData();
            loadActorName();
        }
        
        if (GZ_getButtonRepeat(DELETE_BUTTON)) {
            if (g_currentActor) {
                if (g_currentActor->base.base.name != fpcNm_ALINK_e) {
                    fopAcM_delete(g_currentActor);
                }
            }
        }

        if (GZ_getButtonRepeat(CONTROLLER_A)) {
            if (g_currentActor) {
                g_currentActor->base.base.pause_flag = !g_currentActor->base.base.pause_flag;
            }
        }

        if (GZ_getButtonTrig(MEM_SWITCH_BTN)) {
            switch (cursor.y) {
            case ACTOR_NAME_INDEX:
                g_memoryEditor_addressIndex = (uint32_t)g_currentActor;
                g_menuMgr->push(MN_MEMORY_EDITOR_INDEX);
                return;
            }
        }

        break;
    case ACTOR_POSITION_X_INDEX:
        if (rightPressed || leftPressed) {
            updateValue(&g_currentActor->current.pos.x, rightPressed);
        }
        break;
    case ACTOR_POSITION_Y_INDEX:
        if (rightPressed || leftPressed) {
            updateValue(&g_currentActor->current.pos.y, rightPressed);
        }
        break;
    case ACTOR_POSITION_Z_INDEX:
        if (rightPressed || leftPressed) {
            updateValue(&g_currentActor->current.pos.z, rightPressed);
        }
        break;
    case ACTOR_ANGLE_X_INDEX:
        if (rightPressed || leftPressed) {
            updateValue(&g_currentActor->shape_angle.x, rightPressed);
        }
        break;
    case ACTOR_ANGLE_Y_INDEX:
        if (rightPressed || leftPressed) {
            updateValue(&g_currentActor->shape_angle.y, rightPressed);
        }
        break;
    case ACTOR_ANGLE_Z_INDEX:
        if (rightPressed || leftPressed) {
            updateValue(&g_currentActor->shape_angle.z, rightPressed);
        }
        break;
    case ACTOR_PROC_INDEX:
    case ACTOR_PARAMS_INDEX:
        // allowing arbitrary updates here causes frequent crashes. removing for now.
        break;
    }

    if (g_currentActor) {
        lines[ACTOR_NAME_INDEX].printf("name:  <%s>", l_procData.procName);
        lines[ACTOR_POSITION_X_INDEX].printf("pos-x: <%.1f>", g_currentActor->current.pos.x);
        lines[ACTOR_POSITION_Y_INDEX].printf("pos-y: <%.1f>", g_currentActor->current.pos.y);
        lines[ACTOR_POSITION_Z_INDEX].printf("pos-z: <%.1f>", g_currentActor->current.pos.z);
        lines[ACTOR_ANGLE_X_INDEX].printf("rot-x: <0x%04X>", static_cast<u16>(g_currentActor->shape_angle.x));
        lines[ACTOR_ANGLE_Y_INDEX].printf("rot-y: <0x%04X>", static_cast<u16>(g_currentActor->shape_angle.y));
        lines[ACTOR_ANGLE_Z_INDEX].printf("rot-z: <0x%04X>", static_cast<u16>(g_currentActor->shape_angle.z));
        lines[ACTOR_ADDRESS_INDEX].printf("addr: 0x%08X", g_currentActor);
        lines[ACTOR_PROC_INDEX].printf("proc id: %d", g_currentActor->base.base.name);
        lines[ACTOR_PARAMS_INDEX].printf("params: 0x%08X", g_currentActor->base.base.parameters);
    }

    cursor.move(0, ACTOR_LIST_LINE_COUNT);
    GZ_drawMenuLines(lines, cursor.y, ACTOR_LIST_LINE_COUNT);
}
