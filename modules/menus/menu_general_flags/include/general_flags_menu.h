#ifndef TPGZ_MODULES_MENUS_MENU_GENERAL_FLAGS_INCLUDE_GENERAL_FLAGS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_GENERAL_FLAGS_INCLUDE_GENERAL_FLAGS_MENU_H
#include "menus/menu.h"

struct GeneralFlagsData {
    bool l_bossFlag;
    bool l_coroTD;
    bool l_midnaCharge;
    bool l_transformWarp;
    bool l_midnaZ;
    bool l_ruslTD;
    bool l_eponaStolen;
    bool l_eponaTamed;
    bool l_maloMartCT;
    bool l_mapWarping;
    bool l_midnaHealed;
    bool l_midnaRide;
    bool l_wolfSense;
};

enum GeneralFlagsIndex {
    BOSS_FLAG_INDEX,
    CORO_TD_INDEX,
    EPONA_STOLEN_INDEX,
    EPONA_TAMED_INDEX,
    MALO_MART_CT_INDEX,
    MAP_WARPING_INDEX,
    MIDNA_CHARGE_INDEX,
    MIDNA_HEALTHY,
    MIDNA_ON_BACK,
    MIDNA_Z_INDEX,
    RUSL_TD_INDEX,
    TRANSFORM_WARP_INDEX,
    WOLF_SENSE_INDEX,

    GENERAL_FLAGS_COUNT
};

extern GeneralFlagsData* generalFlagsData;

class GeneralFlagsMenu : public Menu {
public:
    GeneralFlagsMenu(Cursor&);
    virtual ~GeneralFlagsMenu();
    virtual void draw();

private:
};

#endif
