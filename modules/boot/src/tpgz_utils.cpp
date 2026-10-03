#include "tpgz_utils.h"
#include <cstring>
#include "rels/include/defines.h"

// Toggles save event flags
KEEP_FUNC void setEventFlag(u16 flag) {
    dComIfGs_isEventBit(flag) ? dComIfGs_offEventBit(flag) : dComIfGs_onEventBit(flag);
}

// Toggles temp event flags
KEEP_FUNC void setTempEventFlag(u16 flag) {
    dComIfGs_isTmpBit(flag) ? dComIfGs_offTmpBit(flag) : dComIfGs_onTmpBit(flag);
}

// Toggles equipment items
KEEP_FUNC void setItemFirstBit(u8 item) {
    dComIfGs_isItemFirstBit(item) ? dComIfGs_offItemFirstBit(item) : dComIfGs_onItemFirstBit(item);
}

// Toggle dungeon switches
KEEP_FUNC void setDungeonSwitch(int pFlag, int i_roomNo) {
    dComIfGs_isSwitch(pFlag, i_roomNo) ? dComIfGs_offSwitch(pFlag, i_roomNo) : dComIfGs_onSwitch(pFlag, i_roomNo);
}

// Set Savefile spawn info
KEEP_FUNC void setReturnPlace(const char* stage, s8 room, u8 spawn) {
    g_dComIfG_gameInfo.info.getPlayer().getPlayerReturnPlace().set(stage, room, spawn);
}

static s8 l_nextStageWipe = -1;

static dStage_nextStage_c& getNextStage() {
    return g_dComIfG_gameInfo.play.mNextStage;
}

KEEP_FUNC void setNextStageName(const char* name) {
    dStage_nextStage_c& next = getNextStage();
    next.dStage_startStage_c::set(name, next.getRoomNo(), next.getPoint(), next.getLayer());
}

KEEP_FUNC void setNextStageLayer(s8 layer) {
    getNextStage().setLayer(layer);
}

KEEP_FUNC void setNextStageRoom(s8 room) {
    dStage_nextStage_c& next = getNextStage();
    char name[8];
    strcpy(name, next.getName());
    next.dStage_startStage_c::set(name, room, next.getPoint(), next.getLayer());
}

KEEP_FUNC void setNextStagePoint(s16 point) {
    dStage_nextStage_c& next = getNextStage();
    char name[8];
    strcpy(name, next.getName());
    next.dStage_startStage_c::set(name, next.getRoomNo(), point, next.getLayer());
}

KEEP_FUNC void setNextStageWipe(s8 wipe) {
    l_nextStageWipe = wipe;
}

KEEP_FUNC void enableNextStage() {
    dStage_nextStage_c& next = getNextStage();
#ifdef WII_PLATFORM
    s8* flags = reinterpret_cast<s8*>(&next) + 0xD;
    s8 wipe = l_nextStageWipe >= 0 ? l_nextStageWipe : flags[1];
    u8 wipeSpeed = static_cast<u8>(flags[2]);
    flags[0] = 0;
#else
    s8 wipe = l_nextStageWipe >= 0 ? l_nextStageWipe : next.getWipe();
    u8 wipeSpeed = next.getWipeSpeed();
    next.offEnable();
#endif
    l_nextStageWipe = -1;
    char name[8];
    strcpy(name, next.getName());
    next.set(name, next.getRoomNo(), next.getPoint(), next.getLayer(), wipe, wipeSpeed);
}

KEEP_FUNC int popcount(uint32_t i)
{
     i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
     i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
     i *= 0x01010101;                        // horizontal sum of bytes
     return  i >> 24;               // return just that top byte (after truncating to 32-bit even when int is wider than uint32_t)
}
