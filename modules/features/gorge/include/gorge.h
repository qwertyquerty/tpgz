#ifndef TPGZ_MODULES_FEATURES_GORGE_INCLUDE_GORGE_H
#define TPGZ_MODULES_FEATURES_GORGE_INCLUDE_GORGE_H
void GZCmd_loadGorgeVoid();

namespace GorgeVoidIndicator {
void execute();
void initState();
void warpToPosition();
bool isAPressed();
bool isBootsPressed(); 
}  // namespace GorgeVoidIndicator

#endif
