#include "menus/menu_rupee_flags/include/rupee_flags_menu.h"
#include "gz_flags.h"
#include "d/d_com_inf_game.h"
#include "tpgz_utils.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

KEEP_VAR RupeeFlagsData* rupeeFlagsData;

static bool fundraising1Active() {
    return rupeeFlagsData->l_fundraising1;
}

static bool fundraising2Active() {
    return rupeeFlagsData->l_fundraising2;
}

static bool rupeeCsFlagActive() {
    return rupeeFlagsData->l_rupeeFlag;
}

static Line lines[RUPEE_FLAGS_COUNT] = {
    {"donation amount:", DONATION_AMT_INDEX, "Sets the amount of rupees donated to Charlo"},
    {"fundraising amount:", FUNDRAISING_AMT_INDEX, "Sets the current fundraising amount"},
    {"fundraising 1", FUNDRAISING_1_INDEX, "Toggle flag for first fundraising being complete", true,
     fundraising1Active},
    {"fundraising 2", FUNDRAISING_2_INDEX, "Toggle flag for second fundraising being complete", true,
     fundraising2Active},
    {"rupee cutscenes", RUPEE_CS_FLAG_INDEX, "Toggle rupee cutscenes being enabled", true,
     rupeeCsFlagActive},
};

KEEP_FUNC RupeeFlagsMenu::RupeeFlagsMenu(Cursor& cursor)
    : Menu(cursor) {}

RupeeFlagsMenu::~RupeeFlagsMenu() {}

void RupeeFlagsMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);

    if (!rupeeFlagsData) {
        return;
    }

    // update flags
    rupeeFlagsData->l_fundraising1 = dComIfGs_isEventBit(0x2e20);
    rupeeFlagsData->l_fundraising2 = dComIfGs_isEventBit(0x0f10);

    // update donation amount
    u8 donation_high_bits = dComIfGs_getEventReg(0xf7ff);
    u8 donation_low_bits = dComIfGs_getEventReg(0xf8ff);

    rupeeFlagsData->l_donationAmount = donation_high_bits << 8 | donation_low_bits;

    // update fundraising amount
    u8 fund_high_bits = dComIfGs_getEventReg(0xf9ff);
    u8 fund_low_bits = dComIfGs_getEventReg(0xfaff);

    rupeeFlagsData->l_fundraisingAmount = fund_high_bits << 8 | fund_low_bits;

    for (int i = dItemNo_BLUE_RUPEE_e; i <= dItemNo_SILVER_RUPEE_e; i++) {
        if (dComIfGs_isItemFirstBit(i)) {
            rupeeFlagsData->l_rupeeFlag = true;
            break;
        }
    }

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    switch (cursor.y) {
    case DONATION_AMT_INDEX:
        Cursor::moveList(rupeeFlagsData->l_donationAmount);

        if (rupeeFlagsData->l_donationAmount > 1000)
            rupeeFlagsData->l_donationAmount = 1000;

        dComIfGs_setEventReg(0xf7ff, (rupeeFlagsData->l_donationAmount >> 8) & 0xFF);
        dComIfGs_setEventReg(0xf8ff, rupeeFlagsData->l_donationAmount & 0xFF);
        break;
    case FUNDRAISING_AMT_INDEX:
        Cursor::moveList(rupeeFlagsData->l_fundraisingAmount);

        if (rupeeFlagsData->l_fundraisingAmount > 2000)
            rupeeFlagsData->l_fundraisingAmount = 2000;

        dComIfGs_setEventReg(0xf9ff, (rupeeFlagsData->l_fundraisingAmount >> 8) & 0xFF);
        dComIfGs_setEventReg(0xfaff, rupeeFlagsData->l_fundraisingAmount & 0xFF);
        break;
    }

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        switch (cursor.y) {
        case FUNDRAISING_1_INDEX:
            setEventFlag(0x2e20);
            break;
        case FUNDRAISING_2_INDEX:
            setEventFlag(0x0f10);
            break;
        case RUPEE_CS_FLAG_INDEX:
            if (rupeeFlagsData->l_rupeeFlag) {
                for (int i = dItemNo_BLUE_RUPEE_e; i <= dItemNo_SILVER_RUPEE_e; i++) {
                    dComIfGs_offItemFirstBit(i);
                }
                rupeeFlagsData->l_rupeeFlag = false;
            } else {
                for (int i = dItemNo_BLUE_RUPEE_e; i <= dItemNo_SILVER_RUPEE_e; i++) {
                    dComIfGs_onItemFirstBit(i);
                }
            }
            break;
        }
    }

    lines[DONATION_AMT_INDEX].printf(" <%d>", rupeeFlagsData->l_donationAmount);
    lines[FUNDRAISING_AMT_INDEX].printf(" <%d>", rupeeFlagsData->l_fundraisingAmount);

    cursor.move(0, MENU_LINE_NUM);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
