#ifndef TPGZ_MODULES_MENUS_MENU_DUNGEON_FLAGS_INCLUDE_DUNGEON_FLAGS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_DUNGEON_FLAGS_INCLUDE_DUNGEON_FLAGS_MENU_H
#include "menus/menu.h"

struct DungeonFlagsData {
    bool init_once;
    // bool l_introFlag;
    bool l_mapFlag;
    bool l_compassFlag;
    bool l_bosskeyFlag;
    bool l_minibossFlag;
    bool l_bossFlag;
    int l_selDun;
    uint8_t l_keyNum;
};

enum DungeonFlagsIndex {
    SELECT_DUNGEON_INDEX,
    SMALL_KEY_FLAG_INDEX,
    // INTRO_CS_FLAG_INDEX,
    MAP_FLAG_INDEX,
    COMPASS_FLAG_INDEX,
    BOSS_KEY_FLAG_INDEX,
    DEFEAT_MINIBOSS_FLAG_INDEX,
    DEFEAT_BOSS_FLAG_INDEX,
    CLEAR_DUNGEON_FLAGS_INDEX,

    DUNGEON_FLAGS_COUNT
};

extern DungeonFlagsData* dungeonFlagsData;

class DungeonFlagsMenu : public Menu {
public:
    DungeonFlagsMenu(Cursor&);
    virtual ~DungeonFlagsMenu();
    virtual void draw();

private:

};

#endif
