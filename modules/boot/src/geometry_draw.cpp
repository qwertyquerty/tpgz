#include "defines.h"
#include "SSystem/SComponent/c_m3d.h"
#include "m_Do/m_Do_mtx.h"
#include "d/d_cc_d.h"
#include "d/actor/d_a_alink.h"
#include "collision_view.h"
#include "global_data.h"
#include "d/d_com_inf_game.h"
#include "gx.h"
#include "JSystem/J3DGraphBase/J3DSys.h"
#include "JSystem/J3DGraphBase/J3DDrawBuffer.h"
#include "JSystem/J3DGraphBase/J3DShape.h"
#include "m_Do/m_Do_printf.h"
#include "d/d_bg_s_capt_poly.h"
#include "tpgz_math.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_camera_mng.h"

#include <cstdio>
//#include <math.h>
#include "font.h"

KEEP_VAR CollisionItem g_collisionFlags[VIEW_COLLISION_MAX] = {
    {VIEW_POLYGON_GROUND, false},
    {VIEW_POLYGON_WALL, false},
    {VIEW_POLYGON_ROOF, false},
    {VIEW_POLYGON_EDGES, false},
    {VIEW_AT_CC, false},
    {VIEW_TG_CC, false},
    {VIEW_CO_CC, false},
};

#define DRAW_PACKET_MAX 1000  // max amount of draw packets allowed to be drawn at a time

u32 l_drawPacketListNum;
static J3DPacket* l_drawPacketList[DRAW_PACKET_MAX];

J3DPacket* dDbVw_setDrawPacketList(J3DPacket* p_packet, int buf_type) {
    if (p_packet == NULL) {
        p_packet = NULL;
    } else {
        if (l_drawPacketListNum >= DRAW_PACKET_MAX) {
            delete p_packet;
            p_packet = NULL;
        } else {
            l_drawPacketList[l_drawPacketListNum] = p_packet;
            l_drawPacketListNum++;
            j3dSys.getDrawBuffer(buf_type)->entryImm(p_packet, 0);
        }
    }
    return p_packet;
}

void dDbVw_deleteDrawPacketList() {
    for (u32 i = 0; i < l_drawPacketListNum; i++) {
        if (l_drawPacketList[i] != NULL) {
            delete l_drawPacketList[i];
            l_drawPacketList[i] = NULL;
        }
    }

    l_drawPacketListNum = 0;
}

//-------------------------------------------------------
//                         CUBE
//-------------------------------------------------------

void drawCube(MtxP mtx, cXyz* array, const GXColor& color) {
    GXSetArray(GX_VA_POS, array, sizeof(cXyz));
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, color);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);

    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 14);
    GXPosition1x8(4);
    GXPosition1x8(6);
    GXPosition1x8(5);
    GXPosition1x8(7);
    GXPosition1x8(3);
    GXPosition1x8(6);
    GXPosition1x8(2);
    GXPosition1x8(4);
    GXPosition1x8(0);
    GXPosition1x8(5);
    GXPosition1x8(1);
    GXPosition1x8(3);
    GXPosition1x8(0);
    GXPosition1x8(2);
    GXEnd();
}


KEEP_FUNC void dDbVw_drawCubeXlu(cXyz& pos, cXyz& size, csXyz& angle, const GXColor& color) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        GzCubePacket* cube = new GzCubePacket(pos, size, angle, color);

        dDbVw_setDrawPacketList(cube, 1);
    }
}

void mDoExt_cubePacket__dtor(GzCubePacket* i_this) {
    i_this->~GzCubePacket();
    delete i_this;
}

void GzCubePacket::draw() { mDoExt_cubePacket__draw(this); }

void mDoExt_cubePacket__draw(GzCubePacket* i_this) {
    static Vec points[8] = {
        {-1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
    };

    mDoMtx_stack_c::transS(i_this->mPosition.x, i_this-> mPosition.y, i_this->mPosition.z);
    mDoMtx_stack_c::XYZrotM(i_this->mAngle.x, i_this->mAngle.y, i_this->mAngle.z);
    mDoMtx_stack_c::scaleM(i_this->mSize.x, i_this->mSize.y, i_this->mSize.z);
    mDoMtx_stack_c::revConcat(j3dSys.getViewMtx());
    drawCube(mDoMtx_stack_c::get(), (cXyz*)points, i_this->mColor);
}

//-------------------------------------------------------
//                        SPHERE
//-------------------------------------------------------


KEEP_FUNC void dDbVw_drawSphereXlu(cXyz& position, f32 radius, const GXColor& color, u8 param_3) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        GzSpherePacket* sph = new GzSpherePacket(position, radius, color, param_3);

        dDbVw_setDrawPacketList(sph, 1);
    }
}

void mDoExt_spherePacket__dtor(GzSpherePacket* i_this) {
    i_this->~GzSpherePacket();
    delete i_this;
}

void GzSpherePacket::draw() { mDoExt_spherePacket__draw(this); }

void mDoExt_spherePacket__draw(GzSpherePacket* i_this) {
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

    if (i_this->_24) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);

    mDoMtx_stack_c::copy(j3dSys.getViewMtx());
    mDoMtx_stack_c::transM(i_this->mPosition.x, i_this->mPosition.y, i_this->mPosition.z);
    mDoMtx_stack_c::scaleM(i_this->mSize, i_this->mSize, i_this->mSize);

    GXLoadPosMtxImm(mDoMtx_stack_c::get(), 0);
    mDoMtx_stack_c::inverseTranspose();

    GXLoadNrmMtxImm(mDoMtx_stack_c::get(), 0);
    GXSetCurrentMtx(0);

    GXDrawSphere(8, 8);
}

//-------------------------------------------------------
//                       CYLINDER
//-------------------------------------------------------


KEEP_FUNC void dDbVw_drawCylinderXlu(cXyz& position, f32 radius, f32 height, const GXColor& color, u8 param_4) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        GzCylinderPacket* cyl = new GzCylinderPacket(position, radius, height, color, param_4);

        dDbVw_setDrawPacketList(cyl, 1);
    }
}

void mDoExt_cylinderPacket__dtor(GzCylinderPacket* i_this) {
    i_this->~GzCylinderPacket();
    delete i_this;
}

void GzCylinderPacket::draw() { mDoExt_cylinderPacket__draw(this); }

void mDoExt_cylinderPacket__draw(GzCylinderPacket* i_this) {
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

    if (i_this->_28) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);

    f32 height = i_this->mHeight;

    mDoMtx_stack_c::copy(j3dSys.getViewMtx());
    mDoMtx_stack_c::transM(i_this->mPosition.x, i_this->mPosition.y + (height / 2), i_this->mPosition.z);
    mDoMtx_stack_c::scaleM(i_this->mRadius, i_this->mRadius + (height / 2), i_this->mRadius);
    mDoMtx_stack_c::XrotM(0x4000);

    GXLoadPosMtxImm(mDoMtx_stack_c::get(), 0);
    mDoMtx_stack_c::inverseTranspose();

    GXLoadNrmMtxImm(mDoMtx_stack_c::get(), 0);
    GXSetCurrentMtx(0);

    GXDrawCylinder(8);
}

//-------------------------------------------------------
//                     8 Point Cube
//-------------------------------------------------------


KEEP_FUNC void dDbVw_drawCube8pXlu(cXyz* points, const GXColor& color) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        GzCube8pPacket* cube = new GzCube8pPacket(points, color);

        dDbVw_setDrawPacketList(cube, 1);
    }
}

void mDoExt_cube8pPacket__dtor(GzCube8pPacket* i_this) {
    i_this->~GzCube8pPacket();
    delete i_this;
}

void GzCube8pPacket::draw() { mDoExt_cube8pPacket__draw(this); }

void mDoExt_cube8pPacket__draw(GzCube8pPacket* i_this) {
    drawCube(j3dSys.getViewMtx(), i_this->mPoints, i_this->mColor);
}

//-------------------------------------------------------
//                       Triangle
//-------------------------------------------------------


KEEP_FUNC void dDbVw_drawTriangleXlu(cXyz* points, const GXColor& color, u8 param_2) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        GzTrianglePacket* tri = new GzTrianglePacket(points, color, param_2);

        dDbVw_setDrawPacketList(tri, 1);
    }
}

void mDoExt_trianglePacket__dtor(GzTrianglePacket* i_this) {
    i_this->~GzTrianglePacket();
    delete i_this;
}

void GzTrianglePacket::draw() { mDoExt_trianglePacket__draw(this); }

void mDoExt_trianglePacket__draw(GzTrianglePacket* i_this) {
    j3dSys.reinitGX();

    GXSetArray(GX_VA_POS, i_this->mPoints, sizeof(cXyz));
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXLoadPosMtxImm(j3dSys.getViewMtx(), 0);
    GXSetCurrentMtx(0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetZCompLoc(GX_ENABLE);

    if (i_this->_38) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);

    GXColor clearColor = {0, 0, 0, 0};
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, clearColor);
    GXSetFogRangeAdj(GX_DISABLE, 0, NULL);
    GXSetCullMode(GX_CULL_NONE);
    GXSetDither(GX_ENABLE);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetNumIndStages(0);

    GXBegin(GX_TRIANGLES, GX_VTXFMT0, 3);
    GXPosition1x8(0);
    GXPosition1x8(1);
    GXPosition1x8(2);
    GXEnd();

    J3DShape::resetVcdVatCache();
}

//-------------------------------------------------------
//                         Line
//-------------------------------------------------------


KEEP_FUNC void dDbVw_drawLineXlu(cXyz& pointA, cXyz& pointB, const GXColor& color, u8 param_3, u8 width) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        GzLinePacket* line = new GzLinePacket(pointA, pointB, color, param_3, width);

        dDbVw_setDrawPacketList(line, 1);
    }
}

void mDoExt_linePacket__dtor(GzLinePacket* i_this) {
    i_this->~GzLinePacket();
    delete i_this;
}

void GzLinePacket::draw() { mDoExt_linePacket__draw(this); }

void mDoExt_linePacket__draw(GzLinePacket* i_this) {
    j3dSys.reinitGX();

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXLoadPosMtxImm(j3dSys.getViewMtx(), 0);
    GXSetCurrentMtx(0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetZCompLoc(GX_ENABLE);

    if (i_this->_2C) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);

    GXColor clearColor = {0, 0, 0, 0};
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, clearColor);
    GXSetFogRangeAdj(GX_DISABLE, 0, NULL);
    GXSetCullMode(GX_CULL_NONE);
    GXSetDither(GX_ENABLE);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetNumIndStages(0);
    GXSetLineWidth(i_this->mWidth, GX_TO_ZERO);

    GXBegin(GX_LINES, GX_VTXFMT0, 2);
    GXPosition3f32(i_this->mPointA.x, i_this->mPointA.y, i_this->mPointA.z);
    GXPosition3f32(i_this->mPointB.x, i_this->mPointB.y, i_this->mPointB.z);
    GXEnd();

    J3DShape::resetVcdVatCache();
}

//-------------------------------------------------------
//                   Cylinder Matrix
//-------------------------------------------------------


KEEP_FUNC void dDbVw_drawCylinderMXlu(Mtx m, const GXColor& color, u8 param_2) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        GzCylinderMPacket* cylm = new GzCylinderMPacket(m, color, param_2);

        dDbVw_setDrawPacketList(cylm, 1);
    }
}

void mDoExt_cylinderMPacket__dtor(GzCylinderMPacket* i_this) {
    i_this->~GzCylinderMPacket();
    delete i_this;
}

void GzCylinderMPacket::draw() { mDoExt_cylinderMPacket__draw(this); }

void mDoExt_cylinderMPacket__draw(GzCylinderMPacket* i_this) {
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

    if (i_this->_44) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);

    PSMTXConcat(j3dSys.getViewMtx(), i_this->mMatrix, i_this->mMatrix);

    GXLoadPosMtxImm(i_this->mMatrix, 0);
    mDoMtx_inverseTranspose(i_this->mMatrix, i_this->mMatrix);

    GXLoadNrmMtxImm(i_this->mMatrix, 0);
    GXSetCurrentMtx(0);

    GXDrawCylinder(8);
}

//-------------------------------------------------------
//                        Circle
//-------------------------------------------------------


KEEP_FUNC void dDbVw_drawCircleXlu(cXyz& i_position, f32 i_radius, const GXColor& i_color, u8 param_3, u8 i_lineWidth) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        GzCirclePacket* circle = new GzCirclePacket(i_position, i_radius, i_color, param_3, i_lineWidth);

        dDbVw_setDrawPacketList(circle, 1);
    }
}

void mDoExt_circlePacket__dtor(GzCirclePacket* i_this) {
    i_this->~GzCirclePacket();
    delete i_this;
}

void GzCirclePacket::draw() { mDoExt_circlePacket__draw(this); }

void mDoExt_circlePacket__draw(GzCirclePacket* i_this) {
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->m_color);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

    if (i_this->_24) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetLineWidth(i_this->m_lineWidth, GX_TO_ZERO);
    GXLoadPosMtxImm(j3dSys.getViewMtx(), 0);
    GXSetCurrentMtx(0);

    cXyz sp38;
    cXyz sp44;
    int numEdges = 36;
    sp38.y = sp44.y = i_this->m_position.y;
    
    GXBegin(GX_LINES, GX_VTXFMT0, numEdges * 2);
    for (int i = 0; i < numEdges; i++) {
        sp38.x = fcos((i * 6.2831855f) / numEdges) * i_this->m_radius;
        sp38.z = fsin((i * 6.2831855f) / numEdges) * i_this->m_radius;

        sp44.x = fcos(((i + 1) * 6.2831855f) / numEdges) * i_this->m_radius;
        sp44.z = fsin(((i + 1) * 6.2831855f) / numEdges) * i_this->m_radius;

        sp38.x += i_this->m_position.x;
        sp38.z += i_this->m_position.z;
        sp44.x += i_this->m_position.x;
        sp44.z += i_this->m_position.z;
        GXPosition3f32(sp38.x, sp38.y, sp38.z);
        GXPosition3f32(sp44.x, sp44.y, sp44.z);
    }
    GXEnd();
}

#define MAX_DRAW_DIST 2000.0f

extern "C" void* __vt__8dCcD_Cyl[];
extern "C" void* __vt__8dCcD_Sph[];
extern "C" void* __vt__8dCcD_Cps[];

KEEP_FUNC void dCcD_Cyl_Draw(dCcD_Cyl* i_this, const GXColor& i_color) {
    if (dComIfGp_getPlayer(0)->current.pos.abs(i_this->GetC()) < MAX_DRAW_DIST) {
        dDbVw_drawCylinderXlu(i_this->GetC(), i_this->GetR(), i_this->GetH(), i_color, 1);
    }
}

KEEP_FUNC void dCcD_Sph_Draw(dCcD_Sph* i_this, const GXColor& i_color) {
    if (dComIfGp_getPlayer(0)->current.pos.abs(*i_this->GetCP()) < MAX_DRAW_DIST) {
        dDbVw_drawSphereXlu(*i_this->GetCP(), i_this->GetR(), i_color, 1);
    }
}

KEEP_FUNC void dCcD_Cps_Draw(dCcD_Cps* i_this, const GXColor& i_color) {
    if (dComIfGp_getPlayer(0)->current.pos.abs(*i_this->GetStartP()) < MAX_DRAW_DIST) {
        Mtx up_m;
        Mtx sp98;
        Mtx cyl_m;
        PSMTXIdentity(cyl_m);

        cXyz spD8;
        i_this->CalcVec(&spD8);

        mDoMtx_trans(sp98, i_this->GetStartP()->x, i_this->GetStartP()->y, i_this->GetStartP()->z);
        cM3d_UpMtx(spD8, up_m);
        mDoMtx_concat(sp98, up_m, cyl_m);

        mDoMtx_scale(sp98, i_this->GetR(), i_this->GetLen() * 0.5f, i_this->GetR());
        mDoMtx_concat(cyl_m, sp98, cyl_m);
        mDoMtx_trans(sp98, 0.0f, 1.0f, 0.0f);
        mDoMtx_concat(cyl_m, sp98, cyl_m);
        mDoMtx_XrotS(sp98, 0x4000);
        mDoMtx_concat(cyl_m, sp98, cyl_m);

        dDbVw_drawCylinderMXlu(cyl_m, i_color, 1);
        dDbVw_drawSphereXlu(*i_this->GetStartP(), i_this->GetR(), i_color, 1);
        dDbVw_drawSphereXlu(*i_this->GetEndP(), i_this->GetR(), i_color, 1);
    }
}

u16 dCcS_Data::at_obj_count = 0;
u16 dCcS_Data::tg_obj_count = 0;
u16 dCcS_Data::co_obj_count = 0;

KEEP_FUNC void GZ_drawCc(dCcS* i_this) {
    static bool init_vtables = false;
    if (!init_vtables) {
        // we patch the original vtables with our own functions so that
        // we can replicate the debug rom functionality of each collider type
        // having their own specific virtual draw function.
        __vt__8dCcD_Cyl[7] = (void*)dCcD_Cyl_Draw;
        __vt__8dCcD_Sph[7] = (void*)dCcD_Sph_Draw;
        __vt__8dCcD_Cps[7] = (void*)dCcD_Cps_Draw;
        init_vtables = true;
    }

    daAlink_c* player = (daAlink_c*)dComIfGp_getPlayer(0);
    if (player == NULL) {
        return;
    }

    if (g_collisionFlags[VIEW_AT_CC].active) {
        for (u16 i = 0; i < dCcS_Data::at_obj_count; i++) {
            cCcD_Obj* obj = i_this->mpObjAt[i];
            if (obj != NULL) {
                GXColor color = {0xFF, 0x00, 0x00, g_geometryOpacity};
                obj->Draw(color);
            }
        }
    }
    if (g_collisionFlags[VIEW_TG_CC].active) {
        for (u16 i = 0; i < dCcS_Data::tg_obj_count; i++) {
            cCcD_Obj* obj = i_this->mpObjTg[i];
            if (obj != NULL) {
                GXColor color = {0x3A, 0x82, 0xF0, g_geometryOpacity};
                obj->Draw(color);
            }
        }
    }
    if (g_collisionFlags[VIEW_CO_CC].active) {
        for (u16 i = 0; i < dCcS_Data::co_obj_count; i++) {
            cCcD_Obj* obj = i_this->mpObjCo[i];
            if (obj != NULL) {
                GXColor color = {0xFF, 0xFF, 0xFF, g_geometryOpacity};
                obj->Draw(color);
            }
        }
    }
}

//-------------------------------------------------------
//                      POLY DRAW
//-------------------------------------------------------

int poly_draw(dBgS_CaptPoly* i_captpoly, cBgD_Vtx_t* i_vtx, int i_ia, int i_ib, int i_ic, cM3dGPla* i_plane) {
    cXyz vertices[3];

    GXColor ground_col = {0xFF, 0x00, 0x00, g_geometryOpacity};
    GXColor roof_col = {0x00, 0x00, 0xFF, g_geometryOpacity};
    GXColor wall_col = {0x00, 0xFF, 0x00, g_geometryOpacity};

    GXColor flat_col = {0xFF, 0xC5, 0xC5, g_geometryOpacity};

    cXyz raise;
    PSVECScale(&i_plane->mNormal, &raise, (f32)g_collisionRaise);

    vertices[0] = i_vtx[i_ia];
    vertices[1] = i_vtx[i_ib];
    vertices[2] = i_vtx[i_ic];

    PSVECAdd(&vertices[0], &raise, &vertices[0]);
    PSVECAdd(&vertices[1], &raise, &vertices[1]);
    PSVECAdd(&vertices[2], &raise, &vertices[2]);

    if (cBgW_CheckBGround(i_plane->mNormal.y)) {
        if (g_collisionFlags[VIEW_POLYGON_GROUND].active) {
            if (i_plane->mNormal.y >= 1.0f) {
                // draw special color for fully flat ground
                dDbVw_drawTriangleXlu(vertices, flat_col, 1);
            } else {
                dDbVw_drawTriangleXlu(vertices, ground_col, 1);
            }
        }
    } else if (cBgW_CheckBRoof(i_plane->mNormal.y)) {
        if (g_collisionFlags[VIEW_POLYGON_ROOF].active) {
            dDbVw_drawTriangleXlu(vertices, roof_col, 1);
        }
    } else if (g_collisionFlags[VIEW_POLYGON_WALL].active) {
        dDbVw_drawTriangleXlu(vertices, wall_col, 1);
    }

    return 0;
}

int poly_edge_draw(dBgS_CaptPoly* i_captpoly, cBgD_Vtx_t* i_vtx, int i_ia, int i_ib, int i_ic, cM3dGPla* i_plane) {
    if (cBgW_CheckBGround(i_plane->mNormal.y)) {
        if (!g_collisionFlags[VIEW_POLYGON_GROUND].active) {
            return 0;
        }
    } else if (cBgW_CheckBRoof(i_plane->mNormal.y)) {
        if (!g_collisionFlags[VIEW_POLYGON_ROOF].active) {
            return 0;
        }
    } else if (!g_collisionFlags[VIEW_POLYGON_WALL].active) {
        return 0;
    }
    
    GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};

    cXyz raise;
    PSVECScale(&i_plane->mNormal, &raise, (f32)g_collisionRaise);

    cXyz start;
    cXyz end;

    // A to B
    start.set(i_vtx[i_ia].x, i_vtx[i_ia].y, i_vtx[i_ia].z);
    end.set(i_vtx[i_ib].x, i_vtx[i_ib].y, i_vtx[i_ib].z);
    PSVECAdd(&raise, &start, &start);
    PSVECAdd(&raise, &end, &end);
    dDbVw_drawLineXlu(start, end, color, 1, 12);

    // B to C
    start.set(i_vtx[i_ib].x, i_vtx[i_ib].y, i_vtx[i_ib].z);
    end.set(i_vtx[i_ic].x, i_vtx[i_ic].y, i_vtx[i_ic].z);
    PSVECAdd(&raise, &start, &start);
    PSVECAdd(&raise, &end, &end);
    dDbVw_drawLineXlu(start, end, color, 1, 12);

    // C to A
    start.set(i_vtx[i_ic].x, i_vtx[i_ic].y, i_vtx[i_ic].z);
    end.set(i_vtx[i_ia].x, i_vtx[i_ia].y, i_vtx[i_ia].z);
    PSVECAdd(&raise, &start, &start);
    PSVECAdd(&raise, &end, &end);
    dDbVw_drawLineXlu(start, end, color, 1, 12);
    
    return 0;
}

void CaptPoly(dBgS_CaptPoly& i_captpoly) {
    dComIfG_Bgsp().CaptPoly(i_captpoly);
}

KEEP_FUNC void GZ_drawPolygons() {
    if (g_collisionFlags[VIEW_POLYGON_GROUND].active || g_collisionFlags[VIEW_POLYGON_WALL].active || g_collisionFlags[VIEW_POLYGON_ROOF].active) {
        daAlink_c* player = (daAlink_c*)dComIfGp_getPlayer(0);

        if (player != NULL) {
            Vec* base_pos = &player->current.pos;
            if (g_freeCamEnabled) {
                base_pos = &dComIfGp_getCamera(0)->mCamera.mViewCache.mEye;
            }

            cM3dGAab aab;
			cXyz min;
			cXyz max;

			f32 range = (f32)g_collisionRange;
			min.set(base_pos->x - range, base_pos->y - range, base_pos->z - range);
			max.set(base_pos->x + range, base_pos->y + range, base_pos->z + range);
			aab.mMin = min;
			aab.mMax = max;

            dBgS_CaptPoly poly_capt;
			static_cast<dBgS_GrpPassChk*>(poly_capt.GetGrpPassChkInfo())->OnFullGrp();
            poly_capt.Set(min, max);

            // draw edges
            if (g_collisionFlags[VIEW_POLYGON_EDGES].active) {
                poly_capt.SetCallback(poly_edge_draw);
			    CaptPoly(poly_capt);
            }
    
            // draw poly
			poly_capt.SetCallback(poly_draw);
			CaptPoly(poly_capt);
        }
    }    
}