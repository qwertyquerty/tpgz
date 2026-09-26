#ifndef TPGZ_MODULES_MENUS_MENU_WATCHES_INCLUDE_WATCHES_MENU_H
#define TPGZ_MODULES_MENUS_MENU_WATCHES_INCLUDE_WATCHES_MENU_H
#include "menus/menu.h"
#include "utils/memory.h"

#define MAX_WATCHES 50

struct WatchesData {
    WatchesData() : button_held_counter(0), l_scrollSpeed(1.0f), l_addrIdx(3), l_offsetIdx(2) {}

    uint8_t button_held_counter;
    float l_scrollSpeed;
    uint8_t l_addrIdx;
    uint8_t l_offsetIdx;
};

class WatchesMenu : public Menu {
public:
    WatchesMenu(Cursor&, WatchesData&);
    virtual ~WatchesMenu();
    virtual void draw();
    void drawMemoryLines(MemoryWatch*, size_t);

private:
    uint8_t& button_held_counter;
    float& l_scrollSpeed;
    uint8_t& l_addrIdx;
    uint8_t& l_offsetIdx;
};

#endif
