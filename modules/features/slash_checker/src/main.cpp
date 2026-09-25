#include <main.h>
#ifdef WII_PLATFORM
#include "slash_checker.h"
#endif
#include "gz_flags.h"

namespace tpgz {
namespace modules {
void main() {
    #ifdef WII_PLATFORM
    GZFlg_addFlag(new GZFlag(GZFLG_SLASH, ACTIVE_FUNC(STNG_TOOLS_SLASH), GAME_LOOP,
                             SlashChecker::execute));
    #endif
}
void exit() {
    #ifdef WII_PLATFORM
    GZFlag* flg = GZFlg_removeFlag(GZFLG_SLASH);
    delete flg;
    #endif
}

}
}  // namespace tpgz::modules