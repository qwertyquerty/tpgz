#include <cstdio>
#include "collision_view.h"
#include "trigger_view.h"
#include "menus/menu_trigger_view/include/trigger_view_menu.h"
#include "settings.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_actor_mng.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

static bool viewAttnDistsActive() {
    return g_triggerViewFlags[VIEW_ATTN_DISTS].active;
}

static bool viewEventAreasActive() {
    return g_triggerViewFlags[VIEW_EVENT_AREAS].active;
}

static bool viewLoadZonesActive() {
    return g_triggerViewFlags[VIEW_LOAD_ZONES].active;
}

static bool viewMidnaStopsActive() {
    return g_triggerViewFlags[VIEW_MIDNA_STOPS].active;
}

static bool viewPathsActive() {
    return g_triggerViewFlags[VIEW_PATHS].active;
}

static bool viewMistAvoidActive() {
    return g_triggerViewFlags[VIEW_MIST_AVOID].active;
}

static bool viewChgRestartsActive() {
    return g_triggerViewFlags[VIEW_CHG_RESTARTS].active;
}

static bool viewSwitchAreasActive() {
    return g_triggerViewFlags[VIEW_SWITCH_AREAS].active;
}

static bool viewTransformDistsActive() {
    return g_triggerViewFlags[VIEW_TRANSFORM_DISTS].active;
}

static bool viewTwGatesActive() {
    return g_triggerViewFlags[VIEW_TW_GATES].active;
}

static bool viewLeeverRangeActive() {
    return g_triggerViewFlags[VIEW_LEEVER_RANGE].active;
}

static Line lines[TRIGGER_VIEW_MAX + 1] = {
    {"attention distances", VIEW_ATTN_DISTS, "view actor attention ranges", true,
    viewAttnDistsActive},
    {"event areas", VIEW_EVENT_AREAS, "view event triggers", true,
    viewEventAreasActive},
    {"load zones", VIEW_LOAD_ZONES, "view load triggers", true,
    viewLoadZonesActive},
    {"midna stops", VIEW_MIDNA_STOPS, "view midna talk triggers", true,
    viewMidnaStopsActive},
    {"paths", VIEW_PATHS, "view all stage/room paths", true,
    viewPathsActive},
    {"purple mist avoid", VIEW_MIST_AVOID, "view purple mist safe area", true,
    viewMistAvoidActive},
    {"restart changes", VIEW_CHG_RESTARTS, "view spawn location update triggers", true,
    viewChgRestartsActive},
    {"switch areas", VIEW_SWITCH_AREAS, "view switch activation triggers", true,
    viewSwitchAreasActive},
    {"transform distances", VIEW_TRANSFORM_DISTS, "view actor transform blocking distances", true,
    viewTransformDistsActive},
    {"twilight gates", VIEW_TW_GATES, "view twilight gate triggers", true,
    viewTwGatesActive},
    {"leever ranges", VIEW_LEEVER_RANGE, "view leever ranges", true,
    viewLeeverRangeActive},
    {"opacity:", TRIGGER_VIEW_MAX, "the opacity of drawn geometry"},
};

KEEP_FUNC TriggerViewMenu::TriggerViewMenu(Cursor& cursor)
    : Menu(cursor) {}

TriggerViewMenu::~TriggerViewMenu() {}

void TriggerViewMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        g_triggerViewFlags[cursor.y].active = !g_triggerViewFlags[cursor.y].active;
    }

    switch (cursor.y) {
    case TRIGGER_VIEW_MAX:
        Cursor::moveList(g_geometryOpacity);
        break;
    }

    lines[TRIGGER_VIEW_MAX].printf(" <%d>", g_geometryOpacity);

    cursor.move(0, MENU_LINE_NUM);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
