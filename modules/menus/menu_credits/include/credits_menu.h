#ifndef TPGZ_MODULES_MENUS_MENU_CREDITS_INCLUDE_CREDITS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_CREDITS_INCLUDE_CREDITS_MENU_H
#include "menus/menu.h"

class CreditsMenu : public Menu {
public:
    CreditsMenu(Cursor& cursor);
    virtual ~CreditsMenu();
    virtual void draw();
};

#endif
