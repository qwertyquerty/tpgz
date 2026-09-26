#include <main.h>
#include "gz_flags.h"
#include "commands.h"
#include "gorge.h"
#include "settings.h"
#include "modules.h"

static bool gorgeVoidActive() {
    return GZ_getGorgeVoidMode() != 0;
}

namespace tpgz {
namespace modules {
void main() {
    GZCmd_addCmd(new Command(
        CMD_GORGE_VOID, GZStng_getData<uint16_t>(STNG_CMD_GORGE_VOID, GORGE_VOID_BUTTONS),
        GZCmd_loadGorgeVoid
    ));
    GZFlg_addFlag(new GZFlag(
        GZFLG_GORGE_VOID, 
        gorgeVoidActive,
        GAME_LOOP,
        GorgeVoidIndicator::execute
    ));
}
void exit() {
    GZFlag* flg = GZFlg_removeFlag(GZFLG_GORGE_VOID);
    delete flg;
    Command* cmd = GZCmd_removeCmd(CMD_GORGE_VOID);
    delete cmd;
}

}
}  // namespace tpgz::modules