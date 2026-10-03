#include "font.h"
#include <stdint.h>
#include "types.h"
#ifdef WII_PLATFORM
#include <nand.h>
typedef NANDFileInfo FileInfo;
#define STORAGE_FILENAME_MAX NAND_MAX_PATH
#define SECTOR_SIZE 0x400
#define OPEN_MODE_READ NAND_ACCESS_READ
#define OPEN_MODE_WRITE NAND_ACCESS_WRITE
#define OPEN_MODE_RW NAND_ACCESS_RW
#define STORAGE_SEEK_BEG 0
#define STORAGE_SEEK_CUR 1
#define STORAGE_SEEK_END 2
#else
#include <card.h>
typedef CARDFileInfo FileInfo;
#define STORAGE_FILENAME_MAX CARD_FILENAME_MAX
#define SECTOR_SIZE INT32_MAX
#define OPEN_MODE_READ 0
#define OPEN_MODE_WRITE 0
#define OPEN_MODE_RW 0
#define STORAGE_SEEK_BEG 0
#define STORAGE_SEEK_CUR 1
#define STORAGE_SEEK_END 2
#endif

enum StorageError { Ready = 0, Busy = -1, WrongDevice = -2, NoCard = -3, NoFile = -4,
                    IoError = -5, Broken = -6, Exist = -7, NoEnt = -8, InsSpace = -9,
                    NoPerm = -10, Limit = -11, NameTooLong = -12, Encoding = -13,
                    Canceled = -14, FatalError = -128 };

struct Storage {
    FileInfo info;
    s32 sector_size;
    const char* file_name;
    s32 result;
    char file_name_buffer[STORAGE_FILENAME_MAX * 2];
    uint32_t result_size;
    uint32_t position;
};

#ifdef WII_PLATFORM
#define StorageFreeBlocks(ch, bytes, files) ((void)0)
#define StorageCreate(ch, name, size, info) (NANDCreate(name, 0x34, 0))
#define StorageDelete(ch, name) NANDDelete(name)
#define StorageOpen(ch, name, info, mode) NANDOpen(name, info, mode)
#define StorageClose(info) NANDClose(info)
#define StorageRead(storage, data, length) \
    ((storage).result_size = NANDRead(&(storage).info, data, length), (storage).result = Ready)
#define StorageWrite(storage, data, length) \
    ((storage).result_size = NANDWrite(&(storage).info, data, length), (storage).result = Ready)
#define StorageSeek(storage, offset, whence) NANDSeek(&(*(storage)).info, offset, whence)
#else
#define StorageFreeBlocks(ch, bytes, files) CARDFreeBlocks(ch, bytes, files)
#define StorageCreate(ch, name, size, info) CARDCreate(ch, name, size, info)
#define StorageDelete(ch, name) CARDDelete(ch, name)
#define StorageOpen(ch, name, info, mode) CARDOpen(ch, name, info)
#define StorageClose(info) CARDClose(info)
#define StorageRead(storage, data, length) \
    ((storage).result = CARDRead(&(storage).info, data, length, (storage).position), \
     (storage).position += (length), (storage).result)
#define StorageWrite(storage, data, length) \
    ((storage).result = CARDWrite(&(storage).info, data, length, (storage).position), \
     (storage).position += (length), (storage).result)
inline int32_t StorageSeek(Storage* storage, int32_t offset, int32_t whence) {
    if (whence == STORAGE_SEEK_BEG) storage->position = offset;
    else if (whence == STORAGE_SEEK_CUR) storage->position += offset;
    else if (whence == STORAGE_SEEK_END) storage->position = storage->info.length + offset;
    else return IoError;
    return storage->position;
}
#endif
#include "menus/menu.h"

#define GZ_SAVE_VERSION_NUMBER 1

#ifdef WII_PLATFORM
extern void* g_tmpBuf;
#endif

struct GZSaveHeader {
    uint32_t version;
    size_t data_size;
    uint32_t entries;
};

struct GZSaveFile {
    GZSaveHeader header;
};

int32_t GZ_storageWrite(Storage* info, void* data, int32_t size, int32_t offset,
                        int32_t sector_size);
int32_t GZ_storageRead(Storage* info, void* data, int32_t size, int32_t offset,
                       int32_t sector_size);
void GZ_storeMemCard(Storage& card);
void GZ_storeMemfile(Storage& card);
void GZ_deleteMemCard(Storage& card);
void GZ_deleteMemfile(Storage& card);
void GZ_loadMemCard(Storage& card);
void GZ_loadMemfile(Storage& card);
bool GZ_memfileExists(Storage& card);
void GZ_loadGZSave(bool& card_load);
