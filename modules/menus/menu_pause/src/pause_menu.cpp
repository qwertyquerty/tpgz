#include "menus/menu_pause/include/pause_menu.h"
#include <cstdio>
#include "d/d_com_inf_game.h"
#include "tpgz_utils.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

#define MAX_SCENT_OPT 6

KEEP_VAR PauseData* pauseData;

static Line lines[PAUSE_INDEX_COUNT] = {
    {"equipment", EQUIPMENT_INDEX, "Equipment", false, NULL},
                {"golden bugs", GOLDEN_BUG_INDEX, "Golden Bugs", false, NULL},
                {"hidden skills", HIDDEN_SKILL_INDEX, "Hidden Skills", false, NULL},
                {"scent:", SCENT_INDEX, "Current Scent", false, NULL}
};

KEEP_FUNC PauseMenu::PauseMenu(Cursor& cursor)
    : Menu(cursor) {}

PauseMenu::~PauseMenu() {}

void PauseMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);

    switch (dComIfGs_getCollectSmell()) {
    case 180:
        pauseData->l_scent_idx = 1;
        break;
    case 176:
        pauseData->l_scent_idx = 2;
        break;
    case 178:
        pauseData->l_scent_idx = 3;
        break;
    case 179:
        pauseData->l_scent_idx = 4;
        break;
    case 181:
        pauseData->l_scent_idx = 5;
        break;
    default:
        pauseData->l_scent_idx = 0;

    }

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    ListMember scent_opt[6] = {"none", "youths' scent", "scent of ilia", "poe scent", "reekfish scent", "medicine scent"};

    switch (cursor.y) {
    case SCENT_INDEX:
        cursor.x = pauseData->l_scent_idx;
        cursor.move(MAX_SCENT_OPT, MENU_LINE_NUM);

        if (cursor.y == SCENT_INDEX) {
            pauseData->l_scent_idx = cursor.x;
        }
        break;
    default:
        cursor.move(0, MENU_LINE_NUM);
        break;
    }

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        switch (cursor.y) {
        case EQUIPMENT_INDEX:
            g_menuMgr->push(MN_EQUIPMENT_INDEX);
            return;
        case GOLDEN_BUG_INDEX:
            g_menuMgr->push(MN_GOLDEN_BUGS_INDEX);
            return;
        case HIDDEN_SKILL_INDEX:
            g_menuMgr->push(MN_HIDDEN_SKILLS_INDEX);
        }
    }

    switch (pauseData->l_scent_idx) {
    case 1:
        dComIfGs_setCollectSmell(180);
        break;
    case 2:
        dComIfGs_setCollectSmell(176);
        break;
    case 3:
        dComIfGs_setCollectSmell(178);
        break;
    case 4:
        dComIfGs_setCollectSmell(179);
        break;
    case 5:
        dComIfGs_setCollectSmell(181);
        break;
    default:
        dComIfGs_setCollectSmell(255);
    }

    lines[SCENT_INDEX].printf(" <%s>", scent_opt[pauseData->l_scent_idx].member);

    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
