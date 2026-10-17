#include "menus/menu_dungeon_flags/include/dungeon_flags_menu.h"
#include <cstdio>
#include "d/d_com_inf_game.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"
#include "tpgz_utils.h"

#define MAX_DUNGEON_OPTIONS 9

KEEP_VAR DungeonFlagsData* dungeonFlagsData = NULL;

static bool mapFlagActive() {
    return dungeonFlagsData->l_mapFlag;
}

static bool compassFlagActive() {
    return dungeonFlagsData->l_compassFlag;
}

static bool bossKeyFlagActive() {
    return dungeonFlagsData->l_bosskeyFlag;
}

static bool defeatMinibossFlagActive() {
    return dungeonFlagsData->l_minibossFlag;
}

static bool defeatBossFlagActive() {
    return dungeonFlagsData->l_bossFlag;
}

static Line lines[DUNGEON_FLAGS_COUNT] = {
      {"dungeon:", SELECT_DUNGEON_INDEX, "Selected dungeon flags", false, NULL,
       MAX_DUNGEON_OPTIONS},
      {"small keys", SMALL_KEY_FLAG_INDEX, "Selected dungeon small keys", false, NULL, 5},
    //   {"intro cutscene", INTRO_CS_FLAG_INDEX, "Toggle selected dungeon intro cutscene", true,
    //      [](){return dungeonFlagsData->l_introFlag;}},
      {"have map", MAP_FLAG_INDEX, "Give selected dungeon map", true, mapFlagActive},
      {"have compass", COMPASS_FLAG_INDEX, "Give selected dungeon compass", true,
       compassFlagActive},
      {"have boss key", BOSS_KEY_FLAG_INDEX, "Give selected dungeon boss key", true,
       bossKeyFlagActive},
      {"miniboss dead", DEFEAT_MINIBOSS_FLAG_INDEX, "Selected dungeon miniboss is defeated",
       true, defeatMinibossFlagActive},
      {"boss dead", DEFEAT_BOSS_FLAG_INDEX, "Selected dungeon boss is defeated", true,
       defeatBossFlagActive},
      {"clear flags", CLEAR_DUNGEON_FLAGS_INDEX, "Clear all selected dungeon flags"},
};

KEEP_FUNC DungeonFlagsMenu::DungeonFlagsMenu(Cursor& cursor)
    : Menu(cursor) {}

DungeonFlagsMenu::~DungeonFlagsMenu() {}

bool getSaveDungeonItem(int32_t stage, int32_t flag) {
    return g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().isDungeonItem(flag);
}

void setSaveDungeonItem(int32_t stage, int32_t flag) {
    if (getSaveDungeonItem(stage, flag)) {
        g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().offDungeonItem(flag);
    } else {
        g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().onDungeonItem(flag);
    }
}

#include "m_Do/m_Do_printf.h" // OSReport

bool getDungeonMemSwitch(int32_t stage, int32_t flag) {
    return g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().isSwitch(flag);
}

void setDungeonMemSwitch(int32_t stage, int32_t flag) {
    if (getDungeonMemSwitch(stage, flag)) {
        g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().onSwitch(flag);
    } else {
        g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().offSwitch(flag);
    }
}

uint8_t getSaveDungeonKeys(int32_t stage) {
    return g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().getKeyNum();
}

void setSaveDungeonKeys(int32_t stage, uint8_t num) {
    g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().setKeyNum(num);
}

void DungeonFlagsMenu::draw() {
    if (!dungeonFlagsData) {
        return;
    }
    cursor.setMode(Cursor::MODE_LIST);

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        dungeonFlagsData->init_once = false;
        g_menuMgr->pop();
        return;
    }

    uint8_t area_id = 0;
    switch (dungeonFlagsData->l_selDun) {
    case 0:
        area_id = 16;
        break;
    case 1:
        area_id = 17;
        break;
    case 2:
        area_id = 18;
        break;
    case 3:
        area_id = 19;
        break;
    case 4:
        area_id = 20;
        break;
    case 5:
        area_id = 21;
        break;
    case 6:
        area_id = 22;
        break;
    case 7:
        area_id = 23;
        break;
    case 8:
        area_id = 24;
        break;
    }

    if (!dungeonFlagsData->init_once) {
        dungeonFlagsData->l_keyNum = getSaveDungeonKeys(area_id);
        dungeonFlagsData->init_once = true;
    }

    switch (cursor.y) {
    case SELECT_DUNGEON_INDEX:
        cursor.x = dungeonFlagsData->l_selDun;
        cursor.move(MAX_DUNGEON_OPTIONS, MENU_LINE_NUM);

        if (cursor.y == SELECT_DUNGEON_INDEX) {
            dungeonFlagsData->l_selDun = cursor.x;
        }
        dungeonFlagsData->l_keyNum = getSaveDungeonKeys(area_id);
        break;
    case SMALL_KEY_FLAG_INDEX:
        cursor.x = dungeonFlagsData->l_keyNum;
        cursor.move(6, MENU_LINE_NUM);

        if (cursor.y == SMALL_KEY_FLAG_INDEX) {
            dungeonFlagsData->l_keyNum = cursor.x;
            setSaveDungeonKeys(area_id, dungeonFlagsData->l_keyNum);
            dComIfGs_getSave(*reinterpret_cast<s8*>(&g_dComIfG_gameInfo.info.getDan()));
        }
        break;
    default:
        cursor.move(0, MENU_LINE_NUM);
        break;
    }

    // update flags
    // switch (area_id) {
    //     case 16:
    //         dungeonFlagsData->l_introFlag = getDungeonMemSwitch(area_id, 105);
    //         break;
    //     case 17:
    //         dungeonFlagsData->l_introFlag = getDungeonMemSwitch(area_id, 85);
    //         break;
    //     case 18:
    //         dungeonFlagsData->l_introFlag = getDungeonMemSwitch(area_id, 120);
    //         break;
    //     case 19:
    //         dungeonFlagsData->l_introFlag = getDungeonMemSwitch(area_id, 124);
    //         break;
    //     case 20:
    //         dungeonFlagsData->l_introFlag = getDungeonMemSwitch(area_id, 120);
    //         break;
    //     case 21:
    //         dungeonFlagsData->l_introFlag = getDungeonMemSwitch(area_id, 21);
    //         break;
    //     case 22:
    //         dungeonFlagsData->l_introFlag = getDungeonMemSwitch(area_id, 111);
    //         break;
    //     case 23:
    //         dungeonFlagsData->l_introFlag = getDungeonMemSwitch(area_id, 1);
    //         break;
    //     case 24:
    //         dungeonFlagsData->l_introFlag = getDungeonMemSwitch(area_id, 98);
    //         break;
    // }

    dungeonFlagsData->l_mapFlag = getSaveDungeonItem(area_id, dSv_memBit_c::MAP);
    dungeonFlagsData->l_compassFlag = getSaveDungeonItem(area_id, dSv_memBit_c::COMPASS);
    dungeonFlagsData->l_bosskeyFlag = getSaveDungeonItem(area_id, dSv_memBit_c::BOSS_KEY);
    dungeonFlagsData->l_minibossFlag = getSaveDungeonItem(area_id, dSv_memBit_c::STAGE_BOSS_ENEMY_2);
    dungeonFlagsData->l_bossFlag = getSaveDungeonItem(area_id, dSv_memBit_c::STAGE_BOSS_ENEMY);

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        switch (cursor.y) {
        // case INTRO_CS_FLAG_INDEX:
        //     switch (area_id) {
        //     case 16:
        //         setDungeonMemSwitch(area_id, 105);
        //         break;
        //     case 17:
        //         setDungeonMemSwitch(area_id, 85);
        //         break;
        //     case 18:
        //         setDungeonMemSwitch(area_id, 120);
        //         break;
        //     case 19:
        //         setDungeonMemSwitch(area_id, 124);
        //         break;
        //     case 20:
        //         setDungeonMemSwitch(area_id, 120);
        //         break;
        //     case 21:
        //         setDungeonMemSwitch(area_id, 21);
        //         break;
        //     case 22:
        //         setDungeonMemSwitch(area_id, 111);
        //         break;
        //     case 23:
        //         setDungeonMemSwitch(area_id, 1);
        //         break;
        //     case 24:
        //         setDungeonMemSwitch(area_id, 98);
        //         break;
        //     }
        //     break;
        case MAP_FLAG_INDEX:
            setSaveDungeonItem(area_id, dSv_memBit_c::MAP);
            break;
        case COMPASS_FLAG_INDEX:
            setSaveDungeonItem(area_id, dSv_memBit_c::COMPASS);
            break;
        case BOSS_KEY_FLAG_INDEX:
            setSaveDungeonItem(area_id, dSv_memBit_c::BOSS_KEY);
            break;
        case DEFEAT_MINIBOSS_FLAG_INDEX:
            setSaveDungeonItem(area_id, dSv_memBit_c::STAGE_BOSS_ENEMY_2);
            break;
        case DEFEAT_BOSS_FLAG_INDEX:
            setSaveDungeonItem(area_id, dSv_memBit_c::STAGE_BOSS_ENEMY);
            break;
        case CLEAR_DUNGEON_FLAGS_INDEX:
            g_dComIfG_gameInfo.info.getSavedata().getSave(area_id).getBit().init();
            dungeonFlagsData->l_keyNum = 0;
            break;
        }
        // copy current stage save flags over temp flags
        dComIfGs_getSave(*reinterpret_cast<s8*>(&g_dComIfG_gameInfo.info.getDan()));
    }

    ListMember dun_opt[MAX_DUNGEON_OPTIONS] = {
        "Forest Temple",     "Goron Mines",        "Lakebed Temple",
        "Arbiter's Grounds", "Snowpeak Ruins",     "Temple of Time",
        "City in the Sky",   "Palace of Twilight", "Hyrule Castle",
    };

    lines[SMALL_KEY_FLAG_INDEX].printf(" <%d>", dungeonFlagsData->l_keyNum);
    lines[SELECT_DUNGEON_INDEX].printf(" <%s>", dun_opt[dungeonFlagsData->l_selDun].member);

    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
