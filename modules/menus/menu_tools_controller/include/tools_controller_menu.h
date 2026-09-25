#ifndef TPGZ_MODULES_MENUS_MENU_TOOLS_CONTROLLER_INCLUDE_TOOLS_CONTROLLER_MENU_H
#define TPGZ_MODULES_MENUS_MENU_TOOLS_CONTROLLER_INCLUDE_TOOLS_CONTROLLER_MENU_H
#include "menus/menu.h"
#include "tools.h"

enum ToolsControllerIndex {
    INPUT_VIEWER_INDEX,
    TURBO_MODE_INDEX,

    TOOLS_CONTROLLER_COUNT
};

class ToolsControllerMenu : public Menu {
public:
    ToolsControllerMenu(Cursor&);
    virtual ~ToolsControllerMenu();
    virtual void draw();

private:
};

#endif
