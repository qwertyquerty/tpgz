#include <main.h>
#include "gz_flags.h"
#include "ee_checker.h"

namespace tpgz {
namespace modules {
void main() {
    GZFlg_addFlag(
        new GZFlag(GZFLG_EE, ACTIVE_FUNC(STNG_TOOLS_ELEVATOR_ESCAPE), GAME_LOOP, EEChecker::execute));
}
void exit() {
    GZFlag* flg = GZFlg_removeFlag(GZFLG_EE);
    delete flg;
}

}
}  // namespace tpgz::modules