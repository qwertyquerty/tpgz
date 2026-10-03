#include <main.h>
#include "features/savestate/include/savestate.h"
#include "events/pre_loop_listener.h"

namespace tpgz {
namespace modules {
void main() {
    GZ_initSaveStates();
    g_PreLoopListener->addListener(GZ_handleSaveStates);
}
void exit() {
    g_PreLoopListener->removeListener(GZ_handleSaveStates);
    GZ_exitSaveStates();
}

}
}  // namespace tpgz::modules
