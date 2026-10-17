#include "menus/menu_hidden_skills/include/hidden_skills_menu.h"
#include <cstdio>
#include "d/d_com_inf_game.h"
#include "tpgz_utils.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

KEEP_VAR HiddenSkillsData* hiddenSkillsData;

static bool endingBlowActive() {
    return hiddenSkillsData->l_ebFlag;
}

static bool shieldBashActive() {
    return hiddenSkillsData->l_sbFlag;
}

static bool backsliceActive() {
    return hiddenSkillsData->l_bsFlag;
}

static bool helmSplitterActive() {
    return hiddenSkillsData->l_hsFlag;
}

static bool mortalDrawActive() {
    return hiddenSkillsData->l_mdFlag;
}

static bool jumpStrikeActive() {
    return hiddenSkillsData->l_jsFlag;
}

static bool greatSpinActive() {
    return hiddenSkillsData->l_gsFlag;
}

static Line lines[HIDDEN_SKILLS_INDEX_COUNT] = {
    {"ending blow:", ENDING_BLOW_INDEX, "Ending Blow", true, endingBlowActive},
                {"shield bash:", SHIELD_BASH_INDEX, "Shield Bash", true, shieldBashActive},
                {"backslice:", BACKSLICE_INDEX, "Backslice", true, backsliceActive},
                {"helm splitter:", HELM_SPLITTER_INDEX, "Helm Splitter", true, helmSplitterActive},
                {"mortal draw:", MORTAL_DRAW_INDEX, "Mortal Draw", true, mortalDrawActive},
                {"jump strike:", JUMP_STRIKE_INDEX, "Jump Strike", true, jumpStrikeActive},
                {"greatspin:", GREAT_SPIN_INDEX, "Greatspin", true, greatSpinActive}
};

KEEP_FUNC HiddenSkillsMenu::HiddenSkillsMenu(Cursor& cursor)
    : Menu(cursor) {}

HiddenSkillsMenu::~HiddenSkillsMenu() {}

void HiddenSkillsMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    // update hidden skill flags
    hiddenSkillsData->l_ebFlag = dComIfGs_isEventBit(0x2904);
    hiddenSkillsData->l_sbFlag = dComIfGs_isEventBit(0x2908);
    hiddenSkillsData->l_bsFlag = dComIfGs_isEventBit(0x2902);
    hiddenSkillsData->l_hsFlag = dComIfGs_isEventBit(0x2901);
    hiddenSkillsData->l_mdFlag = dComIfGs_isEventBit(0x2A80);
    hiddenSkillsData->l_jsFlag = dComIfGs_isEventBit(0x2A40);
    hiddenSkillsData->l_gsFlag = dComIfGs_isEventBit(0x2A20);

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        switch (cursor.y) {
        case ENDING_BLOW_INDEX:
            setEventFlag(0x2904);
            break;
        case SHIELD_BASH_INDEX:
            setEventFlag(0x2908);
            break;
        case BACKSLICE_INDEX:
            setEventFlag(0x2902);
            break;
        case HELM_SPLITTER_INDEX:
            setEventFlag(0x2901);
            break;
        case MORTAL_DRAW_INDEX:
            setEventFlag(0x2A80);
            break;
        case JUMP_STRIKE_INDEX:
            setEventFlag(0x2A40);
            break;
        case GREAT_SPIN_INDEX:
            setEventFlag(0x2A20);
            break;
        }
    }

    cursor.move(0, MENU_LINE_NUM);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
