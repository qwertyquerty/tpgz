#include <main.h>
#include "menus/menu_timer_settings/include/timer_settings_menu.h"
#include "events/draw_listener.h"
#include "menus/utils/menu_mgr.h"
#include "utils/draw.h"

void onCreate();
void onLoad();
void onDraw();
void onUnload();
void onDelete();

TimerSettingsMenu* l_timerSettingsMenu;

namespace tpgz {
namespace modules {
void main() {
    g_menuMgr->setCreateHook(onCreate);
    g_menuMgr->setLoadHook(onLoad);
    g_menuMgr->setUnloadHook(onUnload);
    g_menuMgr->setDeleteHook(onDelete);
}
void exit() {
    g_menuMgr->setCreateHook(NULL);
    g_menuMgr->setLoadHook(NULL);
    g_menuMgr->setUnloadHook(NULL);
    g_menuMgr->setDeleteHook(NULL);
}
}
}  // namespace tpgz::modules

void onCreate() {
    if (!g_menuMgr->getPermanentData<Cursor>()) {
        g_menuMgr->setPermanentData(new Cursor);
    }
}

void onLoad() {
    l_timerSettingsMenu = new TimerSettingsMenu(*g_menuMgr->getPermanentData<Cursor>());
    g_drawListener->addListener(onDraw);
}

void onDraw() {
    l_timerSettingsMenu->draw();
}

void onUnload() {
    g_drawListener->removeListener(onDraw);
    delete l_timerSettingsMenu;
}

void onDelete() {}
