#ifndef TPGZ_MODULES_MENUS_MENU_FLAG_LOG_INCLUDE_FLAG_LOG_MENU_H
#define TPGZ_MODULES_MENUS_MENU_FLAG_LOG_INCLUDE_FLAG_LOG_MENU_H
#include "menus/menu.h"

class FlagLogMenu : public Menu {
public:
    FlagLogMenu(Cursor&);
    virtual ~FlagLogMenu();
    virtual void draw();

private:
};

#endif
