#include <main.h>
#include "menus/menu_tools_rng/include/tools_rng_menu.h"
#include "events/draw_listener.h"
#include "menus/utils/menu_mgr.h"
#include "utils/draw.h"

void onCreate();
void onLoad();
void onDraw();
void onUnload();
void onDelete();

ToolsRngMenu* l_toolsRngMenu;

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
    g_menuMgr->setPersistentData(new ToolsRngData);
    if (!g_menuMgr->getPermanentData<Cursor>()) {
        g_menuMgr->setPermanentData(new Cursor);
    }
}

void onLoad() {
    toolsRngData = g_menuMgr->getPersistentData<ToolsRngData>();
    l_toolsRngMenu = new ToolsRngMenu(*g_menuMgr->getPermanentData<Cursor>());
    g_drawListener->addListener(onDraw);
}

void onDraw() {
    l_toolsRngMenu->draw();
}

void onUnload() {
    g_drawListener->removeListener(onDraw);
    delete l_toolsRngMenu;
}

void onDelete() {
    ToolsRngData* data = g_menuMgr->getPersistentData<ToolsRngData>();
    delete data;
    g_menuMgr->setPersistentData(NULL);
}
