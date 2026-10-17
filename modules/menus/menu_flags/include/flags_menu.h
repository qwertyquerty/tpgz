#ifndef TPGZ_MODULES_MENUS_MENU_FLAGS_INCLUDE_FLAGS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_FLAGS_INCLUDE_FLAGS_MENU_H
#include "menus/menu.h"

enum FlagsIndex {
    GENERAL_FLAGS_INDEX,
    DUNGEON_FLAGS_INDEX,
    PORTAL_FLAGS_INDEX,
    RUPEE_FLAGS_INDEX,
    FLAG_RECORDS_INDEX,
    FLAG_LOG_INDEX,

    FLAGS_COUNT
};

class FlagsMenu : public Menu {
public:
    FlagsMenu(Cursor&);
    virtual ~FlagsMenu();
    virtual void draw();

private:
};

#endif
