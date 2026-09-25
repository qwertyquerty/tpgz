#ifndef TPGZ_MODULES_MENUS_MENU_MEMORY_INCLUDE_MEMORY_MENU_H
#define TPGZ_MODULES_MENUS_MENU_MEMORY_INCLUDE_MEMORY_MENU_H
#include "menus/menu.h"

class MemoryMenu : public Menu {
public:
    MemoryMenu(Cursor&);
    virtual ~MemoryMenu();
    virtual void draw();

private:
};

#endif
