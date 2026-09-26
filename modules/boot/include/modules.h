#ifndef TPGZ_MODULES_BOOT_INCLUDE_MODULES_H
#define TPGZ_MODULES_BOOT_INCLUDE_MODULES_H
#include <stdint.h>
#include "utils/rels.h"
#include "utils/containers/deque.h"

struct Module {
    Module(bool (*active_)(), const char* path) : active(active_), rel(path) {}

    bool (*active)();
    tpgz::dyn::GZModule rel;
};

extern tpgz::containers::deque<Module*> g_modules;

void GZ_handleModules();
bool inputViewer_active();
bool freeCam_active();
bool moveLink_active();
bool projectionView_active();
bool triggerViewer_active();
bool actorView_active();
bool transformIndicator_active();
bool umd_active();
#ifdef WII_PLATFORM
bool bit_active();
bool slash_active();
#endif
bool corotd_active();
bool lfc_active();
bool ebmb_active();
bool ee_active();
bool mash_checker_active();
#define GORGE_VOID_MODE_COUNT 3

uint32_t GZ_getGorgeVoidMode();
bool gorge_active();
bool gorge_wolf_active();
bool gorge_human_active();
bool rollcheck_active();
bool moon_jump_active();
bool saveStates_active();
bool freeze_actor_active();
bool hide_actor_active();
bool freeze_camera_active();
bool fast_eel_regrab_active();

#endif
