#ifndef TPGZ_MODULES_MENUS_MENU_PROJECTION_VIEW_INCLUDE_PROJECTION_VIEW_MENU_H
#define TPGZ_MODULES_MENUS_MENU_PROJECTION_VIEW_INCLUDE_PROJECTION_VIEW_MENU_H
#include "menus/menu.h"
#include "features/projection_view/include/projection_view.h"

class ProjectionViewMenu : public Menu {
public:
    ProjectionViewMenu(Cursor&);
    virtual ~ProjectionViewMenu();
    virtual void draw();

private:
};

#endif
