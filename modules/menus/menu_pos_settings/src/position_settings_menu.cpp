#include "controller.h"
#include "menus/menu_pos_settings/include/position_settings_menu.h"
#include <cstdio>
#include "utils/draw.h"
#include "fifo_queue.h"
#include "SSystem/SComponent/c_counter.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

#define INITAL_SPEED 1.0f
#define INCREMENT_FACTOR 1.05f
#define MAX_SPEED 20.f

static Line lines[SPRITES_AMNT] = {
    {"main menu", MENU_INDEX, "Change main menu position", false},
                {"input viewer", VIEWER_INDEX, "Change input viewer position", false},
                {"link debug info", DEBUG_INFO_INDEX, "Change link debug info position",
                 false},
                {"displacement", DISPLACEMENT_INDEX, "Change link displacement info position",
                 false},
                {"stage info", STAGE_INFO_INDEX, "Change link stage info position",
                 false},
                {"timer", TIMER_SPR_INDEX, "Change timer position", false},
                {"load timer", LOAD_TIMER_SPR_INDEX, "Change load timer position", false},
                {"igt timer", IGT_TIMER_SPR_INDEX, "Change IGT timer position", false},
                {"fifo queue", FIFO_SPR_INDEX, "Change fifo queue position", false},
                {"heap info", HEAP_INFO_INDEX, "Change Heap info position", false},
                {"mash checker", MASH_INFO_INDEX, "Change Mash Checker position", false},
                {"transform indicator", TRANSFORM_IND_INDEX, "Change Transform Indicator position", false}
};

KEEP_FUNC PosSettingsMenu::PosSettingsMenu(Cursor& cursor, PosSettingsData& data)
    : Menu(cursor), init_once(data.init_once), l_cursorSpeed(data.l_cursorSpeed),
      l_selItem(data.l_selItem) {}

PosSettingsMenu::~PosSettingsMenu() {}

#include "game_state.h"

void drawCursor(Vec2 pos) {
    bool cycle = (GZ_getFrameCount() / 8) % 2;
    if (GZ_checkDropShadows()) {
        Draw::drawRectOutline(DROP_SHADOWS_RGBA, makeVec2(pos.x - 10 * (isWidescreen ? 0.75f : 1.0f) + 1, pos.y + 1), makeVec2(20 * (isWidescreen ? 0.75f : 1.0f), 0), 0xA);
        Draw::drawRectOutline(DROP_SHADOWS_RGBA, makeVec2(pos.x + 1, pos.y - 10 + 1), makeVec2(0, 20), 0xA);
    }
    Draw::drawRectOutline(cycle ? g_cursorColor : 0xFFFFFFFF, makeVec2(pos.x - 10 * (isWidescreen ? 0.75f : 1.0f), pos.y), makeVec2(20 * (isWidescreen ? 0.75f : 1.0f), 0), 0xA);
    Draw::drawRectOutline(cycle ? g_cursorColor : 0xFFFFFFFF, makeVec2(pos.x, pos.y - 10), makeVec2(0, 20), 0xA);
}

GZSettingID l_mapping[] = {
    STNG_SPRITES_MENU,
    STNG_SPRITES_INPUT_VIEWER,
    STNG_SPRITES_DEBUG_INFO,
    STNG_SPRITES_STAGE_INFO,
    STNG_SPRITES_TIMER_SPR,
    STNG_SPRITES_LOAD_TIMER_SPR,
    STNG_SPRITES_IGT_TIMER_SPR,
    STNG_SPRITES_FIFO_SPR,
    STNG_SPRITES_HEAP_INFO,
    STNG_SPRITES_MASH_INFO,
    STNG_SPRITES_TRANSFORM_IND,
    STNG_SPRITES_DISPLACEMENT
};

void PosSettingsMenu::draw() {
    cursor.setMode(Cursor::MODE_UNRESTRICTED);

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        if (l_selItem != POSITION_SETTINGS_NO_SELECTION) {
            l_selItem = POSITION_SETTINGS_NO_SELECTION;
        } else {
            init_once = false;
            g_menuMgr->pop();
            return;
        }
    }

    if (!init_once) {
        l_selItem = POSITION_SETTINGS_NO_SELECTION;
        init_once = true;
    }

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        if (cursor.y < SPRITES_AMNT) {
            l_selItem = l_selItem == cursor.y ? POSITION_SETTINGS_NO_SELECTION : cursor.y;
        }
    }

    GZSettingEntry* stng = GZStng_get(l_mapping[l_selItem]);
    if (!stng) {
        stng = new GZSettingEntry(l_mapping[l_selItem], sizeof(Vec2), new Vec2(makeVec2(0.0f, 0.0f)));
        g_settings.push_back(stng);
    }
    Vec2* pos = static_cast<Vec2*>(stng->data);
    if (l_selItem != POSITION_SETTINGS_NO_SELECTION && l_selItem < SPRITES_AMNT) {
        if (GZ_getButtonRepeat(DPAD_RIGHT, 3)) {
            pos->x += l_cursorSpeed;
        }
        if (GZ_getButtonRepeat(DPAD_LEFT, 3)) {
            pos->x -= l_cursorSpeed;
        }
        if (GZ_getButtonRepeat(DPAD_UP, 3)) {
            pos->y -= l_cursorSpeed;
        }
        if (GZ_getButtonRepeat(DPAD_DOWN, 3)) {
            pos->y += l_cursorSpeed;
        }

        // Draw visual cursor
        drawCursor(*pos);
    }

    if (GZ_getButtonPressed(DPAD_RIGHT) || GZ_getButtonPressed(DPAD_LEFT) ||
        GZ_getButtonPressed(DPAD_UP) || GZ_getButtonPressed(DPAD_DOWN)) {
        if (l_cursorSpeed < MAX_SPEED) {
            l_cursorSpeed *= INCREMENT_FACTOR;
        }
        if (l_cursorSpeed > MAX_SPEED) {
            l_cursorSpeed = MAX_SPEED;
        }
    } else {
        l_cursorSpeed = INITAL_SPEED;
    }

    cursor.lock_x = l_selItem != POSITION_SETTINGS_NO_SELECTION;
    cursor.lock_y = l_selItem != POSITION_SETTINGS_NO_SELECTION;

    cursor.move(0, MENU_LINE_NUM);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
