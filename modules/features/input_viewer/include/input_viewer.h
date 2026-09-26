#ifndef TPGZ_MODULES_FEATURES_INPUT_VIEWER_INCLUDE_INPUT_VIEWER_H
#define TPGZ_MODULES_FEATURES_INPUT_VIEWER_INCLUDE_INPUT_VIEWER_H
#include "font.h"

namespace InputViewer {
void draw();
void drawViewer(Vec2 pos, float scale, bool is_shadow, bool is_widescreen);
}  // namespace InputViewer

#endif
