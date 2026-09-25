#ifndef TPGZ_MODULES_MENUS_MENU_CHEATS_INCLUDE_CHEATS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_CHEATS_INCLUDE_CHEATS_MENU_H
#include "menus/menu.h"
#include "cheats.h"

#ifdef GCN_PLATFORM
#define CHEAT_AMNT 15
#define MOON_JUMP_TEXT "R+A"
#endif

#ifdef WII_PLATFORM
#define CHEAT_AMNT 16
#define MOON_JUMP_TEXT "Z+C+A"
#endif

enum CheatId {
    InfiniteAir,
    InfiniteArrows,
    InfiniteBombs,
    InfiniteHearts,
    InfiniteOil,
    InfiniteRupees,
    InfiniteSlingshot,
    Invincible,
    InvincibleEnemies,
    MoonJump,
    DoorStorage,
    SuperClawshot,
    UnrestrictedItems,
    TransformAnywhere,
    DisableItemTimer,
#ifdef WII_PLATFORM
    GaleLJA,
#endif
    SuperSpinner,
};

class CheatsMenu : public Menu {
public:
    CheatsMenu(Cursor&);
    virtual ~CheatsMenu();
    virtual void draw();

private:
};

#endif
