#include "menus/menu_portal_flags/include/portal_flags_menu.h"
#include <cstdio>
#include "gz_flags.h"
#include "m_Do/m_Do_printf.h"
#include "d/d_com_inf_game.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

#define MAX_REGION_OPTIONS 6
enum SaveArea { ORDON = 0, FARON = 2, ELDIN = 3, LANAYRU = 4, FIELD = 6, GROVE = 7, SNOWPEAK = 8, DESERT = 10 };

KEEP_VAR PortalFlagsData* portalFlagsData;

static bool regionFlagActive() {
    return portalFlagsData->l_mapRegion;
}

static bool springWarpActive() {
    return portalFlagsData->l_springWarp;
}

static bool sFaronWarpActive() {
    return portalFlagsData->l_sfaronWarp;
}

static bool nFaronWarpActive() {
    return portalFlagsData->l_nfaronWarp;
}

static bool groveWarpActive() {
    return portalFlagsData->l_groveWarp;
}

static bool gorgeWarpActive() {
    return portalFlagsData->l_gorgeWarp;
}

static bool kakarikoWarpActive() {
    return portalFlagsData->l_kakWarp;
}

static bool mountainWarpActive() {
    return portalFlagsData->l_mountainWarp;
}

static bool bridgeWarpActive() {
    return portalFlagsData->l_bridgeWarp;
}

static bool townWarpActive() {
    return portalFlagsData->l_ctWarp;
}

static bool lakeWarpActive() {
    return portalFlagsData->l_lakeWarp;
}

static bool domainWarpActive() {
    return portalFlagsData->l_domainWarp;
}

static bool uzrWarpActive() {
    return portalFlagsData->l_uzrWarp;
}

static bool snowpeakWarpActive() {
    return portalFlagsData->l_snowpeakWarp;
}

static bool mesaWarpActive() {
    return portalFlagsData->l_mesaWarp;
}

static bool mirrorWarpActive() {
    return portalFlagsData->l_mirrorWarp;
}

static Line lines[17] = {
    {"region:", SELECT_REGION_INDEX, "Select region flag", false, NULL,
     MAX_REGION_OPTIONS},
    {"region unlocked", REGION_FLAG_INDEX, "Unlock selected map region", true, regionFlagActive},
    {"ordon spring", SPRING_WARP_INDEX, "Ordon Spring warp portal", true, springWarpActive},
    {"south faron", S_FARON_WARP_INDEX, "South Faron warp portal", true, sFaronWarpActive},
    {"north faron", N_FARON_WARP_INDEX, "North Faron warp portal", true, nFaronWarpActive},
    {"sacred grove", GROVE_WARP_INDEX, "Sacred Grove warp portal", true, groveWarpActive},
    {"eldin gorge", GORGE_WARP_INDEX, "Eldin Gorge warp portal", true, gorgeWarpActive},
    {"kak village", KAKARIKO_WARP_INDEX, "Kakariko Village warp portal", true, kakarikoWarpActive},
    {"death mountain", MOUNTAIN_WARP_INDEX, "Death Mountain warp portal", true,
     mountainWarpActive},
    {"eldin bridge", BRIDGE_WARP_INDEX, "Bridge of Eldin warp portal", true, bridgeWarpActive},
    {"castle town", TOWN_WARP_INDEX, "Castle Town warp portal", true, townWarpActive},
    {"lake hylia", LAKE_WARP_INDEX, "Lake Hylia warp portal", true, lakeWarpActive},
    {"zora's domain", DOMAIN_WARP_INDEX, "Zora's Domain warp portal", true, domainWarpActive},
    {"upper river", UZR_WARP_INDEX, "Upper Zora's River warp portal", true, uzrWarpActive},
    {"snowpeak", SNOWPEAK_WARP_INDEX, "Snowpeak warp portal", true, snowpeakWarpActive},
    {"gerudo mesa", MESA_WARP_INDEX, "Gerudo Mesa warp portal", true, mesaWarpActive},
    {"mirror chamber", MIRROR_WARP_INDEX, "Mirror Chamber warp portal", true, mirrorWarpActive},
};

KEEP_FUNC PortalFlagsMenu::PortalFlagsMenu(Cursor& cursor)
    : Menu(cursor) {}

PortalFlagsMenu::~PortalFlagsMenu() {}

bool getSaveSwitch(int32_t stage, int32_t flag) {
    return g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().isSwitch(flag);
}

void setSaveSwitch(int32_t stage, int32_t flag) {
    if (getSaveSwitch(stage, flag)) {
        g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().offSwitch(flag);
    } else {
        g_dComIfG_gameInfo.info.getSavedata().getSave(stage).getBit().onSwitch(flag);
    }
}

inline void setRegionFlag(int regionBit) {
    dSv_player_field_last_stay_info_c& stay = g_dComIfG_gameInfo.info.getPlayer().getPlayerFieldLastStayInfo();
    u8* regionFlags = reinterpret_cast<u8*>(&stay) + 0x19;
    *regionFlags ^= (1 << regionBit);
}

inline bool getRegionFlag(int regionBit) {
    return g_dComIfG_gameInfo.info.getPlayer().getPlayerFieldLastStayInfo().isRegionBit(regionBit);
}

void PortalFlagsMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);

    portalFlagsData->l_mapRegion = getRegionFlag(portalFlagsData->l_selRegion + 1);
    portalFlagsData->l_springWarp = getSaveSwitch(ORDON, 52);
    portalFlagsData->l_sfaronWarp = getSaveSwitch(FARON, 71);
    portalFlagsData->l_nfaronWarp = getSaveSwitch(FARON, 2);
    portalFlagsData->l_groveWarp = getSaveSwitch(GROVE, 100);
    portalFlagsData->l_gorgeWarp = getSaveSwitch(FIELD, 21);
    portalFlagsData->l_kakWarp = getSaveSwitch(ELDIN, 31);
    portalFlagsData->l_mountainWarp = getSaveSwitch(ELDIN, 21);
    portalFlagsData->l_bridgeWarp = getSaveSwitch(FIELD, 99);
    portalFlagsData->l_ctWarp = getSaveSwitch(FIELD, 3);
    portalFlagsData->l_lakeWarp = getSaveSwitch(LANAYRU, 10);
    portalFlagsData->l_domainWarp = getSaveSwitch(LANAYRU, 2);
    portalFlagsData->l_uzrWarp = getSaveSwitch(LANAYRU, 21);
    portalFlagsData->l_snowpeakWarp = getSaveSwitch(SNOWPEAK, 21);
    portalFlagsData->l_mesaWarp = getSaveSwitch(DESERT, 21);
    portalFlagsData->l_mirrorWarp = getSaveSwitch(DESERT, 40);

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        switch (cursor.y) {
        case REGION_FLAG_INDEX:
            setRegionFlag(portalFlagsData->l_selRegion + 1);
            break;
        case SPRING_WARP_INDEX:
            setSaveSwitch(ORDON, 52);
            break;
        case S_FARON_WARP_INDEX:
            setSaveSwitch(FARON, 71);
            break;
        case N_FARON_WARP_INDEX:
            setSaveSwitch(FARON, 2);
            break;
        case GROVE_WARP_INDEX:
            setSaveSwitch(GROVE, 100);
            break;
        case GORGE_WARP_INDEX:
            setSaveSwitch(FIELD, 21);
            break;
        case KAKARIKO_WARP_INDEX:
            setSaveSwitch(ELDIN, 31);
            break;
        case MOUNTAIN_WARP_INDEX:
            setSaveSwitch(ELDIN, 21);
            break;
        case BRIDGE_WARP_INDEX:
            setSaveSwitch(FIELD, 99);
            break;
        case TOWN_WARP_INDEX:
            setSaveSwitch(FIELD, 3);
            break;
        case LAKE_WARP_INDEX:
            setSaveSwitch(LANAYRU, 10);
            break;
        case DOMAIN_WARP_INDEX:
            setSaveSwitch(LANAYRU, 2);
            break;
        case UZR_WARP_INDEX:
            setSaveSwitch(LANAYRU, 21);
            break;
        case SNOWPEAK_WARP_INDEX:
            setSaveSwitch(SNOWPEAK, 21);
            break;
        case MESA_WARP_INDEX:
            setSaveSwitch(DESERT, 21);
            break;
        case MIRROR_WARP_INDEX:
            setSaveSwitch(DESERT, 40);
            break;
        }
        // copy stage save flags to temp flags
        dComIfGs_getSave(*reinterpret_cast<s8*>(&g_dComIfG_gameInfo.info.getDan()));
    }

    switch (cursor.y) {
    case SELECT_REGION_INDEX:
        cursor.x = portalFlagsData->l_selRegion;
        cursor.move(MAX_REGION_OPTIONS, MENU_LINE_NUM);

        if (cursor.y == SELECT_REGION_INDEX) {
            portalFlagsData->l_selRegion = cursor.x;
        }
        break;
    default:
        cursor.move(0, MENU_LINE_NUM);
        break;
    }

    ListMember region_opt[MAX_REGION_OPTIONS] = {
        "ordon", "faron", "eldin", "lanayru", "desert", "snowpeak",
    };

    lines[SELECT_REGION_INDEX].printf(" <%s>", region_opt[portalFlagsData->l_selRegion].member);

    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
