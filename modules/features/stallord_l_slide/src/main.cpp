#include <main.h>
#include "stallord_l_slide_check.h"
#include "gz_flags.h"

namespace tpgz {
namespace modules {
void main() {
    GZFlg_addFlag(new GZFlag(GZFLG_STALLORD_L_SLIDE, ACTIVE_FUNC(STNG_TOOLS_STALLORD_L_SLIDE), GAME_LOOP,
                             StallordLSlideChecker::execute));
}
void exit() {
    GZFlag* flg = GZFlg_removeFlag(GZFLG_STALLORD_L_SLIDE);
    delete flg;
}

}
}  // namespace tpgz::modules
