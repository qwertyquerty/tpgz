#include "tpgz_utils.h"
#include <cstring>
#include "rels/include/defines.h"

// Toggles save event flags
KEEP_FUNC void setEventFlag(u16 flag) {
    dComIfGs_isEventBit__FUs(flag) ? dComIfGs_offEventBit(flag) : dComIfGs_onEventBit(flag);
}

// Toggles temp event flags
KEEP_FUNC void setTempEventFlag(u16 flag) {
    dComIfGs_isTmpBit(flag) ? dComIfGs_offTmpBit(flag) : dComIfGs_onTmpBit(flag);
}

// Toggles equipment items
KEEP_FUNC void setItemFirstBit(u8 item) {
    dComIfGs_isItemFirstBit__FUc(item) ? dComIfGs_offItemFirstBit(item) : dComIfGs_onItemFirstBit(item);
}

// Toggle dungeon switches
KEEP_FUNC void setDungeonSwitch(int pFlag, int i_roomNo) {
    dComIfGs_isSwitch(pFlag, i_roomNo) ? dComIfGs_offSwitch(pFlag, i_roomNo) : dComIfGs_onSwitch(pFlag, i_roomNo);
}

// Set Savefile spawn info
KEEP_FUNC void setReturnPlace(const char* stage, s8 room, u8 spawn) {
    dSv_player_return_place_c__set(&g_dComIfG_gameInfo.info.getPlayer().mPlayerReturnPlace, stage,
                                   room, spawn);
}

KEEP_FUNC void setNextStageName(const char* name) {
    strcpy((char*)g_dComIfG_gameInfo.play.mNextStage.mStage, name);
}

KEEP_FUNC void setNextStageLayer(s8 layer) {
    g_dComIfG_gameInfo.play.mNextStage.setLayer(layer);
}

KEEP_FUNC void setNextStageRoom(s8 room) {
    g_dComIfG_gameInfo.play.mNextStage.setRoomNo(room);
}

KEEP_FUNC void setNextStagePoint(s16 point) {
    g_dComIfG_gameInfo.play.mNextStage.setPoint(point);
}

KEEP_FUNC int popcount(uint32_t i)
{
     i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
     i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
     i *= 0x01010101;                        // horizontal sum of bytes
     return  i >> 24;               // return just that top byte (after truncating to 32-bit even when int is wider than uint32_t)
}
