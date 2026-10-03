#ifndef TPGZ_MODULES_BOOT_INCLUDE_MEMFILES_H
#define TPGZ_MODULES_BOOT_INCLUDE_MEMFILES_H
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

#endif
