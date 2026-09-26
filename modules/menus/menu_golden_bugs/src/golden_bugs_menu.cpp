#include "menus/menu_golden_bugs/include/golden_bugs_menu.h"
#include <cstdio>
#include "d/d_com_inf_game.h"
#include "tpgz_utils.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

KEEP_VAR GoldenBugData* gbData;

#define MAX_GB_OPT 3

static Line lines[GB_INDEX_COUNT] = {
    {"Ant (M):", M_ANT_INDEX, "Male Ant", false, NULL, MAX_GB_OPT},
    {"Ant (F):", F_ANT_INDEX, "Female Ant", false, NULL, MAX_GB_OPT},
    {"Beetle (M:", M_BEETLE_INDEX, "Male Beetle", false, NULL, MAX_GB_OPT},
    {"Beetle (F):", F_BEETLE_INDEX, "Female Beetle", false, NULL, MAX_GB_OPT},
    {"Butterfly (M):", M_BUTTERFLY_INDEX, "Male Butterfly", false, NULL, MAX_GB_OPT},
    {"Butterfly (F):", F_BUTTERFLY_INDEX, "Female Butterfly", false, NULL, MAX_GB_OPT},
    {"Dayfly (M):", M_DAYFLY_INDEX, "Male Dayfly", false, NULL, MAX_GB_OPT},
    {"Dayfly (F):", F_DAYFLY_INDEX, "Female Dayfly", false, NULL, MAX_GB_OPT},
    {"Dragonfly (M):", M_DRAGONFLY_INDEX, "Male Dragonfly", false, NULL, MAX_GB_OPT},
    {"Dragonfly (F):", F_DRAGONFLY_INDEX, "Female Dragonfly", false, NULL, MAX_GB_OPT},
    {"Grasshopper (M):", M_GRASSHOPPER_INDEX, "Male Grasshopper", false, NULL, MAX_GB_OPT},
    {"Grasshopper (F):", F_GRASSHOPPER_INDEX, "Female Grasshopper", false, NULL, MAX_GB_OPT},
    {"Ladybug (M):", M_LADYBUG_INDEX, "Male Ladybug", false, NULL, MAX_GB_OPT},
    {"Ladybug (F):", F_LADYBUG_INDEX, "Female Ladybug", false, NULL, MAX_GB_OPT},
    {"Mantis (M):", M_MANTIS_INDEX, "Male Mantis", false, NULL, MAX_GB_OPT},
    {"Mantis (F):", F_MANTIS_INDEX, "Female Mantis", false, NULL, MAX_GB_OPT},
    {"Phasmid (M):", M_PHASMID_INDEX, "Male Phasmid", false, NULL, MAX_GB_OPT},
    {"Phasmid (F):", F_PHASMID_INDEX, "Female Phasmid", false, NULL, MAX_GB_OPT},
    {"Pill Bug (M):", M_PILL_BUG_INDEX, "Male Pill Bug", false, NULL, MAX_GB_OPT},
    {"Pill Bug (F):", F_PILL_BUG_INDEX, "Female Pill Bug", false, NULL, MAX_GB_OPT},
    {"Snail (M):", M_SNAIL_INDEX, "Male Snail", false, NULL, MAX_GB_OPT},
    {"Snail (F):", F_SNAIL_INDEX, "Female Snail", false, NULL, MAX_GB_OPT},
    {"Stag Beetle (M):", M_STAG_BEETLE_INDEX, "Male Stag Beetle", false, NULL, MAX_GB_OPT},
    {"Stag Beetle (F):", F_STAG_BEETLE_INDEX, "Female Stag Beetle", false, NULL, MAX_GB_OPT},
};

KEEP_FUNC GoldenBugMenu::GoldenBugMenu(Cursor& cursor)
    : Menu(cursor) {

    }

GoldenBugMenu::~GoldenBugMenu() {}

void GoldenBugMenu::setBugs() {
    switch (gbData->l_mAntIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_ANT_e);
        dComIfGs_offEventBit(0x3301);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_ANT_e);
        dComIfGs_offEventBit(0x3301);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_ANT_e);
        dComIfGs_onEventBit(0x3301);
        break;
    }

    switch (gbData->l_fAntIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_ANT_e);
        dComIfGs_offEventBit(0x3480);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_ANT_e);
        dComIfGs_offEventBit(0x3480);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_ANT_e);
        dComIfGs_onEventBit(0x3480);
        break;
    }

    switch (gbData->l_mBeetleIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_BEETLE_e);
        dComIfGs_offEventBit(0x3110);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_BEETLE_e);
        dComIfGs_offEventBit(0x3110);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_BEETLE_e);
        dComIfGs_onEventBit(0x3110);
        break;
    }

    switch (gbData->l_fBeetleIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_BEETLE_e);
        dComIfGs_offEventBit(0x3108);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_BEETLE_e);
        dComIfGs_offEventBit(0x3108);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_BEETLE_e);
        dComIfGs_onEventBit(0x3108);
        break;
    }

    switch (gbData->l_mButterflyIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_BUTTERFLY_e);
        dComIfGs_offEventBit(0x3104);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_BUTTERFLY_e);
        dComIfGs_offEventBit(0x3104);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_BUTTERFLY_e);
        dComIfGs_onEventBit(0x3104);
        break;
    }

    switch (gbData->l_fButterflyIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_BUTTERFLY_e);
        dComIfGs_offEventBit(0x3102);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_BUTTERFLY_e);
        dComIfGs_offEventBit(0x3102);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_BUTTERFLY_e);
        dComIfGs_onEventBit(0x3102);
        break;
    }

    switch (gbData->l_mDayflyIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_MAYFLY_e);
        dComIfGs_offEventBit(0x3440);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_MAYFLY_e);
        dComIfGs_offEventBit(0x3440);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_MAYFLY_e);
        dComIfGs_onEventBit(0x3440);
        break;
    }

    switch (gbData->l_fDayflyIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_MAYFLY_e);
        dComIfGs_offEventBit(0x3420);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_MAYFLY_e);
        dComIfGs_offEventBit(0x3420);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_MAYFLY_e);
        dComIfGs_onEventBit(0x3420);
        break;
    }

    switch (gbData->l_mDragonflyIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_DRAGONFLY_e);
        dComIfGs_offEventBit(0x3304);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_DRAGONFLY_e);
        dComIfGs_offEventBit(0x3304);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_DRAGONFLY_e);
        dComIfGs_onEventBit(0x3304);
        break;
    }

    switch (gbData->l_fDragonflyIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_DRAGONFLY_e);
        dComIfGs_offEventBit(0x3302);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_DRAGONFLY_e);
        dComIfGs_offEventBit(0x3302);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_DRAGONFLY_e);
        dComIfGs_onEventBit(0x3302);
        break;
    }

    switch (gbData->l_mGrasshopperIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_GRASSHOPPER_e);
        dComIfGs_offEventBit(0x3240);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_GRASSHOPPER_e);
        dComIfGs_offEventBit(0x3240);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_GRASSHOPPER_e);
        dComIfGs_onEventBit(0x3240);
        break;
    }

    switch (gbData->l_fGrasshopperIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_GRASSHOPPER_e);
        dComIfGs_offEventBit(0x3220);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_GRASSHOPPER_e);
        dComIfGs_offEventBit(0x3220);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_GRASSHOPPER_e);
        dComIfGs_onEventBit(0x3220);
        break;
    }

    switch (gbData->l_mLadybugIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_LADYBUG_e);
        dComIfGs_offEventBit(0x3340);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_LADYBUG_e);
        dComIfGs_offEventBit(0x3340);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_LADYBUG_e);
        dComIfGs_onEventBit(0x3340);
        break;
    }

    switch (gbData->l_fLadybugIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_LADYBUG_e);
        dComIfGs_offEventBit(0x3320);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_LADYBUG_e);
        dComIfGs_offEventBit(0x3320);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_LADYBUG_e);
        dComIfGs_onEventBit(0x3320);
        break;
    }

    switch (gbData->l_mMantisIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_MANTIS_e);
        dComIfGs_offEventBit(0x3201);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_MANTIS_e);
        dComIfGs_offEventBit(0x3201);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_MANTIS_e);
        dComIfGs_onEventBit(0x3201);
        break;
    }

    switch (gbData->l_fMantisIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_MANTIS_e);
        dComIfGs_offEventBit(0x3380);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_MANTIS_e);
        dComIfGs_offEventBit(0x3380);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_MANTIS_e);
        dComIfGs_onEventBit(0x3380);
        break;
    }

    switch (gbData->l_mPhasmidIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_NANAFUSHI_e);
        dComIfGs_offEventBit(0x3210);
        break;  
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_NANAFUSHI_e);
        dComIfGs_offEventBit(0x3210);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_NANAFUSHI_e);
        dComIfGs_onEventBit(0x3210);
        break;
    }

    switch (gbData->l_fPhasmidIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_NANAFUSHI_e);
        dComIfGs_offEventBit(0x3208);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_NANAFUSHI_e);
        dComIfGs_offEventBit(0x3208);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_NANAFUSHI_e);
        dComIfGs_onEventBit(0x3208);
        break;
    }

    switch (gbData->l_mPillBugIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_DANGOMUSHI_e);
        dComIfGs_offEventBit(0x3204);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_DANGOMUSHI_e);
        dComIfGs_offEventBit(0x3204);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_DANGOMUSHI_e);
        dComIfGs_onEventBit(0x3204);
        break;
    }

    switch (gbData->l_fPillBugIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_DANGOMUSHI_e);
        dComIfGs_offEventBit(0x3202);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_DANGOMUSHI_e);
        dComIfGs_offEventBit(0x3202);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_DANGOMUSHI_e);
        dComIfGs_onEventBit(0x3202);
        break;
    }

    switch (gbData->l_mSnailIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_SNAIL_e);
        dComIfGs_offEventBit(0x3310);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_SNAIL_e);
        dComIfGs_offEventBit(0x3310);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_SNAIL_e);
        dComIfGs_onEventBit(0x3310);
        break;
    }

    switch (gbData->l_fSnailIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_SNAIL_e);
        dComIfGs_offEventBit(0x3308);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_SNAIL_e);
        dComIfGs_offEventBit(0x3308);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_SNAIL_e);
        dComIfGs_onEventBit(0x3308);
        break;
    }

    switch (gbData->l_mStagBeetleIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_M_STAG_BEETLE_e);
        dComIfGs_offEventBit(0x3101);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_M_STAG_BEETLE_e);
        dComIfGs_offEventBit(0x3101);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_M_STAG_BEETLE_e);
        dComIfGs_onEventBit(0x3101);
        break;
    }

    switch (gbData->l_fStagBeetleIdx) {
    case 0:
        dComIfGs_offItemFirstBit(dItemNo_F_STAG_BEETLE_e);
        dComIfGs_offEventBit(0x3280);
        break;
    case 1:
        dComIfGs_onItemFirstBit(dItemNo_F_STAG_BEETLE_e);
        dComIfGs_offEventBit(0x3280);
        break;
    case 2:
        dComIfGs_onItemFirstBit(dItemNo_F_STAG_BEETLE_e);
        dComIfGs_onEventBit(0x3280);
        break;
    }
}

u8 GoldenBugMenu::getBugIdx(u8 bug, u16 flag) {
    if (dComIfGs_isEventBit(flag)) {
        return 2;
    }

    if (dComIfGs_isItemFirstBit(bug)) {
        return 1;
    }

    return 0;
}

void GoldenBugMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    // update gb flags
    gbData->l_mAntIdx = getBugIdx(dItemNo_M_ANT_e,0x3301);
    gbData->l_fAntIdx = getBugIdx(dItemNo_F_ANT_e,0x3480);
    gbData->l_mBeetleIdx = getBugIdx(dItemNo_M_BEETLE_e,0x3110);
    gbData->l_fBeetleIdx = getBugIdx(dItemNo_F_BEETLE_e,0x3108);
    gbData->l_mButterflyIdx = getBugIdx(dItemNo_M_BUTTERFLY_e,0x3104);
    gbData->l_fButterflyIdx = getBugIdx(dItemNo_F_BUTTERFLY_e,0x3102);
    gbData->l_mDayflyIdx = getBugIdx(dItemNo_M_MAYFLY_e,0x3440);
    gbData->l_fDayflyIdx = getBugIdx(dItemNo_F_MAYFLY_e,0x3420);
    gbData->l_mDragonflyIdx = getBugIdx(dItemNo_M_DRAGONFLY_e,0x3304);
    gbData->l_fDragonflyIdx = getBugIdx(dItemNo_F_DRAGONFLY_e,0x3302);
    gbData->l_mGrasshopperIdx = getBugIdx(dItemNo_M_GRASSHOPPER_e,0x3240);
    gbData->l_fGrasshopperIdx = getBugIdx(dItemNo_F_GRASSHOPPER_e,0x3220);
    gbData->l_mLadybugIdx = getBugIdx(dItemNo_M_LADYBUG_e,0x3340);
    gbData->l_fLadybugIdx = getBugIdx(dItemNo_F_LADYBUG_e,0x3320);
    gbData->l_mMantisIdx = getBugIdx(dItemNo_M_MANTIS_e,0x3201);
    gbData->l_fMantisIdx = getBugIdx(dItemNo_F_MANTIS_e,0x3380);
    gbData->l_mPhasmidIdx = getBugIdx(dItemNo_M_NANAFUSHI_e,0x3210);
    gbData->l_fPhasmidIdx = getBugIdx(dItemNo_F_NANAFUSHI_e,0x3208);
    gbData->l_mPillBugIdx = getBugIdx(dItemNo_M_DANGOMUSHI_e,0x3204);
    gbData->l_fPillBugIdx = getBugIdx(dItemNo_F_DANGOMUSHI_e,0x3202);
    gbData->l_mSnailIdx = getBugIdx(dItemNo_M_SNAIL_e,0x3310);
    gbData->l_fSnailIdx = getBugIdx(dItemNo_F_SNAIL_e,0x3308);
    gbData->l_mStagBeetleIdx = getBugIdx(dItemNo_M_STAG_BEETLE_e,0x3101);
    gbData->l_fStagBeetleIdx = getBugIdx(dItemNo_F_STAG_BEETLE_e,0x3280);

    ListMember mAnt_opt[3] = {"none", "Ant (M)", "Ant (M) (turned in)"};
    ListMember fAnt_opt[3] = {"none", "Ant (F)", "Ant (F) (turned in)"};
    ListMember mBeetle_opt[3] = {"none", "Beetle (M)", "Beetle (M) (turned in)"};
    ListMember fBeetle_opt[3] = {"none", "Beetle (F)", "Beetle (F) (turned in)"};
    ListMember mButterfly_opt[3] = {"none", "Butterfly (M)", "Butterfly (M) (turned in)"};
    ListMember fButterfly_opt[3] = {"none", "Butterfly (F)", "Butterfly (F) (turned in)"};
    ListMember mDayfly_opt[3] = {"none", "Dayfly (M)", "Dayfly (M) (turned in)"};
    ListMember fDayfly_opt[3] = {"none", "Dayfly (F)", "Dayfly (F) (turned in)"};
    ListMember mDragonfly_opt[3] = {"none", "Dragonfly (M)", "Dragonfly (M) (turned in)"};
    ListMember fDragonfly_opt[3] = {"none", "Dragonfly (F)", "Dragonfly (F) (turned in)"};
    ListMember mGrasshopper_opt[3] = {"none", "Grasshopper (M)", "Grasshopper (M) (turned in)"};
    ListMember fGrasshopper_opt[3] = {"none", "Grasshopper (F)", "Grasshopper (F) (turned in)"};
    ListMember mLadybug_opt[3] = {"none", "Ladybug (M)", "Ladybug (M) (turned in)"};
    ListMember fLadybug_opt[3] = {"none", "Ladybug (F)", "Ladybug (F) (turned in)"};
    ListMember mMantis_opt[3] = {"none", "Mantis (M)", "Mantis (M) (turned in)"};
    ListMember fMantis_opt[3] = {"none", "Mantis (F)", "Mantis (F) (turned in)"};
    ListMember mPhasmid_opt[3] = {"none", "Phasmid (M)", "Phasmid (M) (turned in)"};
    ListMember fPhasmid_opt[3] = {"none", "Phasmid (F)", "Phasmid (F) (turned in)"};
    ListMember mPillBug_opt[3] = {"none", "Pill Bug (M)", "Pill Bug (M) (turned in)"};
    ListMember fPillBug_opt[3] = {"none", "Pill Bug (F)", "Pill Bug (F) (turned in)"};
    ListMember mSnail_opt[3] = {"none", "Snail (M)", "Snail (M) (turned in)"};
    ListMember fSnail_opt[3] = {"none", "Snail (F)", "Snail (F) (turned in)"};
    ListMember mStagBeetle_opt[3] = {"none", "Stag Beetle (M)", "Stag Beetle (M) (turned in)"};
    ListMember fStagBeetle_opt[3] = {"none", "Stag Beetle (F)", "Stag Beetle (F) (turned in)"};

    switch (cursor.y) {
    case M_ANT_INDEX:
        cursor.x = gbData->l_mAntIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_ANT_INDEX) {
            gbData->l_mAntIdx = cursor.x;
        }

        break;
    case F_ANT_INDEX:
        cursor.x = gbData->l_fAntIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_ANT_INDEX) {
            gbData->l_fAntIdx = cursor.x;
        }

        break;
    case M_BEETLE_INDEX:
        cursor.x = gbData->l_mBeetleIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_BEETLE_INDEX) {
            gbData->l_mBeetleIdx = cursor.x;
        }

        break;
    case F_BEETLE_INDEX:
        cursor.x = gbData->l_fBeetleIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_BEETLE_INDEX) {
            gbData->l_fBeetleIdx = cursor.x;
        }

        break;
    case M_BUTTERFLY_INDEX:
        cursor.x = gbData->l_mButterflyIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_BUTTERFLY_INDEX) {
            gbData->l_mButterflyIdx = cursor.x;
        }

        break;
    case F_BUTTERFLY_INDEX:
        cursor.x = gbData->l_fButterflyIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_BUTTERFLY_INDEX) {
            gbData->l_fButterflyIdx = cursor.x;
        }

        break;
    case M_DAYFLY_INDEX:
        cursor.x = gbData->l_mDayflyIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_DAYFLY_INDEX) {
            gbData->l_mDayflyIdx = cursor.x;
        }

        break;
    case F_DAYFLY_INDEX:
        cursor.x = gbData->l_fDayflyIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_DAYFLY_INDEX) {
            gbData->l_fDayflyIdx = cursor.x;
        }

        break;
    case M_DRAGONFLY_INDEX:
        cursor.x = gbData->l_mDragonflyIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_DRAGONFLY_INDEX) {
            gbData->l_mDragonflyIdx = cursor.x;
        }

        break;
    case F_DRAGONFLY_INDEX:
        cursor.x = gbData->l_fDragonflyIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_DRAGONFLY_INDEX) {
            gbData->l_fDragonflyIdx = cursor.x;
        }

        break;
    case M_GRASSHOPPER_INDEX:
        cursor.x = gbData->l_mGrasshopperIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_GRASSHOPPER_INDEX) {
            gbData->l_mGrasshopperIdx = cursor.x;
        }

        break;
    case F_GRASSHOPPER_INDEX:
        cursor.x = gbData->l_fGrasshopperIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_GRASSHOPPER_INDEX) {
            gbData->l_fGrasshopperIdx = cursor.x;
        }

        break;
    case M_LADYBUG_INDEX:
        cursor.x = gbData->l_mLadybugIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_LADYBUG_INDEX) {
            gbData->l_mLadybugIdx = cursor.x;
        }

        break;
    case F_LADYBUG_INDEX:
        cursor.x = gbData->l_fLadybugIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_LADYBUG_INDEX) {
            gbData->l_fLadybugIdx = cursor.x;
        }

        break;
    case M_MANTIS_INDEX:
        cursor.x = gbData->l_mMantisIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_MANTIS_INDEX) {
            gbData->l_mMantisIdx = cursor.x;
        }

        break;
    case F_MANTIS_INDEX:
        cursor.x = gbData->l_fMantisIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_MANTIS_INDEX) {
            gbData->l_fMantisIdx = cursor.x;
        }

        break;
    case M_PHASMID_INDEX:
        cursor.x = gbData->l_mPhasmidIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_PHASMID_INDEX) {
            gbData->l_mPhasmidIdx = cursor.x;
        }

        break;
    case F_PHASMID_INDEX:
        cursor.x = gbData->l_fPhasmidIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_PHASMID_INDEX) {
            gbData->l_fPhasmidIdx = cursor.x;
        }

        break;
    case M_PILL_BUG_INDEX:
        cursor.x = gbData->l_mPillBugIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_PILL_BUG_INDEX) {
            gbData->l_mPillBugIdx = cursor.x;
        }

        break;
    case F_PILL_BUG_INDEX:
        cursor.x = gbData->l_fPillBugIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_PILL_BUG_INDEX) {
            gbData->l_fPillBugIdx = cursor.x;
        }

        break;
    case M_SNAIL_INDEX:
        cursor.x = gbData->l_mSnailIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_SNAIL_INDEX) {
            gbData->l_mSnailIdx = cursor.x;
        }

        break;
    case F_SNAIL_INDEX:
        cursor.x = gbData->l_fSnailIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_SNAIL_INDEX) {
            gbData->l_fSnailIdx = cursor.x;
        }

        break;
    case M_STAG_BEETLE_INDEX:
        cursor.x = gbData->l_mStagBeetleIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == M_STAG_BEETLE_INDEX) {
            gbData->l_mStagBeetleIdx = cursor.x;
        }

        break;
    case F_STAG_BEETLE_INDEX:
        cursor.x = gbData->l_fStagBeetleIdx;
        cursor.move(MAX_GB_OPT, MENU_LINE_NUM);

        if (cursor.y == F_STAG_BEETLE_INDEX) {
            gbData->l_fStagBeetleIdx = cursor.x;
        }

        break;
    default:
        cursor.move(0, MENU_LINE_NUM);
        break;
    }

    setBugs();

    lines[M_ANT_INDEX].printf(" <%s>", mAnt_opt[gbData->l_mAntIdx].member);
    lines[F_ANT_INDEX].printf(" <%s>", fAnt_opt[gbData->l_fAntIdx].member);
    lines[M_BEETLE_INDEX].printf(" <%s>", mBeetle_opt[gbData->l_mBeetleIdx].member);
    lines[F_BEETLE_INDEX].printf(" <%s>", fBeetle_opt[gbData->l_fBeetleIdx].member);
    lines[M_BUTTERFLY_INDEX].printf(" <%s>", mButterfly_opt[gbData->l_mButterflyIdx].member);
    lines[F_BUTTERFLY_INDEX].printf(" <%s>", fButterfly_opt[gbData->l_fButterflyIdx].member);
    lines[M_DAYFLY_INDEX].printf(" <%s>", mDayfly_opt[gbData->l_mDayflyIdx].member);
    lines[F_DAYFLY_INDEX].printf(" <%s>", fDayfly_opt[gbData->l_fDayflyIdx].member);
    lines[M_DRAGONFLY_INDEX].printf(" <%s>", mDragonfly_opt[gbData->l_mDragonflyIdx].member);
    lines[F_DRAGONFLY_INDEX].printf(" <%s>", fDragonfly_opt[gbData->l_fDragonflyIdx].member);
    lines[M_GRASSHOPPER_INDEX].printf(" <%s>", mGrasshopper_opt[gbData->l_mGrasshopperIdx].member);
    lines[F_GRASSHOPPER_INDEX].printf(" <%s>", fGrasshopper_opt[gbData->l_fGrasshopperIdx].member);
    lines[M_LADYBUG_INDEX].printf(" <%s>", mLadybug_opt[gbData->l_mLadybugIdx].member);
    lines[F_LADYBUG_INDEX].printf(" <%s>", fLadybug_opt[gbData->l_fLadybugIdx].member);
    lines[M_MANTIS_INDEX].printf(" <%s>", mMantis_opt[gbData->l_mMantisIdx].member);
    lines[F_MANTIS_INDEX].printf(" <%s>", fMantis_opt[gbData->l_fMantisIdx].member);
    lines[M_PHASMID_INDEX].printf(" <%s>", mPhasmid_opt[gbData->l_mPhasmidIdx].member);
    lines[F_PHASMID_INDEX].printf(" <%s>", fPhasmid_opt[gbData->l_fPhasmidIdx].member);
    lines[M_PILL_BUG_INDEX].printf(" <%s>", mPillBug_opt[gbData->l_mPillBugIdx].member);
    lines[F_PILL_BUG_INDEX].printf(" <%s>", fPillBug_opt[gbData->l_fPillBugIdx].member);
    lines[M_SNAIL_INDEX].printf(" <%s>", mSnail_opt[gbData->l_mSnailIdx].member);
    lines[F_SNAIL_INDEX].printf(" <%s>", fSnail_opt[gbData->l_fSnailIdx].member);
    lines[M_STAG_BEETLE_INDEX].printf(" <%s>", mStagBeetle_opt[gbData->l_mStagBeetleIdx].member);
    lines[F_STAG_BEETLE_INDEX].printf(" <%s>", fStagBeetle_opt[gbData->l_fStagBeetleIdx].member);


    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}