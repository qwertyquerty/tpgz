#ifndef TPGZ_MODULES_MENUS_MENU_PRACTICE_INCLUDE_PRACTICE_MENU_H
#define TPGZ_MODULES_MENUS_MENU_PRACTICE_INCLUDE_PRACTICE_MENU_H
#include "menus/menu.h"
#include "practice.h"
#include "gz_flags.h"

#ifdef GCN_PLATFORM
#define PRACTICE_MENU_NUM 6
#elif defined WII_PLATFORM
#define PRACTICE_MENU_NUM 4
#endif

class PracticeMenu : public Menu {
public:
    PracticeMenu(Cursor&);
    virtual ~PracticeMenu();
    virtual void draw();

private:
};

#endif
