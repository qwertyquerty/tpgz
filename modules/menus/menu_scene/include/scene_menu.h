#ifndef TPGZ_MODULES_MENUS_MENU_SCENE_INCLUDE_SCENE_MENU_H
#define TPGZ_MODULES_MENUS_MENU_SCENE_INCLUDE_SCENE_MENU_H
#include "menus/menu.h"
#include "scene.h"

class SceneMenu : public Menu {
public:
    SceneMenu(Cursor&);
    virtual ~SceneMenu();
    virtual void draw();

private:
};

#endif
