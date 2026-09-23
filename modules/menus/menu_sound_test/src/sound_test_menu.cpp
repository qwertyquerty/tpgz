#include "menus/menu_sound_test/include/sound_test_menu.h"
#include <cstdio>
#include "settings.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_actor_mng.h"
#include "m_Do/m_Do_printf.h"
#include "m_Do/m_Do_audio.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

KEEP_FUNC SoundTestMenu::SoundTestMenu(Cursor& cursor)
    : Menu(cursor), lines{
          {"category id:", 0, "Select Sound Category ID", false},
          {"sound id:", 1, "Select Sound Effect ID", false},
          {"play", 2, "Play Sound Effect", false},
          {"stop", 3, "Stop Sound Effect", false},
      } {}

SoundTestMenu::~SoundTestMenu() {}

void SoundTestMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);
    static u32 l_categoryID = 0;
    static u32 l_soundID = 0;

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    switch (cursor.y) {
    case 0:
        Cursor::moveList(l_categoryID);
        if (l_categoryID > 9) {
            l_categoryID = 0;
        }
        break;
    case 1:
        Cursor::moveList(l_soundID);
        if (l_soundID > 0xFFFF) {
            l_soundID = 0;
        }
        break;
    }

    u32 composite_id = (l_categoryID << 0x10) | l_soundID;

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        switch (cursor.y) {
        case 2:
            mDoAud_seStart(composite_id, nullptr, 0, 0);
            break;
        case 3:
            mDoAud_seStop(composite_id, 0);
            break;
        }
    }

    lines[0].printf(" <%d>", l_categoryID);
    lines[1].printf(" <%d>", l_soundID);

    cursor.move(0, MENU_LINE_NUM);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}