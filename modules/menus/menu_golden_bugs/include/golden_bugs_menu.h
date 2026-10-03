#include "menus/menu.h"

struct GoldenBugData {
    uint8_t l_mAntIdx;
    uint8_t l_fAntIdx;
    uint8_t l_mBeetleIdx;
    uint8_t l_fBeetleIdx;
    uint8_t l_mButterflyIdx;
    uint8_t l_fButterflyIdx;
    uint8_t l_mDayflyIdx;
    uint8_t l_fDayflyIdx;
    uint8_t l_mDragonflyIdx;
    uint8_t l_fDragonflyIdx;
    uint8_t l_mGrasshopperIdx;
    uint8_t l_fGrasshopperIdx;
    uint8_t l_mLadybugIdx;
    uint8_t l_fLadybugIdx;
    uint8_t l_mMantisIdx;
    uint8_t l_fMantisIdx;
    uint8_t l_mPhasmidIdx;
    uint8_t l_fPhasmidIdx;
    uint8_t l_mPillBugIdx;
    uint8_t l_fPillBugIdx;
    uint8_t l_mSnailIdx;
    uint8_t l_fSnailIdx;
    uint8_t l_mStagBeetleIdx;
    uint8_t l_fStagBeetleIdx;
};

enum BugIndex {
    M_ANT_INDEX,
    F_ANT_INDEX,
    M_BEETLE_INDEX,
    F_BEETLE_INDEX,
    M_BUTTERFLY_INDEX,
    F_BUTTERFLY_INDEX,
    M_DAYFLY_INDEX,
    F_DAYFLY_INDEX,
    M_DRAGONFLY_INDEX,
    F_DRAGONFLY_INDEX,
    M_GRASSHOPPER_INDEX,
    F_GRASSHOPPER_INDEX,
    M_LADYBUG_INDEX,
    F_LADYBUG_INDEX,
    M_MANTIS_INDEX,
    F_MANTIS_INDEX,
    M_PHASMID_INDEX,
    F_PHASMID_INDEX,
    M_PILL_BUG_INDEX,
    F_PILL_BUG_INDEX,
    M_SNAIL_INDEX,
    F_SNAIL_INDEX,
    M_STAG_BEETLE_INDEX,
    F_STAG_BEETLE_INDEX,
    
    GB_INDEX_COUNT
};

extern GoldenBugData* gbData;

class GoldenBugMenu : public Menu {
public:
    GoldenBugMenu(Cursor&);
    virtual ~GoldenBugMenu();
    virtual void draw();

private:
    uint8_t getBugIdx(u8 bug, u16 flag);
    void setBugIdx(u8 bug, u8 idx, u16 flag);
    void setBugs();
};