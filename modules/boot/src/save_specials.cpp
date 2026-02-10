#include "save_specials.h"
#include "gz_flags.h"
#include "libtp_c/include/utils.h"
#include "libtp_c/include/d/com/d_com_inf_game.h"
#include "libtp_c/include/f_op/f_op_actor_mng.h"
#include "libtp_c/include/f_op/f_op_actor_iter.h"
#include "libtp_c/include/rel/d/a/b/d_a_b_ds.h"
#include "libtp_c/include/d/a/d_a_e_zs.h"
#include "libtp_c/include/rel/d/a/obj/d_a_obj_lv4sand.h"
#include "libtp_c/include/d/d_procname.h"
#include "rels/include/defines.h"

typedef bool (*predicate_t)(fopAc_ac_c&);

fopAc_ac_c* find_actor(predicate_t const& predicate) {
    if (predicate == nullptr) {
        return nullptr;
    }
    node_class* node = g_fopAcTg_Queue.mpHead;
    fopAc_ac_c* actorData = NULL;
    for (int i = 0; i < g_fopAcTg_Queue.mSize; i++) {
        if (node != NULL) {
            create_tag_class* tag = (create_tag_class*)node;
            fopAc_ac_c* tmpData = (fopAc_ac_c*)tag->mpTagData;
            if (predicate(*tmpData)) {
                actorData = tmpData;
                break;
            }
            node = node->mpNextNode;
        }
    }
    return actorData;
}

#if defined(WII_NTSCU_10) || defined(WII_PAL)
#define ROCK_ID 763
#else
#define ROCK_ID 765
#endif

KEEP_FUNC void SaveMngSpecial_OrdonRock() {
    gSaveManager.setSaveAngle(32768);
    gSaveManager.setSavePosition(400.0f, 307.5f, -11270.2f);
    gSaveManager.setLinkInfo();

    cXyz position(400.0f, 307.8f, -11365.f);

    fopAc_ac_c* actorData = find_actor([](fopAc_ac_c& act) {
        return act.mBase.mProcName == ROCK_ID && act.mBase.mParameters == 0x00FF6511;
    });

    if (actorData != NULL) {
        actorData->current.pos = position;
        actorData->shape_angle.y = 5880;
    }
}

KEEP_FUNC void SaveMngSpecial_BossFlags() {
    gSaveManager.injectDefault_during();
    bossFlags = 0xFF;
}

KEEP_FUNC void SaveMngSpecial_Goats1() {
    gSaveManager.injectDefault_during();
    setNextStageLayer(5);
}

KEEP_FUNC void SaveMngSpecial_Hugo() {
    gSaveManager.injectDefault_during();
    dComIfGs_onSwitch(47, 0);   // midna trigger off
    dComIfGs_offSwitch(63, 0);  // hugo alive
}

#if defined(WII_NTSCU_10) || defined(WII_PAL)
#define HUGO_ACTOR_ID 466
#else
#define HUGO_ACTOR_ID 468
#endif

KEEP_FUNC void SaveMngSpecial_SpawnHugo() {
    gSaveManager.setSaveAngle(40166);
    gSaveManager.setSavePosition(2.9385, 396.9580, -18150.087);
    gSaveManager.setLinkInfo();

    cXyz position(-289.9785, 401.5400, -18533.078);

    // Find hugo in the actor list
    fopAc_ac_c* actorData =
        find_actor([](auto& act) { return act.mBase.mProcName == HUGO_ACTOR_ID; });

    if (actorData != NULL) {
        actorData->current.pos = position;
        actorData->shape_angle.y = 5880;
    }
}

#if defined(WII_NTSCU_10) || defined(WII_PAL)
#define BUBBLE_ACTOR_ID 489
#else
#define BUBBLE_ACTOR_ID 491
#endif

KEEP_FUNC void SaveMngSpecial_PurpleMist() {
    gSaveManager.injectDefault_during();
    dComIfGs_setTransformStatus(STATUS_HUMAN);
}

KEEP_FUNC void SaveMngSpecial_ForestBit() {
    gSaveManager.injectDefault_during();
    dComIfGp_getPlayer()->mNoResetFlg2 |= 1; // lantern out
    daAlink_c__swordEquip(dComIfGp_getPlayer(), 0); // sword out
    gSaveManager.setSaveAngle(32768);
    gSaveManager.setSavePosition(45.0f, 1911.1345f, 20425.0f);
    gSaveManager.setLinkInfo();
}

KEEP_FUNC void SaveMngSpecial_KargOoB() {
    gSaveManager.mPracticeFileOpts.inject_options_before_load = nullptr;
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 0xA;  // spawn on kargorok
    dComIfGs_setTransformStatus(STATUS_HUMAN);
}

KEEP_FUNC void SaveMngSpecial_WaterfallSidehop() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastSpeedF = 10.0f;  // link spawns swimming forward
}

KEEP_FUNC void SaveMngSpecial_EarlyEle() {
    gSaveManager.injectDefault_during();
    dComIfGs_onTmpBit(0x0002);
}

KEEP_FUNC void SaveMngSpecial_EarlyEleSpawn() {
    gSaveManager.setSaveAngle(43917);
    gSaveManager.setSavePosition(1130, -355.6, -5569);
    gSaveManager.setLinkInfo();
}

KEEP_FUNC void SaveMngSpecial_HorseSpawn() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 1;  // spawn on epona
}

KEEP_FUNC void SaveMngSpecial_EldinCollection() {
    SaveMngSpecial_HorseSpawn();
    g_dComIfG_gameInfo.info.mRestart.mLastSpeedF = 42.0f;
}

KEEP_FUNC void SaveMngSpecial_KB2Skip() {
    gSaveManager.injectDefault_during();
    setNextStageLayer(3);
}

KEEP_FUNC void SaveMngSpecial_Escort() {
    gSaveManager.injectDefault_during();
    setNextStageRoom(0xD);
    setNextStagePoint(98);
    setNextStageLayer(2);
}

KEEP_FUNC void SaveMngSpecial_EscortKeys() {
    dComIfGs_setKeyNum(2);  // give 2 keys for field gates
}

KEEP_FUNC void SaveMngSpecial_Dangoro() {
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x200000;  // turn off intro cs, start fight
}

KEEP_FUNC void SaveMngSpecial_Norgor() {
    g_meter2_info.mRentalBombBag = 0;  // Rental Bomb Bag Idx set to bag 0
    dComIfGs_setItem(SLOT_15, NORMAL_BOMB);
    dComIfGs_setBombNum(0, 30);
    dComIfGs_setSelectItemIndex(SELECT_ITEM_Y, SLOT_15);
}

KEEP_FUNC void SaveMngSpecial_LakebedBKSkip() {
    gSaveManager.injectDefault_during();
    dComIfGs_onSwitch(2, 0);    // bridge turned
    dComIfGs_onSwitch(122, 0);  // dungeon intro cs off
}

KEEP_FUNC void SaveMngSpecial_Darkhammer() {
    dComIfGs_onEventBit(0x0B02);
    dComIfGs_onEventBit(0x0B04);  // iza bomb bag stolen
}

KEEP_FUNC void SaveMngSpecial_Morpheel() {
    dComIfGp_getPlayer()->mEquipItem = HOOKSHOT;                        // clawshot
    dComIfGp_getPlayer()->onNoResetFlg0(daPy_py_c::FLG0_EQUIP_HVY_BOOTS);  // ib
    gSaveManager.setSaveAngle(10754);
    gSaveManager.setSavePosition(-1193.0f, -23999.0f, -770.0f);
    gSaveManager.setLinkInfo();
}

KEEP_FUNC void SaveMngSpecial_Iza1Skip() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 0xA;  // spawn on kargorok
    setNextStageName("F_SP112");                       // set stage to river
    setNextStageRoom(1);
    setNextStagePoint(0);
    setNextStageLayer(4);
}

KEEP_FUNC void SaveMngSpecial_AnyPlummOoB() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 0xA;  // spawn on kargorok
    setNextStageName("F_SP112");                       // set stage to river
    setNextStageRoom(1);
    setNextStagePoint(0);
    setNextStageLayer(4);
    bossFlags = 0xFF;
}

KEEP_FUNC void SaveMngSpecial_Stallord() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x300000;  // turn off intro cs, start fight
    setNextStagePoint(1);                                          // spawn at in front of stally
}

KEEP_FUNC void SaveMngSpecial_Stallord2() {
    daB_DS_c* stallord = (daB_DS_c*)fopAcM_SearchByName(PROC_B_DS);

    // create the phase 2 version of stallord
    fopAcM_create(PROC_B_DS, fopAcM_GetParam(stallord) | 2, &stallord->current.pos,
                fopAcM_GetRoomNo(stallord), nullptr, nullptr, -1);
    fopAcM_delete(stallord);  // delete phase 1 stallord

    daObjLv4Wall_c* rwall = (daObjLv4Wall_c*)fopAcM_SearchByName(PROC_Obj_Lv4RailWall);
    daObjSwSpinner_c* spinnersw = (daObjSwSpinner_c*)fopAcM_SearchByName(PROC_Obj_SwSpinner);

    spinnersw->mRotSpeedY = 3000;  // set arena spinner switch to max speed
    rwall->field_0x954 = 101;  // set spinner switch speed counter to threshold
    rwall->mHeight = 3370.0f;  // set arena height to max
}

KEEP_FUNC void SaveMngSpecial_Stallord2_init() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x300000;  // turn off intro cs, start fight
    setNextStagePoint(1);                                          // spawn at in front of stally
}

KEEP_FUNC void SaveMngSpecial_StallordCad() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x300000;  // turn off intro cs, start fight
    setNextStagePoint(1);                                          // spawn at in front of stally
    gSaveManager.setSaveAngle(60562);
    gSaveManager.setSavePosition(327.221771f, 1800.0f, -4990.98975f);
    gSaveManager.setLinkInfo();

    // Find joseph in the actor list
    fopAc_ac_c* actorData1 =
        find_actor([](auto& act) { return act.mBase.mProcName == PROC_E_ZS && (int) act.current.pos.x == -920; });

    daE_ZS_c* joseph = (daE_ZS_c*) actorData1;
    joseph->mAction = 1; // not functionally important, slouching idle with both arms down
    joseph->mMode = 0; // not functionally important, slouching idle with both arms down

    // cXyz posi (327.221771, 1800.0, -4990.98975);
    // joseph->current.pos = posi;
    daE_ZS_c__setBck_void_(joseph, 9, 2, 3.0f, 1.0f); // rise up animation
    joseph->field_0x65c = 0; // something something vertical matrix
    joseph->field_0x673 = 1; // visibility?
    joseph->mCyl.mGObjInf.OnTgSetBit(); // hitbox
    joseph->mCyl.mGObjInf.OnCoSetBit(); // push collider
    joseph->mStatus |= 0x200000; // clawshottable
}

KEEP_FUNC void SaveMngSpecial_StallordWallClip() {
    gSaveManager.injectDefault_during();
    dComIfGs_setSelectItemIndex(SELECT_ITEM_X, SLOT_0); // rang on x
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x300000;  // turn off intro cs, start fight
    setNextStagePoint(1);                                          // spawn at in front of stally
    gSaveManager.setSaveAngle(32741);
    gSaveManager.setSavePosition(356.972076f, 1775.0f, -4439.97656f);
    gSaveManager.setLinkInfo();

    // Find joseph in the actor list
    fopAc_ac_c* actorData1 =
        find_actor([](auto& act) { return act.mBase.mProcName == PROC_E_ZS && (int) act.current.pos.x == -920; });

    daE_ZS_c* joseph = (daE_ZS_c*) actorData1;
    joseph->mAction = 1; // not functionally important, slouching idle with both arms down
    joseph->mMode = 0; // not functionally important, slouching idle with both arms down
    
    daE_ZS_c__setBck_void_(joseph, 9, 2, 3.0f, 1.0f); // rise up animation
    joseph->field_0x65c = 0; // something something vertical matrix
    joseph->field_0x673 = 1; // visibility?
    joseph->mCyl.mGObjInf.OnTgSetBit(); // hitbox
    joseph->mCyl.mGObjInf.OnCoSetBit(); // push collider
    joseph->mStatus |= 0x200000; // clawshottable
    cXyz corner (301.9, 1800.0, -4966.0);
    joseph->current.pos = corner;
}

KEEP_FUNC void SaveMngSpecial_StallordBombBoost() {
    gSaveManager.injectDefault_during();
    dComIfGs_setSelectItemIndex(SELECT_ITEM_X, SLOT_0); // rang on x
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x300000;  // turn off intro cs, start fight
    setNextStagePoint(1);                                          // spawn at in front of stally
    gSaveManager.setSaveAngle(32097);
    gSaveManager.setSavePosition(623.792603f, 1800.0f, -4914.61377f);
    gSaveManager.setLinkInfo();

    // Find joseph in the actor list
    fopAc_ac_c* actorData1 =
        find_actor([](auto& act) { return act.mBase.mProcName == PROC_E_ZS && (int) act.current.pos.x == -920; });

    daE_ZS_c* joseph = (daE_ZS_c*) actorData1;
    joseph->mAction = 1; // not functionally important, slouching idle with both arms down
    joseph->mMode = 0; // not functionally important, slouching idle with both arms down
    
    daE_ZS_c__setBck_void_(joseph, 9, 2, 3.0f, 1.0f); // rise up animation
    joseph->field_0x65c = 0; // something something vertical matrix
    joseph->field_0x673 = 1; // visibility?
    joseph->mCyl.mGObjInf.OnTgSetBit(); // hitbox
    joseph->mCyl.mGObjInf.OnCoSetBit(); // push collider
    joseph->mStatus |= 0x200000; // clawshottable
    cXyz corner (301.9, 1800.0, -4966.0);
    joseph->current.pos = corner;
}

#if defined(WII_NTSCU_10) || defined(WII_PAL)
#define ZANT_ACTOR_ID 247
#else
#define ZANT_ACTOR_ID 249
#endif

KEEP_FUNC void SaveMngSpecial_ZantFinal() {
    class daB_ZANT_c {
    public:
        /* 0x0000 */ fopEn_enemy_c base;
        /* 0x05AC */ u8 field_0x5ac[0x6d4 - 0x5ac];
        /* 0x06D4 */ int mAction;
        /* 0x06D8 */ int field_0x6d8;
        /* 0x06DC */ int mMode;
        /* 0x06E0 */ u8 field_0x6e0[0x1b];
        /* 0x06FB */ u8 mFightPhase;
    };

    // Find zant in the actor list
    daB_ZANT_c* actorData = (daB_ZANT_c*)find_actor([](auto& act) { return act.mBase.mProcName == PROC_B_ZANT; });

    // Set his action, fight phase and mode to trigger the transition demo
    if (actorData != nullptr) {
        // Set Zant's state
        actorData->mAction = 23;      // ACT_ROOM_CHANGE
        actorData->mFightPhase = 5;   // PHASE_YO
        actorData->mMode = 0;         // MODE_START_DEMO
    }

    daAlink_c__swordEquip(dComIfGp_getPlayer(), 0); // sword out
}

KEEP_FUNC void SaveMngSpecial_SPRBossKey() {
    gSaveManager.injectDefault_during();
    setNextStageRoom(0xB);  // boss key room
    setNextStagePoint(0);   // default spawn
}

KEEP_FUNC void SaveMngSpecial_ToTEarlyPoe() {
    gSaveManager.injectDefault_during();
    gSaveManager.setSaveAngle(49299);
    gSaveManager.setSavePosition(-2462.85f, 2750.0f, -7.10f);
    gSaveManager.setLinkInfo();
}

KEEP_FUNC void SaveMngSpecial_ToTEarlyHP() {
    gSaveManager.injectDefault_during();
    gSaveManager.setSaveAngle(0);
    gSaveManager.setSavePosition(-6626.f, 5250.0f, -5587.f);
    gSaveManager.setLinkInfo();
    dComIfGs_onSwitch(224, 4);  // gate moved to correct pos
}

KEEP_FUNC void SaveMngSpecial_HugoArchery() {
    gSaveManager.injectDefault_during();
    // g_dComIfG_gameInfo.temp_flags.flags[14] = 0xC0;  // start archery minigame
}

KEEP_FUNC void SaveMngSpecial_CityPoeCycle() {
    gSaveManager.injectDefault_during();
    gSaveManager.setSaveAngle(0);
    gSaveManager.setSavePosition(-13990.0f, 3000.0f, -16200.0f);
    gSaveManager.setLinkInfo();
}

KEEP_FUNC void SaveMngSpecial_FanTower() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mDan.mSwitch[0] = 0;  // reset city switches
}

KEEP_FUNC void SaveMngSpecial_Argorok() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x10000;
}

KEEP_FUNC void SaveMngSpecial_Palace1() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mDan.mSwitch[0] = 0;  // reset palace switches
}

KEEP_FUNC void SaveMngSpecial_Palace2() {
    gSaveManager.injectDefault_during();
    daAlink_c__swordEquip(dComIfGp_getPlayer(), 0); // sword out
    gSaveManager.setSaveAngle(32731);
    gSaveManager.setSavePosition(251.83f, 1400.0f, 584.0f);
    gSaveManager.setLinkInfo();
}

KEEP_FUNC void SaveMngSpecial_CaveOfOrdeals() {
    gSaveManager.injectDefault_during();
    g_dComIfG_gameInfo.info.mDan.mSwitch[0] = 0;
}

KEEP_FUNC void BeastGanonSpecial_setLayer() {
    gSaveManager.injectDefault_during();
    setNextStageLayer(1);
}

KEEP_FUNC void SaveMngSpecial_emptyLake() {
    gSaveManager.injectDefault_during();
    setNextStageLayer(4);
    bossFlags = 0xFF;
}

KEEP_FUNC void SaveMngSpecial_Aeralfos() {
    gSaveManager.injectDefault_during();
    dComIfGp_getPlayer()->mEquipItem = HOOKSHOT; // claw out (doesn't show as out, but still is for this intention, which is that b will slash)
}

KEEP_FUNC void SaveMngSpecial_NoSQAeralfos() {
    gSaveManager.injectDefault_during();
    dComIfGs_setLife(4);  // one heart
}

KEEP_FUNC void SaveMngSpecial_DeathSword() {
    gSaveManager.injectDefault_during();
    dComIfGs_setLife(2);  // half heart
}

KEEP_FUNC void SaveMngSpecial_ArgorokCSSkip() {
    gSaveManager.injectDefault_during();
    daAlink_c__swordEquip(dComIfGp_getPlayer(), 0); // sword out
    gSaveManager.setSaveAngle(16267);
    gSaveManager.setSavePosition(-10406.9248f, 0.0f, -12446.7979f);
    gSaveManager.setLinkInfo();
}

KEEP_FUNC void SaveMngSpecial_PalaceBossKey() {
    gSaveManager.injectDefault_during();
    daAlink_c__swordEquip(dComIfGp_getPlayer(), 0); // sword out
}

KEEP_FUNC void SaveMngSpecial_EarlyPlatform() {
    gSaveManager.injectDefault_during();
    daAlink_c__swordEquip(dComIfGp_getPlayer(), 0); // sword out
}

KEEP_FUNC void SaveMngSpecial_ZantDangoro() {
    gSaveManager.injectDefault_during();
    daAlink_c__swordEquip(dComIfGp_getPlayer(), 0); // sword out
    dComIfGs_setSelectItemIndex(SELECT_ITEM_X, SLOT_0); // rang on y
    gSaveManager.setSaveAngle(0);
    gSaveManager.setSavePosition(0.0f, -500.0f, 0.0f);
    gSaveManager.setLinkInfo();
}

KEEP_FUNC void SaveMngSpecial_KB4() {
    gSaveManager.injectDefault_during();
    daAlink_c__swordEquip(dComIfGp_getPlayer(), 0); // sword out
    gSaveManager.setSaveAngle(24548);
    gSaveManager.setSavePosition(-8567.7666f, 52.0f, -4893.16992f);
    gSaveManager.setSaveCamera(-8804.90332f, 225.034332f, -4658.11719f, -8567.05664f, 160.653625f, -4893.875f); // does not work
    gSaveManager.setPositionCamera();
}
