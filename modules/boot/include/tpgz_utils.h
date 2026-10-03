#ifndef TPGZ_MODULES_BOOT_INCLUDE_TPGZ_UTILS_H
#define TPGZ_MODULES_BOOT_INCLUDE_TPGZ_UTILS_H
#include <cstdint>
#include "d/d_com_inf_game.h"
void setEventFlag(u16 flag);
void setTempEventFlag(u16 flag);
void setItemFirstBit(u8 item);
void setDungeonSwitch(int pFlag, int i_roomNo);
void setReturnPlace(const char* stage, s8 room, u8 spawn);
void setNextStageName(const char* name);
void setNextStageLayer(s8 layer);
void setNextStageRoom(s8 room);
void setNextStagePoint(s16 point);
void setNextStageWipe(s8 wipe);
void enableNextStage();
int popcount(uint32_t i);

#endif
