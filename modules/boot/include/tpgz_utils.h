#pragma once
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
int popcount(uint32_t i);
