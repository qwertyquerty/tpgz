#ifndef TPGZ_MODULES_MENUS_MENU_TRIGGER_VIEW_INCLUDE_TRIGGER_VIEW_MENU_H
#define TPGZ_MODULES_MENUS_MENU_TRIGGER_VIEW_INCLUDE_TRIGGER_VIEW_MENU_H
#include "menus/menu.h"
#include "trigger_view.h"

class TriggerViewMenu : public Menu {
public:
    TriggerViewMenu(Cursor&);
    virtual ~TriggerViewMenu();
    virtual void draw();

private:
};

#endif
