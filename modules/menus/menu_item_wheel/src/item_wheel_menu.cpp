
#include "menus/menu_item_wheel/include/item_wheel_menu.h"
#include <cstdio>
#include "font.h"
#include "d/d_com_inf_game.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

#define ITEM_WHEEL_SLOTS 24
#define MAX_ITEMS ARRAY_COUNT(l_lookupTbl)

#ifdef GCN_PLATFORM
#define DEFAULT_BTN_TXT "Z"
#define DEFAULT_BTN Z
#define RESET_BTN_TXT "X"
#define RESET_BUTTON X
#endif

#ifdef WII_PLATFORM
#define DEFAULT_BTN_TXT "+"
#define DEFAULT_BTN PLUS
#define RESET_BTN_TXT "-"
#define RESET_BUTTON MINUS
#endif

static Line lines[ITEM_WHEEL_SLOTS] = {
    {"Slot 0:", SLOT_0, "", false, NULL, false},
    {"Slot 1:", SLOT_1, "", false, NULL, false},
    {"Slot 2:", SLOT_2, "", false, NULL, false},
    {"Slot 3:", SLOT_3, "", false, NULL, false},
    {"Slot 4:", SLOT_4, "", false, NULL, false},
    {"Slot 5:", SLOT_5, "", false, NULL, false},
    {"Slot 6:", SLOT_6, "", false, NULL, false},
    {"Slot 7:", SLOT_7, "", false, NULL, false},
    {"Slot 8:", SLOT_8, "", false, NULL, false},
    {"Slot 9:", SLOT_9, "", false, NULL, false},
    {"Slot 10:", SLOT_10, "", false, NULL, false},
    {"Slot 11:", SLOT_11, "", false, NULL, false},
    {"Slot 12:", SLOT_12, "", false, NULL, false},
    {"Slot 13:", SLOT_13, "", false, NULL, false},
    {"Slot 14:", SLOT_14, "", false, NULL, false},
    {"Slot 15:", SLOT_15, "", false, NULL, false},
    {"Slot 16:", SLOT_16, "", false, NULL, false},
    {"Slot 17:", SLOT_17, "", false, NULL, false},
    {"Slot 18:", SLOT_18, "", false, NULL, false},
    {"Slot 19:", SLOT_19, "", false, NULL, false},
    {"Slot 20:", SLOT_20, "", false, NULL, false},
    {"Slot 21:", SLOT_21, "", false, NULL, false},
    {"Slot 22:", SLOT_22, "", false, NULL, false},
    {"Slot 23:", SLOT_23, "", false, NULL, false},
};

KEEP_FUNC ItemWheelMenu::ItemWheelMenu(Cursor& cursor, ItemWheelData& data)
    : Menu(cursor), l_listIdx(data.l_listIdx) {}

ItemWheelMenu::~ItemWheelMenu() {}

const uint8_t l_validItems[] = {
    dItemNo_NONE_e,            dItemNo_ANCIENT_DOCUMENT_e,
    dItemNo_ANCIENT_DOCUMENT2_e,  dItemNo_AIR_LETTER_e,
    dItemNo_ASHS_SCRIBBLING_e,    dItemNo_RAFRELS_MEMO_e,
    dItemNo_IRONBALL_e,           dItemNo_BEE_CHILD_e,
    dItemNo_CHUCHU_BLUE_e,        dItemNo_BLUE_BOTTLE_e,
    dItemNo_POKE_BOMB_e,          dItemNo_HOOKSHOT_e,
    dItemNo_COPY_ROD_e,           dItemNo_W_HOOKSHOT_e,
    dItemNo_BOMB_BAG_LV1_e,       dItemNo_EMPTY_BOTTLE_e,
    dItemNo_FAIRY_e,              dItemNo_FAIRY_DROP_e,
    dItemNo_FISHING_ROD_1_e,      dItemNo_BEE_ROD_e,
    dItemNo_JEWEL_ROD_e,          dItemNo_JEWEL_BEE_ROD_e,
    dItemNo_JEWEL_WORM_ROD_e,     dItemNo_WORM_ROD_e,
    dItemNo_BOOMERANG_e,          dItemNo_LV2_SOUP_e,
    dItemNo_CHUCHU_GREEN_e,       dItemNo_HALF_MILK_BOTTLE_e,
    dItemNo_HAWK_EYE_e,           dItemNo_BOW_e,
    dItemNo_HORSE_FLUTE_e,        dItemNo_HOT_SPRING_e,
    dItemNo_IRIAS_PENDANT_e,      dItemNo_BILL_e,
    dItemNo_HVY_BOOTS_e,          dItemNo_KANTERA_e,
    dItemNo_OIL_BOTTLE_e,         dItemNo_GREEN_BOTTLE_e,
    dItemNo_MILK_BOTTLE_e,        dItemNo_UGLY_SOUP_e,
    dItemNo_DUNGEON_BACK_e,       dItemNo_DUNGEON_EXIT_e,
    dItemNo_TKS_LETTER_e,         dItemNo_CHUCHU_PURPLE_e,
    dItemNo_CHUCHU_RARE_e,        dItemNo_CHUCHU_RED_e,
    dItemNo_RED_BOTTLE_e,         dItemNo_NORMAL_BOMB_e,
    dItemNo_LETTER_e,             dItemNo_LV1_SOUP_e,
    dItemNo_PACHINKO_e,           dItemNo_SPINNER_e,
    dItemNo_LV3_SOUP_e,           dItemNo_WATER_BOTTLE_e,
    dItemNo_WATER_BOMB_e,         dItemNo_WOOD_STATUE_e,
    dItemNo_WORM_e,               dItemNo_CHUCHU_YELLOW_e
};

const uint8_t l_defaultItems[ITEM_WHEEL_SLOTS] = {
    dItemNo_BOOMERANG_e,     dItemNo_KANTERA_e,      dItemNo_SPINNER_e,          dItemNo_HVY_BOOTS_e,    dItemNo_BOW_e,
    dItemNo_HAWK_EYE_e,      dItemNo_IRONBALL_e,     dItemNo_NONE_e,          dItemNo_COPY_ROD_e,     dItemNo_HOOKSHOT_e,
    dItemNo_W_HOOKSHOT_e,    dItemNo_EMPTY_BOTTLE_e, dItemNo_EMPTY_BOTTLE_e,     dItemNo_EMPTY_BOTTLE_e, dItemNo_EMPTY_BOTTLE_e,
    dItemNo_NORMAL_BOMB_e,   dItemNo_WATER_BOMB_e,   dItemNo_POKE_BOMB_e,        dItemNo_DUNGEON_EXIT_e, dItemNo_RAFRELS_MEMO_e,
    dItemNo_FISHING_ROD_1_e, dItemNo_HORSE_FLUTE_e,  dItemNo_ANCIENT_DOCUMENT_e, dItemNo_PACHINKO_e,
};

const ItemLookup l_lookupTbl[] = {
    {dItemNo_NONE_e, "n/a"},
    {dItemNo_ANCIENT_DOCUMENT_e, "ancient sky book (empty)"},
    {dItemNo_ANCIENT_DOCUMENT2_e, "ancient sky book (filled)"},
    {dItemNo_AIR_LETTER_e, "ancient sky book (partial)"},
    {dItemNo_ASHS_SCRIBBLING_e, "ashei's sketch"},
    {dItemNo_RAFRELS_MEMO_e, "auru's memo"},
    {dItemNo_IRONBALL_e, "ball and chain"},
    {dItemNo_BEE_CHILD_e, "bee larva"},
    {dItemNo_CHUCHU_BLUE_e, "blue chu"},
    {dItemNo_BLUE_BOTTLE_e, "blue potion"},
    {dItemNo_POKE_BOMB_e, "bomblings"},
    {dItemNo_HOOKSHOT_e, "clawshot"},
    {dItemNo_COPY_ROD_e, "dominion rod"},
    {dItemNo_W_HOOKSHOT_e, "double clawshot"},
    {dItemNo_BOMB_BAG_LV1_e, "empty bomb bag"},
    {dItemNo_EMPTY_BOTTLE_e, "empty bottle"},
    {dItemNo_FAIRY_e, "fairy"},
    {dItemNo_FAIRY_DROP_e, "fairy tears"},
    {dItemNo_FISHING_ROD_1_e, "fishing rod"},
    {dItemNo_BEE_ROD_e, "fishing rod (bee larva)"},
    {dItemNo_JEWEL_ROD_e, "fishing rod (coral earring)"},
    {dItemNo_JEWEL_BEE_ROD_e, "fishing rod (coral earring/bee larva)"},
    {dItemNo_JEWEL_WORM_ROD_e, "fishing rod (coral earring/worm)"},
    {dItemNo_WORM_ROD_e, "fishing rod (worm)"},
    {dItemNo_BOOMERANG_e, "gale boomerang"},
    {dItemNo_LV2_SOUP_e, "good soup"},
    {dItemNo_CHUCHU_GREEN_e, "green chu"},
    {dItemNo_HALF_MILK_BOTTLE_e, "half milk"},
    {dItemNo_HAWK_EYE_e, "hawkeye"},
    {dItemNo_BOW_e, "hero's bow"},
    {dItemNo_HORSE_FLUTE_e, "horse call"},
    {dItemNo_HOT_SPRING_e, "hot spring water"},
    {dItemNo_IRIAS_PENDANT_e, "Ilia's charm"},
    {dItemNo_BILL_e, "invoice"},
    {dItemNo_HVY_BOOTS_e, "iron boots"},
    {dItemNo_KANTERA_e, "lantern"},
    {dItemNo_OIL_BOTTLE_e, "lantern oil"},
    {dItemNo_GREEN_BOTTLE_e, "magic potion"},
    {dItemNo_MILK_BOTTLE_e, "milk"},
    {dItemNo_UGLY_SOUP_e, "nasty soup"},
    {dItemNo_DUNGEON_BACK_e, "ooccoo Jr."},
    {dItemNo_DUNGEON_EXIT_e, "ooccoo Sr."},
    {dItemNo_TKS_LETTER_e, "ooccoo's note"},
    {dItemNo_CHUCHU_PURPLE_e, "purple chu"},
    {dItemNo_CHUCHU_RARE_e, "rare chu"},
    {dItemNo_CHUCHU_RED_e, "red chu"},
    {dItemNo_RED_BOTTLE_e, "red potion"},
    {dItemNo_NORMAL_BOMB_e, "regular bombs"},
    {dItemNo_LETTER_e, "renado's letter"},
    {dItemNo_LV1_SOUP_e, "simple soup"},
    {dItemNo_PACHINKO_e, "slingshot"},
    {dItemNo_SPINNER_e, "spinner"},
    {dItemNo_LV3_SOUP_e, "superb soup"},
    {dItemNo_WATER_BOTTLE_e, "water"},
    {dItemNo_WATER_BOMB_e, "water bombs"},
    {dItemNo_WOOD_STATUE_e, "wooden statue"},
    {dItemNo_WORM_e, "worm"},
    {dItemNo_CHUCHU_YELLOW_e, "yellow chu"}
};

void ItemWheelMenu::updateListIdx() {
    uint8_t item_id = (*dComIfGs_getSaveData()).getPlayer().getItem().getItem(cursor.y, false);
    for (unsigned int i = 0; i < MAX_ITEMS; i++) {
        if (item_id == l_validItems[i]) {
            l_listIdx = i;
        }
    }
}

void ItemWheelMenu::fixSpecialItems(int i) {
    if (l_validItems[i] == dItemNo_NORMAL_BOMB_e || l_validItems[i] == dItemNo_WATER_BOMB_e ||
        l_validItems[i] == dItemNo_POKE_BOMB_e) {
        switch (cursor.y) {
        case SLOT_15:
            dComIfGs_setBombNum(0, 1);
            break;
        case SLOT_16:
            dComIfGs_setBombNum(1, 1);
            break;
        case SLOT_17:
            dComIfGs_setBombNum(2, 1);
            break;
        }
    }
}

void ItemWheelMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);
    updateListIdx();

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    for (size_t slot_no = 0; slot_no < MENU_LINE_NUM; slot_no++) {
        int item_id = dComIfGs_getItem(slot_no, false);

        for (unsigned int j = 0; j < MAX_ITEMS; j++) {
            if (l_lookupTbl[j].item_id == item_id) {
                lines[slot_no].printf(" <%s>", item_id != dItemNo_NONE_e ? l_lookupTbl[j].name : "n/a");
            }

            if (l_lookupTbl[j].item_id == l_defaultItems[slot_no]) {
                snprintf(lines[slot_no].description, sizeof(lines[slot_no].description),
                         "Slot %d default: %s. " DEFAULT_BTN_TXT ": set default; " RESET_BTN_TXT ": reset.",
                         slot_no, l_lookupTbl[j].name);
            } else {
                continue;
            }
        }
    }

    if (GZ_getButtonRepeat(DPAD_RIGHT)) {
        l_listIdx++;

        if (l_listIdx > MAX_ITEMS) {
            l_listIdx = 0;
        }

        fixSpecialItems(l_listIdx);
        dComIfGs_setItem(cursor.y, l_validItems[l_listIdx]);
    }

    if (GZ_getButtonRepeat(DPAD_LEFT)) {
        l_listIdx--;

        if (l_listIdx < 0) {
            l_listIdx = MAX_ITEMS;
        }

        fixSpecialItems(l_listIdx);
        dComIfGs_setItem(cursor.y, l_validItems[l_listIdx]);
    }

    if (GZ_getButtonTrig(DEFAULT_BTN)) {
        dComIfGs_setItem(cursor.y, l_defaultItems[cursor.y]);
    }

    if (GZ_getButtonTrig(RESET_BUTTON)) {
        dComIfGs_setItem(cursor.y, dItemNo_NONE_e);
    }

    cursor.move(0, MENU_LINE_NUM);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
