#ifndef TPGZ_MODULES_MENUS_MENU_TOOLS_INCLUDE_TOOLS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_TOOLS_INCLUDE_TOOLS_MENU_H
#include "menus/menu.h"
#include "tools.h"

class ToolsMenu : public Menu {
public:
    ToolsMenu(Cursor&);
    virtual ~ToolsMenu();
    virtual void draw();

private:
};

#endif
