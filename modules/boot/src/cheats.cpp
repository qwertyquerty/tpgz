#include "f_op/f_op_actor_tag.h"
#include "d/actor/d_a_alink.h"
#include "cheats.h"
#include "commands.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_actor_mng.h"
#include "f_pc/f_pc_name.h"
#include "d/actor/d_a_e_zs.h"
#include "d/actor/d_a_e_s1.h"
#include "d/d_item.h"
#include "rels/include/patch.h"
#include "rels/include/defines.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"
#include "fifo_queue.h"

static u8* zsField(daE_ZS_c* zs, u32 gcnOffset) {
    return reinterpret_cast<u8*>(zs) + sizeof(fopEn_enemy_c) + (gcnOffset - 0x5AC);
}

#ifdef GCN_PLATFORM
#define INVINCIBLE_ENEMIES_OFFSET (0x328)
#define cc_at_check cc_at_check__FP10fopAc_ac_cP11dCcU_AtInfo
#endif
#ifdef WII_PLATFORM
#define INVINCIBLE_ENEMIES_OFFSET (0x244)
#define cc_at_check cc_at_check__FP10fopAc_ac_cP11dCcU_AtInfo
#endif

extern "C" {
void* cc_at_check(void*, void*);
}

bool l_doorCollision;

void GZ_applyCheats() {
    if (GZ_checkCheat(STNG_CHEATS_INVINCIBLE_ENEMIES)) {
        /* Patch cc_at_check instruction to nop out health subtraction */
        *reinterpret_cast<uint32_t*>((uint32_t)(&cc_at_check) + INVINCIBLE_ENEMIES_OFFSET) =
            0x60000000;  // nop
        DCFlushRange((void*)((uint32_t)(&cc_at_check) + INVINCIBLE_ENEMIES_OFFSET),
                     sizeof(uint32_t));
        ICInvalidateRange((void*)((uint32_t)(&cc_at_check) + INVINCIBLE_ENEMIES_OFFSET),
                          sizeof(uint32_t));

        /* Special handling for any enemy that doesn't use cc_at_check */
        node_class* node = g_fopAcTg_Queue.mpHead;
        for (int i = 0; i < g_fopAcTg_Queue.mSize; i++) {
            if (node != NULL) {
                create_tag_class* tag = (create_tag_class*)node;
                fopEn_enemy_c* actor = (fopEn_enemy_c*)tag->mpTagData;

                if (actor != NULL) {
                    switch (fopAcM_GetName(actor)) {
                    case fpcNm_E_ZS_e: {
                        daE_ZS_c* zs = static_cast<daE_ZS_c*>(actor);

                        // if action is damage action
                        if (*reinterpret_cast<int*>(zsField(zs, 0x660)) == 2) {
                            zs->setActionMode(1, 0);

                            reinterpret_cast<dCcD_Cyl*>(zsField(zs, 0x8C8))->GetGObjInf()->OnTgSetBit();  // turn back on hit collision
                            reinterpret_cast<dCcD_Cyl*>(zsField(zs, 0x8C8))->GetGObjInf()->OnCoSetBit();  // turn back on push collision
                            zs->health = 20;                // reset health back to max
                        }
                        break;
                    }
                    case fpcNm_E_S1_e: {
                        e_s1_class* s1 = static_cast<e_s1_class*>(actor);
                        s1->health = 50;

                        if (s1->mMode == 9 || s1->mMode == 5 || s1->mMode == 10) {
                            s1->mMode = 0;  // reset action back to idle if in damage/fail action
                        }
                        break;
                    }
                    }
                }
            }
            node = node->mpNextNode;
        }
    } else {
        /* Unpatch cc_at_check instruction to restore health subtraction */
        *reinterpret_cast<uint32_t*>((uint32_t)(&cc_at_check) + INVINCIBLE_ENEMIES_OFFSET) =
            0x7C030050;  // sub r0, r0, r3
        DCFlushRange((void*)((uint32_t)(&cc_at_check) + INVINCIBLE_ENEMIES_OFFSET),
                     sizeof(uint32_t));
        ICInvalidateRange((void*)((uint32_t)(&cc_at_check) + INVINCIBLE_ENEMIES_OFFSET),
                          sizeof(uint32_t));
    }

    if (GZ_checkCheat(STNG_CHEATS_INVINCIBLE)) {
        daAlink_c* player = (daAlink_c*)dComIfGp_getPlayer(0);
        if (player != NULL) {
            for (int i = 0; i < 3; i++) {
                player->mTgCyls[i].GetGObjInf()->OffTgSetBit();
                player->mTgCyls[i].ResetTgHit();
            }

            if (player->checkWolf()) {
                player->mAtSph.GetGObjInf()->OffTgSetBit();
                player->mAtSph.ResetTgHit();
            }
        }
    }

    if (GZ_checkCheat(STNG_CHEATS_INFINITE_HEARTS)) {
        uint16_t max_life = dComIfGs_getMaxLife();
        dComIfGs_setLife((max_life / 5) * 4);
    }

    if (GZ_checkCheat(STNG_CHEATS_INFINITE_AIR)) {
        dComIfGp_setOxygen(600);
    }

    if (GZ_checkCheat(STNG_CHEATS_INFINITE_OIL)) {
        dComIfGs_setOil(21600);
    }

    if (GZ_checkCheat(STNG_CHEATS_INFINITE_BOMBS)) {
        dComIfGs_setBombNum(0, 99);
        dComIfGs_setBombNum(1, 99);
        dComIfGs_setBombNum(2, 99);
    }

    if (GZ_checkCheat(STNG_CHEATS_INFINITE_RUPEES)) {
        dComIfGs_setRupee(1000);
    }

    if (GZ_checkCheat(STNG_CHEATS_INFINITE_ARROWS)) {
        dComIfGs_setArrowNum(99);
    }

    if (GZ_checkCheat(STNG_CHEATS_INFINITE_SLINGSHOT)) {
        dComIfGs_setPachinkoNum(99);
    }

    if (GZ_checkCheat(STNG_CHEATS_SUPER_CLAWSHOT)) {
        const_cast<daAlinkHIO_hookshot_c1&>(daAlinkHIO_hookshot_c0::m).mShootSpeed = 2870.0f;
        const_cast<daAlinkHIO_hookshot_c1&>(daAlinkHIO_hookshot_c0::m).mMaxLength = 69420.0f;
        const_cast<daAlinkHIO_hookshot_c1&>(daAlinkHIO_hookshot_c0::m).mReturnSpeed = 2870.0f;
        const_cast<daAlinkHIO_hookshot_c1&>(daAlinkHIO_hookshot_c0::m).mStickReturnSpeed = 500.0f;
    } else {
        const_cast<daAlinkHIO_hookshot_c1&>(daAlinkHIO_hookshot_c0::m).mShootSpeed = 100.0f;
        const_cast<daAlinkHIO_hookshot_c1&>(daAlinkHIO_hookshot_c0::m).mMaxLength = 2000.0f;
        const_cast<daAlinkHIO_hookshot_c1&>(daAlinkHIO_hookshot_c0::m).mReturnSpeed = 150.0f;
        const_cast<daAlinkHIO_hookshot_c1&>(daAlinkHIO_hookshot_c0::m).mStickReturnSpeed = 60.0f;
    }

    if (GZ_checkCheat(STNG_CHEATS_DOOR_STORAGE)) {
        if (dComIfGp_getPlayer(0)) {
            ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.SetWallNone();
            ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.OnLineCheckNone();
            l_doorCollision = true;
        }
    } else {
        if (dComIfGp_getPlayer(0) && l_doorCollision) {
            ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.ClrWallNone();
            ((daAlink_c*)dComIfGp_getPlayer(0))->mLinkAcch.OffLineCheckNone();
            l_doorCollision = false;
        }
    }

    if (GZ_checkCheat(STNG_CHEATS_DISABLE_ITEM_TIMER)) {
        const_cast<daItemBase_data&>(daItemBase_c::m_data).mWaitTime = 0x7FFF;
    } else {
        const_cast<daItemBase_data&>(daItemBase_c::m_data).mWaitTime = 240;
    }

#ifdef WII_PLATFORM
    if (GZ_checkCheat(STNG_CHEATS_GALE_LJA)) {
        if (dComIfGp_getPlayer(0) && ((daAlink_c*)dComIfGp_getPlayer(0))->mProcID == 0x60 &&
            ((daAlink_c*)dComIfGp_getPlayer(0))->mEquipItem == dItemNo_NONE_e) {
            ((daAlink_c*)dComIfGp_getPlayer(0))->mEquipItem = 0x0103;
        }
    }
#endif
}
