#ifndef WII_PLATFORM
#include <cstring>
#include "../../../../tp/libs/dolphin/src/card/__card.h"

extern "C" {

s32 __CARDFreeBlock(s32 chan, u16 nBlock, CARDCallback callback) {
    CARDControl* card = &__CARDBlock[chan];
    if (!card->attached)
        return CARD_RESULT_NOCARD;

    u16* fat = (u16*)__CARDGetFatBlock(card);
    while (nBlock != 0xFFFF) {
        if (!CARDIsValidBlockNo(card, nBlock))
            return CARD_RESULT_BROKEN;

        u16 nextBlock = fat[nBlock];
        fat[nBlock] = 0;
        nBlock = nextBlock;
        ++fat[3];
    }

    return __CARDUpdateFatBlock(chan, fat, callback);
}

s32 __CARDGetFileNo(CARDControl* card, const char* fileName, s32* pfileNo) {
    if (!card->attached)
        return CARD_RESULT_NOCARD;

    CARDDir* dir = __CARDGetDirBlock(card);
    for (s32 fileNo = 0; fileNo < CARD_MAX_FILE; fileNo++) {
        CARDDir* ent = &dir[fileNo];
        if (__CARDAccess(card, ent) < 0)
            continue;
        if (__CARDCompareFileName(ent, fileName)) {
            *pfileNo = fileNo;
            return CARD_RESULT_READY;
        }
    }

    return CARD_RESULT_NOFILE;
}

static void DeleteCallback(s32 chan, s32 result) {
    CARDControl* card = &__CARDBlock[chan];
    CARDCallback callback = card->apiCallback;
    card->apiCallback = NULL;

    if (result >= 0) {
        result = __CARDFreeBlock(chan, card->startBlock, callback);
        if (result >= 0)
            return;
    }

    __CARDPutControlBlock(card, result);
    if (callback)
        callback(chan, result);
}

s32 CARDDeleteAsync(s32 chan, const char* fileName, CARDCallback callback) {
    CARDControl* card;
    s32 fileNo;

    s32 result = __CARDGetControlBlock(chan, &card);
    if (result < 0)
        return result;
    result = __CARDGetFileNo(card, fileName, &fileNo);
    if (result < 0)
        return __CARDPutControlBlock(card, result);

    CARDDir* ent = &__CARDGetDirBlock(card)[fileNo];
    card->startBlock = ent->startBlock;
    memset(ent, 0xff, sizeof(CARDDir));

    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;
    result = __CARDUpdateDir(chan, DeleteCallback);
    if (result < 0)
        __CARDPutControlBlock(card, result);
    return result;
}

s32 CARDDelete(s32 chan, const char* fileName) {
    s32 result = CARDDeleteAsync(chan, fileName, __CARDSyncCallback);
    if (result < 0)
        return result;

    return __CARDSync(chan);
}
}
#endif
