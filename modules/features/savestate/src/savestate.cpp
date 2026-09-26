#include "features/savestate/include/savestate.h"
#include <cstdio>
#include <cstring>
#include "fifo_queue.h"
#include "game_state.h"
#include "global_data.h"
#include "modules.h"
#include "commands.h"
#include "settings.h"
#include "utils/mem2.h"
#include "menus/utils/menu_mgr.h"
#include "utils/texture.h"
#include "rels/include/cxx.h"
#include "rels/include/defines.h"
#include "d/d_com_inf_game.h"
#include "m_Do/m_Do_dvd_thread.h"
#include "m_Do/m_Do_graphic.h"
#include "JSystem/JKernel/JKRExpHeap.h"
#include "JSystem/JKernel/JKRThread.h"
#include "JSystem/JKernel/JKRDisposer.h"
#include "DynamicLink.h"
#include "JSystem/JAudio2/JASAudioThread.h"
#include "JSystem/JAudio2/JASTaskThread.h"
#include "JSystem/JAudio2/JASDvdThread.h"
#include "JSystem/JAudio2/JAIStreamMgr.h"
#include "JSystem/JAudio2/JAISeMgr.h"
#include "JSystem/JAudio2/JAISeqMgr.h"
#include "JSystem/JAudio2/JASDriverIF.h"
#include "JSystem/JAudio2/JASHeapCtrl.h"
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

#define MEM2_MARGIN 0x40000
#define MAX_WAIT_FRAMES 120
#define GP_IDLE_POLLS 100000
#define MAX_AUDIO_DRAIN_SUBFRAMES 240
#define MAX_OUTSIDE_MODULES 32
#define JKRTHREAD_MESSAGE_QUEUE_OFFSET 0x30
#define JKREXPHEAP_FREE_LIST_OFFSET 0x78
#define JKRSOLIDHEAP_HEAD_OFFSET 0x70
#define JKRSOLIDHEAP_TAIL_OFFSET 0x74
#define MAX_HEAP_DEPTH 8
#define MIN_ZERO_RUN 32
#define CHECK_SAVE_STATE_LOCATION 1
#define ZERO_SEGMENT_FLAG 0x80000000
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
    u32 audioMemoryHash;
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

#ifdef WII_PLATFORM
static JKRHeap** const l_heapSources[] = {
    (JKRHeap**)&gameHeap, (JKRHeap**)&zeldaHeap,   (JKRHeap**)&archiveHeap,
    (JKRHeap**)&j2dHeap,  (JKRHeap**)&commandHeap, &DynamicModuleControlBase::m_heap,
};
#else
static JKRHeap** const l_heapSources[] = {
    (JKRHeap**)&gameHeap, (JKRHeap**)&zeldaHeap, (JKRHeap**)&archiveHeap, (JKRHeap**)&j2dHeap, (JKRHeap**)&commandHeap,
};
#endif
static const u32 MAX_HEAPS = sizeof(l_heapSources) / sizeof(l_heapSources[0]);
static JKRExpHeap* l_heaps[MAX_HEAPS];
static u32 l_heapCount;
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

static u32 hashWord(u32 hash, u32 value) {
    for (int i = 0; i < 4; i++) {
        hash = (hash ^ ((value >> (i * 8)) & 0xFF)) * 0x01000193;
    }
    return hash;
}

static u32 hashAudioHeap(u32 hash, JASHeap* heap, u32 depth) {
    hash = hashWord(hashWord(hashWord(hash, (u32)heap), (u32)heap->getBase()), heap->getSize());
    if (depth < MAX_HEAP_DEPTH) {
        for (JSUTree<JASHeap>* child = heap->getFirstChild(); child != NULL; child = child->getNextChild()) {
            hash = hashAudioHeap(hash, child->getObject(), depth + 1);
        }
    }
    return hash;
}

static u32 audioMemoryHash() {
    return hashAudioHeap(0x811C9DC5, JASKernel::getAramHeap(), 0);
}

static const u8* sceneWaves() {
    return reinterpret_cast<const u8*>(Z2GetSceneMgr()) + Z2_SCENE_WAVES_OFFSET;
}

static u32 heapStart(u32 i) {
    return (u32)l_heaps[i];
}

static u32 heapEnd(u32 i) {
    return (u32)l_heaps[i]->getEndAddr();
}

static bool overlapsHeaps(u32 start, u32 end) {
    for (u32 i = 0; i < l_heapCount; i++) {
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

static JKRExpHeap::CMemBlock* heapFreeFirst(JKRExpHeap* heap) {
    return *reinterpret_cast<JKRExpHeap::CMemBlock**>(reinterpret_cast<u8*>(heap) + JKREXPHEAP_FREE_LIST_OFFSET);
}

static bool isBlockListValid(u32 start, u32 end, JKRExpHeap::CMemBlock* block, bool used) {
    for (u32 count = 0; block != NULL; block = block->getNextBlock(), count++) {
        u32 addr = (u32)block;
        if (count > 0x10000 || addr < start || addr + sizeof(*block) > end || (used && !block->isValid()) ||
            addr + sizeof(*block) + block->getSize() > end)
        {
            return false;
        }
    }
    return true;
}

static bool areHeapsValid() {
    for (u32 i = 0; i < l_heapCount; i++) {
        if (!isBlockListValid(heapStart(i), heapEnd(i), l_heaps[i]->getUsedFirst(), true) ||
            !isBlockListValid(heapStart(i), heapEnd(i), heapFreeFirst(l_heaps[i]), false))
        {
            return false;
        }
    }
    return true;
}

static bool collectPreserved(RangeList& list) {
    for (u32 i = 0; i < l_heapCount; i++) {
        for (JKRExpHeap::CMemBlock* block = l_heaps[i]->getUsedFirst(); block != NULL; block = block->getNextBlock()) {
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

static bool pushFreeSpace(RangeList& list, JKRHeap* heap, u32 depth) {
    u32 start = (u32)heap;
    u32 end = (u32)heap->getEndAddr();
    u32 type = heap->getHeapType();
    if (type == 'EXPH') {
        JKRExpHeap::CMemBlock* first = heapFreeFirst(static_cast<JKRExpHeap*>(heap));
        if (!isBlockListValid(start, end, first, false)) {
            first = NULL;
        }
        for (JKRExpHeap::CMemBlock* block = first; block != NULL; block = block->getNextBlock()) {
            u32 content = (u32)block->getContent();
            if (!list.push(content, content + block->getSize())) {
                return false;
            }
        }
    } else if (type == 'SLID') {
        u32 head = *reinterpret_cast<u32*>(start + JKRSOLIDHEAP_HEAD_OFFSET);
        u32 tail = *reinterpret_cast<u32*>(start + JKRSOLIDHEAP_TAIL_OFFSET);
        if (start < head && head <= tail && tail <= end && !list.push(head, tail)) {
            return false;
        }
    }
    if (depth < MAX_HEAP_DEPTH) {
        for (JSUTree<JKRHeap>* child = heap->getHeapTree().getFirstChild(); child != NULL;
             child = child->getNextChild())
        {
            JKRHeap* childHeap = child->getObject();
            if ((u32)childHeap >= start && (u32)childHeap->getEndAddr() <= end &&
                !pushFreeSpace(list, childHeap, depth + 1))
            {
                return false;
            }
        }
    }
    return true;
}

static bool collectSkipped(RangeList& list, const RangeList& preserved) {
    for (u32 i = 0; i < preserved.count; i++) {
        if (!list.push(preserved.items[i].start, preserved.items[i].end)) {
            return false;
        }
    }
    for (u32 i = 0; i < l_heapCount; i++) {
        if (!pushFreeSpace(list, l_heaps[i], 0)) {
            return false;
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
    for (u32 i = 0; i < l_heapCount; i++) {
        if (!pushSegments(segments, heapStart(i), heapEnd(i), skipped)) {
            return false;
        }
    }
#ifdef WII_PLATFORM
    JKRExpHeap::CMemBlock* capture = JKRExpHeap::CMemBlock::getBlock(mDoGph_gInf_c::getFrameBufferTimg());
    if (capture->isValid()) {
        u32 start = (u32)capture->getContent();
        if (!segments.push(start, start + capture->getSize())) {
            return false;
        }
    }
#endif
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

struct Encoder {
    u8* out;
    u8* limit;
    u32 records;
    bool write;
    bool overflow;
};

static u32 recordSize(u32 size, bool zero) {
    return zero ? sizeof(SaveStateSegment) : (sizeof(SaveStateSegment) + size + 3) & ~3;
}

static void emitRecord(Encoder& encoder, u32 addr, u32 size, bool zero) {
    u32 bytes = recordSize(size, zero);
    if (encoder.out + bytes > encoder.limit) {
        encoder.overflow = true;
        return;
    }
    if (encoder.write) {
        SaveStateSegment* segment = reinterpret_cast<SaveStateSegment*>(encoder.out);
        segment->addr = addr;
        segment->size = size | (zero ? ZERO_SEGMENT_FLAG : 0);
        if (!zero) {
            memcpy(segment + 1, reinterpret_cast<void*>(addr), size);
        }
    }
    encoder.out += bytes;
    encoder.records++;
}

static u32 findZeroRun(u32 start, u32 end, u32* runEnd) {
    u32 runStart = 0;
    u32 words = 0;
    u32 addr = (start + 3) & ~3;
    for (; addr + 4 <= end; addr += 4) {
        if (*reinterpret_cast<const u32*>(addr) == 0) {
            runStart = words++ == 0 ? addr : runStart;
        } else if (words * 4 >= MIN_ZERO_RUN) {
            break;
        } else {
            words = 0;
        }
    }
    if (words * 4 >= MIN_ZERO_RUN) {
        *runEnd = runStart + words * 4;
        return runStart;
    }
    *runEnd = end;
    return end;
}

static void encodeSegments(Encoder& encoder, const RangeList& segments) {
    for (u32 i = 0; i < segments.count && !encoder.overflow; i++) {
        u32 cursor = segments.items[i].start;
        u32 end = segments.items[i].end;
        while (cursor < end && !encoder.overflow) {
            u32 runEnd;
            u32 runStart = findZeroRun(cursor, end, &runEnd);
            if (runStart > cursor) {
                emitRecord(encoder, cursor, runStart - cursor, false);
            }
            if (runStart < end) {
                emitRecord(encoder, runStart, runEnd - runStart, true);
            }
            cursor = runEnd;
        }
    }
}

static void captureState() {
    RangeList preserved, skipped, segments;
    ModuleLink outsideModules[MAX_OUTSIDE_MODULES];
    u32 outsideModuleCount;
    if (!areHeapsValid()) {
        pushMessage("save state failed: unexpected heap layout");
        return;
    }
    if (!collectPreserved(preserved) || !collectSkipped(skipped, preserved) || !collectSegments(segments, skipped)) {
        pushMessage("save state failed: out of memory");
        return;
    }
    if (!collectOutsideModules(outsideModules, &outsideModuleCount, preserved)) {
        pushMessage("save state failed: too many unrestored modules");
        return;
    }

    Encoder estimate = {NULL, reinterpret_cast<u8*>(0xFFFFFFFF), 0, false, false};
    encodeSegments(estimate, segments);
    u32 headerSize = alignUp(sizeof(SaveStateHeader) + preserved.count * sizeof(SaveStateRange));
    u32 size = headerSize + (u32)estimate.out;
    if (!ensureStorage(size)) {
        char buf[64];
#ifdef WII_PLATFORM
        s32 usable = JKRHeap::getRootHeap2()->getFreeSize() - MEM2_MARGIN;
        snprintf(buf, sizeof(buf), "save state failed: needs %d KB, %d KB usable", (int)(size / 1024),
                 (int)((usable > 0 ? usable : 0) / 1024));
#else
        snprintf(buf, sizeof(buf), "save state failed: needs %d KB", (int)(size / 1024));
#endif
        pushMessage(buf);
        return;
    }

    SaveStateHeader* header = reinterpret_cast<SaveStateHeader*>(l_storage);
    header->preservedCount = preserved.count;
    strncpy(header->stageName, dComIfGp_getStartStageName(), sizeof(header->stageName));
    memcpy(header->sceneWaves, sceneWaves(), sizeof(header->sceneWaves));
    header->audioMemoryHash = audioMemoryHash();
    header->outsideModuleCount = outsideModuleCount;
    memcpy(header->outsideModules, outsideModules, sizeof(outsideModules));
    memcpy(header + 1, preserved.items, preserved.count * sizeof(SaveStateRange));

    Encoder encoder = {firstSegment(preserved.count), l_storage + l_storageSize, 0, true, false};
    OSTime startTime = OSGetTime();
    BOOL enabled = OSDisableInterrupts();
    encodeSegments(encoder, segments);
    OSRestoreInterrupts(enabled);
    OSTime elapsed = OSGetTime() - startTime;

    if (encoder.overflow) {
        l_hasState = false;
        pushMessage("save state failed: memory changed during save");
        return;
    }
    header->totalSize = encoder.out - l_storage;
    header->segmentCount = encoder.records;
    l_hasState = true;
    pushTimedMessage("state saved", header->totalSize, elapsed);
}

static const char* validateState(SaveStateHeader* header) {
#if CHECK_SAVE_STATE_LOCATION
    if (strncmp(header->stageName, dComIfGp_getStartStageName(), sizeof(header->stageName)) != 0) {
        return "load state failed: different stage";
    }
    if (memcmp(header->sceneWaves, sceneWaves(), sizeof(header->sceneWaves)) != 0) {
        return "load state failed: different room audio";
    }
    if (header->audioMemoryHash != audioMemoryHash()) {
        return "load state failed: audio memory changed";
    }
#endif

    ModuleLink outsideModules[MAX_OUTSIDE_MODULES];
    u32 outsideModuleCount;
    RangeList preserved;
    if (!areHeapsValid()) {
        return "load state failed: unexpected heap layout";
    }
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
        bool zero = (segment->size & ZERO_SEGMENT_FLAG) != 0;
        u32 size = segment->size & ~ZERO_SEGMENT_FLAG;
        if (zero) {
            memset(dst, 0, size);
        } else {
            memcpy(dst, segment + 1, size);
        }
        DCFlushRange(dst, size);
        ICInvalidateRange(dst, size);
        cursor += recordSize(size, zero);
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
    l_heapCount = 0;
    bool heapsValid = true;
    for (u32 i = 0; i < MAX_HEAPS; i++) {
        JKRHeap* heap = *l_heapSources[i];
        if (heap != NULL) {
            heapsValid &= heap->getHeapType() == 'EXPH';
            l_heaps[l_heapCount++] = static_cast<JKRExpHeap*>(heap);
        }
    }
    l_bootModule = findBootModule();
    l_storage = g_mem2Storage;
    l_storageSize = g_mem2StorageSize;
    l_unavailableReason = g_mem2UnavailableReason;
    if (l_unavailableReason == NULL && (l_bootModule == NULL || !heapsValid)) {
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


    if (l_pendingAction == SS_ACTION_NONE) {
        u16 saveButtons = GZStng_getData<uint16_t>(STNG_CMD_STORE_POSITION, STORE_POSITION_BUTTONS);
        u16 loadButtons = GZStng_getData<uint16_t>(STNG_CMD_LOAD_POSITION, LOAD_POSITION_BUTTONS);
        bool pressedSave = buttons == saveButtons && l_lastButtons != saveButtons;
        bool pressedLoad = buttons == loadButtons && l_lastButtons != loadButtons;
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
