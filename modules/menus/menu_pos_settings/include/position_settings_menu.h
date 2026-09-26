#ifndef TPGZ_MODULES_MENUS_MENU_POS_SETTINGS_INCLUDE_POSITION_SETTINGS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_POS_SETTINGS_INCLUDE_POSITION_SETTINGS_MENU_H
#include "menus/menu.h"
#include "pos_settings.h"
#define POSITION_SETTINGS_NO_SELECTION ((uint8_t)-1)

struct PosSettingsData {
    PosSettingsData() : l_selItem(POSITION_SETTINGS_NO_SELECTION) {}

    bool init_once;
    float l_cursorSpeed;
    uint8_t l_selItem;
};

class PosSettingsMenu : public Menu {
public:
    PosSettingsMenu(Cursor&, PosSettingsData&);
    virtual ~PosSettingsMenu();
    virtual void draw();

private:
    bool& init_once;

    float& l_cursorSpeed;
    uint8_t& l_selItem;

};

#endif
