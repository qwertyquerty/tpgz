#include <main.h>
#include "menus/menu_golden_bugs/include/golden_bugs_menu.h"
#include "events/draw_listener.h"
#include "menus/utils/menu_mgr.h"
#include "utils/draw.h"

void onCreate();
void onLoad();
void onDraw();
void onUnload();
void onDelete();

GoldenBugMenu* l_goldenBugMenu;

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
    g_menuMgr->setPersistentData(new GoldenBugData);
    if (!g_menuMgr->getPermanentData<Cursor>()) {
        g_menuMgr->setPermanentData(new Cursor);
    }
}

void onLoad() {
    gbData = g_menuMgr->getPersistentData<GoldenBugData>();
    l_goldenBugMenu = new GoldenBugMenu(*g_menuMgr->getPermanentData<Cursor>());
    g_drawListener->addListener(onDraw);
}

void onDraw() {
    l_goldenBugMenu->draw();
}

void onUnload() {
    g_drawListener->removeListener(onDraw);
    delete l_goldenBugMenu;
}

void onDelete() {
    GoldenBugData* data = g_menuMgr->getPersistentData<GoldenBugData>();
    delete data;
    g_menuMgr->setPersistentData(NULL);
}
