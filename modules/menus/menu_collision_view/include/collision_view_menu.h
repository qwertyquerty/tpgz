#ifndef TPGZ_MODULES_MENUS_MENU_COLLISION_VIEW_INCLUDE_COLLISION_VIEW_MENU_H
#define TPGZ_MODULES_MENUS_MENU_COLLISION_VIEW_INCLUDE_COLLISION_VIEW_MENU_H
#include "menus/menu.h"

class CollisionViewMenu : public Menu {
public:
    CollisionViewMenu(Cursor&);
    virtual ~CollisionViewMenu();
    virtual void draw();

private:
};

#endif
