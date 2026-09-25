#include "menus/menu_tools_scene/include/tools_scene_menu.h"
#include <cstdio>
#include "commands.h"
#include "global_data.h"
#include "timer.h"
#include "d/d_com_inf_game.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

#ifdef GCN_PLATFORM
#define FREE_CAM_MOVEMENT_TEXT "Stick/L/R"
#define FREE_CAM_VIEW_TEXT "C-stick"
#endif
#ifdef WII_PLATFORM
#define FREE_CAM_MOVEMENT_TEXT "Stick+DPad"
#define FREE_CAM_VIEW_TEXT "C+Stick"
#endif

#define MAX_TUNIC_COLORS 7

const char l_descTemplates[TOOLS_SCENE_COUNT][100] = {
    "use %s to reload current area",
    "use %s to pause, %s to frame advance",
    FREE_CAM_TEXT " to activate, " FREE_CAM_MOVEMENT_TEXT " to move, " FREE_CAM_VIEW_TEXT " to view, Z to speed",
    "show Heap size info",
};

static Line lines[TOOLS_SCENE_COUNT] = {
    {"area reload", AREA_RELOAD_INDEX, "use " RELOAD_AREA_TEXT " to reload current area",
                 true, ACTIVE_FUNC(STNG_TOOLS_RELOAD_AREA)},
                {"frame advance", FRAME_ADVANCE_INDEX, "use " FRAME_ADVANCE_TEXT " to pause, " FRAME_PAUSE_TEXT " to frame advance",
                 true, ACTIVE_FUNC(STNG_TOOLS_FRAME_ADVANCE)},
                {"free cam", FREE_CAM_INDEX,
                 FREE_CAM_TEXT " to activate, " FREE_CAM_MOVEMENT_TEXT " to move, " FREE_CAM_VIEW_TEXT
                               " to view, Z to speed",
                 true, ACTIVE_FUNC(STNG_TOOLS_FREE_CAM)},
                {"heap debug info", HEAP_DEBUG_INDEX, "show Heap size info", true,
                 ACTIVE_FUNC(STNG_TOOLS_HEAP_DEBUG)}
};

KEEP_FUNC ToolsSceneMenu::ToolsSceneMenu(Cursor& cursor)
    : Menu(cursor) {}

ToolsSceneMenu::~ToolsSceneMenu() {}

GZSettingID l_mapping[] = {
    STNG_TOOLS_RELOAD_AREA,
    STNG_TOOLS_FRAME_ADVANCE,    
    STNG_TOOLS_FREE_CAM,
    STNG_TOOLS_HEAP_DEBUG,
};

#define set_active(id, status)                                                                     \
    ({                                                                                             \
        GZSettingEntry* stng = GZStng_get(id);                                                        \
        if (stng)                                                                                  \
            *(bool*)stng->data = status;                                                           \
    })

void ToolsSceneMenu::draw() {
    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {        
        GZSettingEntry* stng = NULL;

        stng = GZStng_get(l_mapping[cursor.y]);
        if (!stng) {
            stng = new GZSettingEntry(l_mapping[cursor.y], sizeof(bool), new bool);
            g_settings.push_back(stng);
        }

        if (stng)
            *(bool*)stng->data = !*(bool*)stng->data;
    }

    char buf[100];

    switch (cursor.y) {
    case AREA_RELOAD_INDEX: {
        uint16_t combo = GZStng_getData<uint16_t>(STNG_CMD_RELOAD_AREA, RELOAD_AREA_BUTTONS);
        char* comboStr = new char[GZCmd_getComboLen(combo) + 1];
        GZCmd_comboToStr(combo, comboStr);
        snprintf(buf, sizeof(buf), l_descTemplates[cursor.y], comboStr);
        delete[] comboStr;
        break;
    }
    case FRAME_ADVANCE_INDEX: {
        uint16_t comboPause =
            GZStng_getData<uint16_t>(STNG_CMD_FRAME_PAUSE, FRAME_PAUSE_BUTTONS);
        char* comboPauseStr = new char[GZCmd_getComboLen(comboPause) + 1];
        GZCmd_comboToStr(comboPause, comboPauseStr);
        uint16_t comboAdvance =
            GZStng_getData<uint16_t>(STNG_CMD_FRAME_ADVANCE, FRAME_ADVANCE_BUTTONS);
        char* comboAdvanceStr = new char[GZCmd_getComboLen(comboAdvance) + 1];
        GZCmd_comboToStr(comboAdvance, comboAdvanceStr);
        snprintf(buf, sizeof(buf), l_descTemplates[cursor.y], comboPauseStr, comboAdvanceStr);
        delete[] comboAdvanceStr;
        delete[] comboPauseStr;
        break;
    }
    default: {
        snprintf(buf, sizeof(buf), l_descTemplates[cursor.y]);
        break;
    }
    }

    strncpy(lines[cursor.y].description, buf, sizeof(lines[cursor.y].description));

    cursor.move(0, MENU_LINE_NUM);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
