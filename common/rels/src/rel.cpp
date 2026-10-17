#include "REL/executor.h"

namespace tpgz {
namespace modules {
extern void main();
extern void exit();
}
}

void _prolog() {
    ModuleConstructorsX(_ctors);
    tpgz::modules::main();
}

void _epilog() {
    tpgz::modules::exit();
    ModuleDestructorsX(_dtors);
}

void _unresolved() {}
