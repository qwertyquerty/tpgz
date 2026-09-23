#pragma once

#include <cstdint>
#include "tpgz_vec.h"
#include "SSystem/SComponent/c_xyz.h"

struct PositionData {
    cXyz link;
    CameraMatrix cam;
    uint16_t angle;
};

extern PositionData memfile_posdata;

void GZ_setLinkPosition();