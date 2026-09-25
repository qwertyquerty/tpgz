#include "features/savestate/include/savestate.h"
#include <cstdio>
#include <cstring>
#include "fifo_queue.h"
#include "game_state.h"
#include "global_data.h"
#include "modules.h"
#include "utils/mem2.h"
#include "menus/utils/menu_mgr.h"
#include "utils/texture.h"
#include "rels/include/cxx.h"
#include "rels/include/defines.h"
#include "d/d_com_inf_game.h"
#include "m_Do/m_Do_dvd_thread.h"
#include "JSystem/JKernel/JKRExpHeap.h"
#include "JSystem/JKernel/JKRThread.h"
#include "JSystem/JKernel/JKRDisposer.h"
#include "JSystem/JAudio2/JASAudioThread.h"
#include "JSystem/JAudio2/JASTaskThread.h"
#include "JSystem/JAudio2/JASDvdThread.h"
#include "JSystem/JAudio2/JAIStreamMgr.h"
#include "JSystem/JAudio2/JAISeMgr.h"
#include "JSystem/JAudio2/JAISeqMgr.h"
#include "JSystem/JAudio2/JASDriverIF.h"
#include "Z2AudioLib/Z2SoundMgr.h"
#include "Z2AudioLib/Z2SceneMgr.h"
#include "os/OSCache.h"
#include "os/OSModule.h"
#include <os.h>
#include <dvd.h>
#include <gx.h>
#include <vi.h>

struct SaveStateRange {
    u32 start;
    u32 end;
};

#include "savestate_ranges.h"

extern JKRExpHeap* gameHeap;
extern JKRExpHeap* zeldaHeap;
extern JKRExpHeap* archiveHeap;
extern JKRExpHeap* j2dHeap;
extern JKRExpHeap* commandHeap;

extern Texture l_framePauseTex;
extern Texture l_framePlayTex;

#define MEM2_MARGIN 0x100000
#define MAX_WAIT_FRAMES 120
#define GP_IDLE_POLLS 100000
#define MAX_AUDIO_DRAIN_SUBFRAMES 240
#define MAX_OUTSIDE_MODULES 8
#define JKRTHREAD_MESSAGE_QUEUE_OFFSET 0x30
#define DVD_THREAD_COMMAND_LIST_OFFSET 0x24
#define Z2_SCENE_WAVES_OFFSET 0x0D
#define Z2_SCENE_WAVES_SIZE 10
#define OS_ACTIVE_THREAD_QUEUE 0x800000DC
#define OS_MODULE_LIST 0x800030C8

#ifdef GCN_PLATFORM
#define PAD_HOLD mDoCPd_c::getCpadInfo(0).mButtonFlags
#define PAD_TRIG mDoCPd_c::getCpadInfo(0).mPressedButtonFlags
#else
#define PAD_HOLD GZ_getWiiPadStatus().hold
#define PAD_TRIG GZ_getWiiPadStatus().trig
#endif

enum SaveStateAction {
    SS_ACTION_NONE,
    SS_ACTION_SAVE,
    SS_ACTION_LOAD,
};

struct ModuleLink {
    OSModuleInfo* module;
    OSModuleInfo* next;
    OSModuleInfo* prev;
};

struct SaveStateHeader {
    u32 totalSize;
    u32 segmentCount;
    u32 preservedCount;
    char stageName[8];
    u8 sceneWaves[Z2_SCENE_WAVES_SIZE];
    u32 outsideModuleCount;
    ModuleLink outsideModules[MAX_OUTSIDE_MODULES];
};

struct SaveStateSegment {
    u32 addr;
    u32 size;
};

struct RangeList {
    RangeList() : items(NULL), count(0), capacity(0) {}
    ~RangeList() { delete[] items; }

    bool push(u32 start, u32 end) {
        if (end <= start) {
            return true;
        }
        if (count == capacity) {
            u32 newCapacity = capacity == 0 ? 64 : capacity * 2;
            SaveStateRange* newItems = new SaveStateRange[newCapacity];
            if (newItems == NULL) {
                return false;
            }
            memcpy(newItems, items, count * sizeof(SaveStateRange));
            delete[] items;
            items = newItems;
            capacity = newCapacity;
        }
        items[count].start = start;
        items[count].end = end;
        count++;
        return true;
    }

    void sortMerge() {
        for (u32 gap = count / 2; gap > 0; gap /= 2) {
            for (u32 i = gap; i < count; i++) {
                SaveStateRange tmp = items[i];
                u32 j = i;
                for (; j >= gap && items[j - gap].start > tmp.start; j -= gap) {
                    items[j] = items[j - gap];
                }
                items[j] = tmp;
            }
        }
        u32 out = 0;
        for (u32 i = 0; i < count; i++) {
            if (out > 0 && items[i].start <= items[out - 1].end) {
                if (items[i].end > items[out - 1].end) {
                    items[out - 1].end = items[i].end;
                }
            } else {
                items[out++] = items[i];
            }
        }
        count = out;
    }

    SaveStateRange* items;
    u32 count;
    u32 capacity;
};

static JKRExpHeap** const l_heaps[] = {&gameHeap, &zeldaHeap, &archiveHeap, &j2dHeap, &commandHeap};
static const u32 HEAP_COUNT = sizeof(l_heaps) / sizeof(l_heaps[0]);
static const u32 DOL_RANGE_COUNT = sizeof(l_dolRanges) / sizeof(l_dolRanges[0]);

static const char* l_unavailableReason;
static u8* l_storage;
static u32 l_storageSize;
static bool l_hasState;
static OSModuleInfo* l_bootModule;
static SaveStateAction l_pendingAction;
static int l_waitFrames;
static u16 l_lastButtons;
static const char* l_busyReason;

static const u8* sceneWaves() {
    return reinterpret_cast<const u8*>(Z2GetSceneMgr()) + Z2_SCENE_WAVES_OFFSET;
}

static u32 heapStart(u32 i) {
    return (u32)*l_heaps[i];
}

static u32 heapEnd(u32 i) {
    return (u32)(*l_heaps[i])->getEndAddr();
}

static bool overlapsHeaps(u32 start, u32 end) {
    for (u32 i = 0; i < HEAP_COUNT; i++) {
        if (start < heapEnd(i) && end > heapStart(i)) {
            return true;
        }
    }
    return false;
}

static bool overlapsSnapshot(u32 start, u32 end) {
    for (u32 i = 0; i < DOL_RANGE_COUNT; i++) {
        if (start < l_dolRanges[i].end && end > l_dolRanges[i].start) {
            return true;
        }
    }
    return overlapsHeaps(start, end);
}

static bool pushPreserved(RangeList& list, u32 start, u32 end) {
    return !overlapsSnapshot(start, end) || list.push(start, end);
}

static bool isRestored(u32 addr, const RangeList& preserved) {
    for (u32 i = 0; i < preserved.count; i++) {
        if (addr >= preserved.items[i].start && addr < preserved.items[i].end) {
            return false;
        }
    }
    return overlapsHeaps(addr, addr + 1);
}

static OSModuleInfo* moduleListHead() {
    return reinterpret_cast<OSModuleQueue*>(OS_MODULE_LIST)->head;
}

static OSModuleInfo* findBootModule() {
    u32 addr = (u32)&GZ_handleModules;
    for (OSModuleInfo* module = moduleListHead(); module != NULL; module = module->link.next) {
        OSSectionInfo* sections = reinterpret_cast<OSSectionInfo*>(module->sectionInfoOffset);
        for (u32 i = 0; i < module->numSections; i++) {
            u32 start = sections[i].offset & ~1;
            if (start != 0 && addr >= start && addr < start + sections[i].size) {
                return module;
            }
        }
    }
    return NULL;
}

static bool collectOutsideModules(ModuleLink* links, u32* count, const RangeList& preserved) {
    *count = 0;
    for (OSModuleInfo* module = moduleListHead(); module != NULL; module = module->link.next) {
        if (isRestored((u32)module, preserved)) {
            continue;
        }
        if (*count == MAX_OUTSIDE_MODULES) {
            return false;
        }
        links[*count].module = module;
        links[*count].next = module->link.next;
        links[*count].prev = module->link.prev;
        (*count)++;
    }
    return true;
}

static u32 threadObjectSize(JKRThread* thread) {
    if (thread == JASDvd::sThread) {
        return sizeof(JASTaskThread);
    }
    if (thread == JASGlobalInstance<JASAudioThread>::getInstance()) {
        return sizeof(JASAudioThread);
    }
    return sizeof(JKRThread);
}

static bool collectPreserved(RangeList& list) {
    for (u32 i = 0; i < HEAP_COUNT; i++) {
        for (JKRExpHeap::CMemBlock* block = (*l_heaps[i])->getUsedFirst(); block != NULL; block = block->getNextBlock()) {
            u32 start = (u32)block->getContent();
            if (block->getGroupId() == TPGZ_HEAP_GROUP_ID && !list.push(start, start + block->getSize())) {
                return false;
            }
        }
    }

    if (l_bootModule != NULL) {
        OSSectionInfo* sections = reinterpret_cast<OSSectionInfo*>(l_bootModule->sectionInfoOffset);
        for (u32 i = 0; i < l_bootModule->numSections; i++) {
            u32 start = sections[i].offset;
            if (start != 0 && (start & 1) == 0 && !pushPreserved(list, start, start + sections[i].size)) {
                return false;
            }
        }
    }

    OSThreadQueue* threads = reinterpret_cast<OSThreadQueue*>(OS_ACTIVE_THREAD_QUEUE);
    for (OSThread* thread = threads->head; thread != NULL; thread = thread->linkActive.next) {
        if (!pushPreserved(list, (u32)thread, (u32)thread + sizeof(OSThread)) ||
            !pushPreserved(list, (u32)thread->stackEnd, (u32)thread->stackBase))
        {
            return false;
        }
    }

    for (JSULink<JKRThread>* link = JKRThread::getList().getFirst(); link != NULL; link = link->getNext()) {
        u32 thread = (u32)link->getObject();
        OSMessageQueue* queue = reinterpret_cast<OSMessageQueue*>(thread + JKRTHREAD_MESSAGE_QUEUE_OFFSET);
        u32 messages = (u32)queue->msgArray;
        if (!pushPreserved(list, thread + sizeof(JKRDisposer), thread + threadObjectSize(link->getObject())) ||
            (messages != 0 && !pushPreserved(list, messages, messages + queue->msgCount * sizeof(OSMessage))))
        {
            return false;
        }
    }

    list.sortMerge();
    return true;
}

static bool collectSkipped(RangeList& list, const RangeList& preserved) {
    for (u32 i = 0; i < preserved.count; i++) {
        if (!list.push(preserved.items[i].start, preserved.items[i].end)) {
            return false;
        }
    }
    for (u32 i = 0; i < HEAP_COUNT; i++) {
        JKRExpHeap* heap = *l_heaps[i];
        JKRExpHeap::CMemBlock* block =
            *reinterpret_cast<JKRExpHeap::CMemBlock**>(reinterpret_cast<u8*>(&heap->mAllocMode) + 0xC);
        for (; block != NULL; block = block->getNextBlock()) {
            u32 start = (u32)block->getContent();
            if (!list.push(start, start + block->getSize())) {
                return false;
            }
        }
    }
    list.sortMerge();
    return true;
}

static bool pushSegments(RangeList& segments, u32 cursor, u32 end, const RangeList& skipped) {
    for (u32 i = 0; i < skipped.count && cursor < end; i++) {
        if (skipped.items[i].end <= cursor || skipped.items[i].start >= end) {
            continue;
        }
        if (!segments.push(cursor, skipped.items[i].start)) {
            return false;
        }
        cursor = skipped.items[i].end;
    }
    return cursor >= end || segments.push(cursor, end);
}

static bool collectSegments(RangeList& segments, const RangeList& skipped) {
    for (u32 i = 0; i < DOL_RANGE_COUNT; i++) {
        if (!pushSegments(segments, l_dolRanges[i].start, l_dolRanges[i].end, skipped)) {
            return false;
        }
    }
    for (u32 i = 0; i < HEAP_COUNT; i++) {
        if (!pushSegments(segments, heapStart(i), heapEnd(i), skipped)) {
            return false;
        }
    }
    return true;
}

static u32 alignUp(u32 value) {
    return (value + 0x1F) & ~0x1F;
}

static u8* firstSegment(u32 preservedCount) {
    return l_storage + alignUp(sizeof(SaveStateHeader) + preservedCount * sizeof(SaveStateRange));
}

static bool ensureStorage(u32 size) {
#ifdef WII_PLATFORM
    if (l_storage != NULL && l_storageSize >= size) {
        return true;
    }
    JKRHeap* heap = JKRHeap::getRootHeap2();
    if (l_storage != NULL) {
        JKRHeap::free(l_storage, heap);
        l_storage = NULL;
        l_hasState = false;
    }
    u32 available = heap->getFreeSize();
    if (available < size + MEM2_MARGIN) {
        return false;
    }
    l_storageSize = size + size / 8 < available - MEM2_MARGIN ? size + size / 8 : available - MEM2_MARGIN;
    l_storage = static_cast<u8*>(JKRHeap::alloc(l_storageSize, 0x20, heap));
    return l_storage != NULL;
#else
    return l_storageSize >= size;
#endif
}

static bool drainAudio() {
    Z2SoundMgr* soundMgr = Z2GetSoundMgr();
    for (int i = 0; i < MAX_AUDIO_DRAIN_SUBFRAMES; i++) {
        soundMgr->getSeMgr()->stop();
        soundMgr->getSeqMgr()->stop();
        soundMgr->getStreamMgr()->stop();
        soundMgr->calc();
        if (soundMgr->getSeMgr()->getNumActiveSe() == 0 && soundMgr->getSeqMgr()->getNumActiveSeqs() == 0 &&
            !soundMgr->getStreamMgr()->isActive())
        {
            return true;
        }
        soundMgr->mixOut();
        JASDriver::waitSubFrame();
    }
    return false;
}

static const char* findBusyReason() {
    node_list_class* commands = reinterpret_cast<node_list_class*>(reinterpret_cast<u8*>(&mDoDvdThd::l_param) +
                                                                   DVD_THREAD_COMMAND_LIST_OFFSET);
    if (commands->mSize != 0 || DVDGetDriveStatus() != DVD_STATE_END) {
        return "disc";
    }
    if (!drainAudio()) {
        return "audio";
    }
    GXBool overhi, underlow, readIdle, cmdIdle, brkpt;
    for (int i = 0; i < GP_IDLE_POLLS; i++) {
        GXGetGPStatus(&overhi, &underlow, &readIdle, &cmdIdle, &brkpt);
        if (readIdle && cmdIdle) {
            return NULL;
        }
    }
    return "graphics";
}

static bool isInPlayableScene() {
#ifdef WII_PLATFORM
    if (GZ_isDvdErrorShown()) {
        return false;
    }
#endif
    return dComIfGp_getPlayer(0) != NULL && !l_fopScnRq_IsUsingOfOverlap;
}

static void pushMessage(const char* msg) {
    FIFOQueue::push(msg, Queue);
    OSReport("%s\n", msg);
}

static void pushTimedMessage(const char* prefix, u32 bytes, OSTime ticks) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%s (%d KB, %d ms)", prefix, (int)(bytes / 1024), (int)OSTicksToMilliseconds(ticks));
    pushMessage(buf);
}

static void captureState() {
    RangeList preserved, skipped, segments;
    ModuleLink outsideModules[MAX_OUTSIDE_MODULES];
    u32 outsideModuleCount;
    if (!collectPreserved(preserved) || !collectSkipped(skipped, preserved) || !collectSegments(segments, skipped) ||
        !collectOutsideModules(outsideModules, &outsideModuleCount, preserved))
    {
        pushMessage("save state failed: out of memory");
        return;
    }

    u32 size = alignUp(sizeof(SaveStateHeader) + preserved.count * sizeof(SaveStateRange));
    for (u32 i = 0; i < segments.count; i++) {
        size += alignUp(sizeof(SaveStateSegment) + segments.items[i].end - segments.items[i].start);
    }
    if (!ensureStorage(size)) {
        char buf[64];
        snprintf(buf, sizeof(buf), "save state failed: needs %d KB", (int)(size / 1024));
        pushMessage(buf);
        return;
    }

    SaveStateHeader* header = reinterpret_cast<SaveStateHeader*>(l_storage);
    header->totalSize = size;
    header->segmentCount = segments.count;
    header->preservedCount = preserved.count;
    strncpy(header->stageName, dComIfGp_getStartStageName(), sizeof(header->stageName));
    memcpy(header->sceneWaves, sceneWaves(), sizeof(header->sceneWaves));
    header->outsideModuleCount = outsideModuleCount;
    memcpy(header->outsideModules, outsideModules, sizeof(outsideModules));
    memcpy(header + 1, preserved.items, preserved.count * sizeof(SaveStateRange));

    u8* cursor = firstSegment(preserved.count);
    OSTime startTime = OSGetTime();
    BOOL enabled = OSDisableInterrupts();
    for (u32 i = 0; i < segments.count; i++) {
        SaveStateSegment* segment = reinterpret_cast<SaveStateSegment*>(cursor);
        segment->addr = segments.items[i].start;
        segment->size = segments.items[i].end - segments.items[i].start;
        memcpy(segment + 1, reinterpret_cast<void*>(segment->addr), segment->size);
        cursor += alignUp(sizeof(SaveStateSegment) + segment->size);
    }
    OSRestoreInterrupts(enabled);

    l_hasState = true;
    pushTimedMessage("state saved", size, OSGetTime() - startTime);
}

static const char* validateState(SaveStateHeader* header) {
    if (strncmp(header->stageName, dComIfGp_getStartStageName(), sizeof(header->stageName)) != 0) {
        return "load state failed: different stage";
    }
    if (memcmp(header->sceneWaves, sceneWaves(), sizeof(header->sceneWaves)) != 0) {
        return "load state failed: different room audio";
    }

    ModuleLink outsideModules[MAX_OUTSIDE_MODULES];
    u32 outsideModuleCount;
    RangeList preserved;
    if (!collectPreserved(preserved) || !collectOutsideModules(outsideModules, &outsideModuleCount, preserved)) {
        return "load state failed: out of memory";
    }
    bool same = outsideModuleCount == header->outsideModuleCount && preserved.count == header->preservedCount &&
                memcmp(preserved.items, header + 1, preserved.count * sizeof(SaveStateRange)) == 0;
    for (u32 i = 0; same && i < outsideModuleCount; i++) {
        same = outsideModules[i].module == header->outsideModules[i].module;
    }
    return same ? NULL : "load state failed: tpgz memory changed";
}

static void restoreState() {
    SaveStateHeader* header = reinterpret_cast<SaveStateHeader*>(l_storage);
    const char* error = validateState(header);
    if (error != NULL) {
        pushMessage(error);
        return;
    }

    u8* cursor = firstSegment(header->preservedCount);
    OSTime startTime = OSGetTime();
    BOOL enabled = OSDisableInterrupts();
    for (u32 i = 0; i < header->segmentCount; i++) {
        SaveStateSegment* segment = reinterpret_cast<SaveStateSegment*>(cursor);
        void* dst = reinterpret_cast<void*>(segment->addr);
        memcpy(dst, segment + 1, segment->size);
        DCFlushRange(dst, segment->size);
        ICInvalidateRange(dst, segment->size);
        cursor += alignUp(sizeof(SaveStateSegment) + segment->size);
    }
    for (u32 i = 0; i < header->outsideModuleCount; i++) {
        header->outsideModules[i].module->link.next = header->outsideModules[i].next;
        header->outsideModules[i].module->link.prev = header->outsideModules[i].prev;
    }
    OSRestoreInterrupts(enabled);

    g_actorViewEnabled = false;
    g_moveLinkEnabled = false;
    g_freeCamEnabled = false;
    pushTimedMessage("state loaded", header->totalSize, OSGetTime() - startTime);
}

static bool tryRunPendingAction() {
    for (tpgz::containers::deque<Module*>::iterator it = g_modules.begin(); it != g_modules.end(); ++it) {
        if ((*it)->active != saveStates_active && (*it)->rel.isLoaded()) {
            (*it)->rel.close();
        }
    }
    if (l_framePauseTex.loadCode == TEX_OK) {
        free_texture(&l_framePauseTex);
    }
    if (l_framePlayTex.loadCode == TEX_OK) {
        free_texture(&l_framePlayTex);
    }
    l_busyReason = findBusyReason();
    if (l_busyReason != NULL) {
        return false;
    }
    if (l_pendingAction == SS_ACTION_SAVE) {
        captureState();
    } else {
        restoreState();
    }
    return true;
}

KEEP_FUNC void GZ_initSaveStates() {
    l_bootModule = findBootModule();
    l_storage = g_mem2Storage;
    l_storageSize = g_mem2StorageSize;
    l_unavailableReason = g_mem2UnavailableReason;
    if (l_unavailableReason == NULL && l_bootModule == NULL) {
        l_unavailableReason = "save states unavailable";
    }
}

KEEP_FUNC void GZ_exitSaveStates() {
#ifdef WII_PLATFORM
    if (l_storage != NULL) {
        JKRHeap::free(l_storage, JKRHeap::getRootHeap2());
    }
#endif
    g_skipGameFrame = false;
}

KEEP_FUNC void GZ_handleSaveStates() {
    g_skipGameFrame = false;
    u16 buttons = GZ_getButtonStatus();

#ifdef WII_PLATFORM
    if ((buttons & SAVE_STATE_MODIFIER_BUTTONS) == SAVE_STATE_MODIFIER_BUTTONS) {
        PAD_HOLD &= ~SAVE_STATE_DPAD_BUTTONS;
        PAD_TRIG &= ~SAVE_STATE_DPAD_BUTTONS;
    }
#endif

    if (l_pendingAction == SS_ACTION_NONE) {
        bool pressedSave = buttons == SAVE_STATE_BUTTONS && l_lastButtons != SAVE_STATE_BUTTONS;
        bool pressedLoad = buttons == LOAD_STATE_BUTTONS && l_lastButtons != LOAD_STATE_BUTTONS;
        l_lastButtons = buttons;
        if ((!pressedSave && !pressedLoad) || g_menuMgr->isOpen()) {
            return;
        }

        PAD_HOLD = 0;
        PAD_TRIG = 0;
        if (l_unavailableReason != NULL) {
            pushMessage(l_unavailableReason);
        } else if (!isInPlayableScene()) {
            pushMessage("save states only work in gameplay");
        } else if (pressedLoad && !l_hasState) {
            pushMessage("no save state");
        } else {
            l_pendingAction = pressedSave ? SS_ACTION_SAVE : SS_ACTION_LOAD;
            l_waitFrames = 0;
        }
        if (l_pendingAction == SS_ACTION_NONE) {
            return;
        }
    }

    if (!isInPlayableScene()) {
        l_pendingAction = SS_ACTION_NONE;
        pushMessage("save state cancelled");
    } else if (tryRunPendingAction()) {
        l_pendingAction = SS_ACTION_NONE;
    } else if (++l_waitFrames > MAX_WAIT_FRAMES) {
        l_pendingAction = SS_ACTION_NONE;
        char buf[64];
        snprintf(buf, sizeof(buf), "save state failed: %s did not go idle", l_busyReason);
        pushMessage(buf);
    } else {
        g_skipGameFrame = true;
        VIWaitForRetrace();
    }
}
