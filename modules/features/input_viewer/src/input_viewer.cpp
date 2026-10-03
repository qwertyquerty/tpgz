#include "input_viewer.h"
#include <cstdio>
#include "tpgz_vec.h"
#include "controller.h"
#include "font.h"
#include "JSystem/JUtility/JUTGamePad.h"
#include "tpgz_math.h"
#include "pos_settings.h"
#include "settings.h"
#include "utils/draw.h"
#include "tools.h"
#include "rels/include/defines.h"

#define OUTLINE_WIDTH 0x10

void drawEllipse(uint32_t color, Vec2 center, Vec2 dim) {
    const size_t n_points = 4;
    float dx[n_points] = {0};
    float dy[n_points] = {0};
    float tmp;
    for (size_t i = 0; i < n_points; ++i) {
        dx[i] = cos(i * M_PI / 2.0 / n_points);
        dy[i] = sin(i * M_PI / 2.0 / n_points);
    }
    Draw::begin(4 * n_points, GX_TRIANGLEFAN);
    for (uint8_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < n_points; ++j) {
            Draw::add_vertex(color, makeVec2(center.x + dim.x / 2 * dx[j], center.y + dim.y / 2 * dy[j]),
                             makeVec2(0.5f + dx[j], 0.5f + dy[j]));
            tmp = dx[j];
            dx[j] = -dy[j];
            dy[j] = tmp;
        }
    }
    Draw::end();
}

void drawEllipseOutline(uint32_t color, Vec2 center, Vec2 dim, uint8_t line_width) {
    const size_t n_points = 4;
    float dx[n_points] = {0};
    float dy[n_points] = {0};
    float tmp;
    for (size_t i = 0; i < n_points; ++i) {
        dx[i] = cos(i * M_PI / 2.0 / n_points);
        dy[i] = sin(i * M_PI / 2.0 / n_points);
    }
    Draw::begin_outline(4 * n_points + 1, line_width);
    for (uint8_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < n_points; ++j) {
            Draw::add_vertex(color, makeVec2(center.x + dim.x / 2 * dx[j], center.y + dim.y / 2 * dy[j]),
                             makeVec2(0.5f + dx[j], 0.5f + dy[j]));
            tmp = dx[j];
            dx[j] = -dy[j];
            dy[j] = tmp;
        }
    }
    Draw::add_vertex(color, makeVec2(center.x + dim.x / 2 * dx[0], center.y + dim.y / 2 * dy[0]),
                     makeVec2(0.5f + dx[0], 0.5f + dy[0]));
    Draw::end();
}

/* void drawStick(uint32_t color, Vec2 pos, float size) {
    float s2 = size / 2;
    float dx1 = 0;
    float dy1 = -1.0f;
    float dx2 = 0.7071f;
    float dy2 = -0.7071f;
    float tmp;
    Draw::begin(8, GX_TRIANGLEFAN);
    for (uint8_t i = 0; i < 4; ++i) {
        Draw::add_vertex(color, makeVec2(pos.x + s2 * dx1, pos.y + s2 * dy1), makeVec2(0.5f + dx1, 0.5f + dy1));
        Draw::add_vertex(color, makeVec2(pos.x + s2 * dx2, pos.y + s2 * dy2), makeVec2(0.5f + dx2, 0.5f + dy2));
        tmp = dx1;
        dx1 = -dy1;
        dy1 = tmp;
        tmp = dx2;
        dx2 = -dy2;
        dy2 = tmp;
    }
    Draw::end();
} */

void drawStickOutline(uint32_t color, Vec2 pos, float size, float x_ratio) {
    float s2 = size / 2;
    float dx1 = 0;
    float dy1 = -1.0f;
    float dx2 = 0.7071f;
    float dy2 = -0.7071f;
    float tmp;
    Draw::begin_outline(9, OUTLINE_WIDTH);
    for (uint8_t i = 0; i < 4; ++i) {
        Draw::add_vertex(color, makeVec2(pos.x + s2 * dx1 * x_ratio, pos.y + s2 * dy1),
                         makeVec2((0.5f + dx1) * x_ratio, 0.5f + dy1));
        Draw::add_vertex(color, makeVec2(pos.x + s2 * dx2 * x_ratio, pos.y + s2 * dy2),
                         makeVec2((0.5f + dx2) * x_ratio, 0.5f + dy2));
        tmp = dx1;
        dx1 = -dy1;
        dy1 = tmp;
        tmp = dx2;
        dx2 = -dy2;
        dy2 = tmp;
    }
    Draw::add_vertex(color, makeVec2(pos.x + s2 * dx1 * x_ratio, pos.y + s2 * dy1),
                     makeVec2((0.5f + dx1) * x_ratio, 0.5f + dy1));
    Draw::end();
}

void drawButton(GZPad button, char c, uint32_t color, Vec2 pos, Vec2 dim, float char_size) {
    bool is_button_down = GZ_getButtonPressed(button);
    Draw::drawRectOutline(color, makeVec2(pos.x, pos.y), dim, OUTLINE_WIDTH);
    if (is_button_down) {
        Draw::drawRect(color, makeVec2(pos.x, pos.y), dim);
    }
    float width = Font::getCharWidth(c, char_size);
    Font::GZ_drawChar(c, pos.x + (dim.x - width) / 2, pos.y + (dim.y + char_size / 2) / 2,
                      is_button_down ? 0x00000060 : color, false, char_size);
}

void drawButtonEllipse(GZPad button, char c, uint32_t color, Vec2 pos, Vec2 dim, float char_size) {
    bool is_button_down = GZ_getButtonPressed(button);
    drawEllipseOutline(color, makeVec2(pos.x + dim.x / 2, pos.y + dim.y / 2), dim, OUTLINE_WIDTH);
    if (is_button_down) {
        drawEllipse(color, makeVec2(pos.x + dim.x / 2, pos.y + dim.y / 2), dim);
    }
    float width = Font::getCharWidth(c, char_size);
    Font::GZ_drawChar(c, pos.x + (dim.x - width) / 2, pos.y + (dim.y + char_size / 2) / 2,
                      is_button_down ? 0x00000060 : color, false, char_size);
}

void drawCross(uint32_t color, Vec2 pos, float size, float x_ratio) {
    float branch_width = 3.f * size / 11.f;
    float branch_length = 4.f * size / 11.f;
    drawButton(DPAD_LEFT, '-', color, makeVec2(pos.x, pos.y + branch_length),
               makeVec2(branch_length * x_ratio, branch_width), 8 * size / 25);
    drawButton(DPAD_UP, '|', color, makeVec2(pos.x + branch_length * x_ratio, pos.y),
               makeVec2(branch_width * x_ratio, branch_length), 4 * size / 25);
    drawButton(DPAD_RIGHT, '-', color,
               makeVec2(pos.x + (branch_length + branch_width) * x_ratio, pos.y + branch_length),
               makeVec2(branch_length * x_ratio, branch_width), 8 * size / 25);
    drawButton(DPAD_DOWN, '|', color,
               makeVec2(pos.x + branch_length * x_ratio, pos.y + branch_length + branch_width),
               makeVec2(branch_width * x_ratio, branch_length), 4 * size / 25);
}

void InputViewer::drawViewer(Vec2 pos, float scale, bool is_shadow, bool wide_screen) {
    float x_ratio = wide_screen ? 0.75f : 1.0f;
#ifdef GCN_PLATFORM
    drawButton(A, 'A', is_shadow ? 0x00000060 : 0x00FF7fFF,
               makeVec2(pos.x + 130.f * scale, pos.y + 30.f * scale), makeVec2(30.f * scale, 30.f * scale),
               8 * scale);
    drawButton(B, 'B', is_shadow ? 0x00000060 : 0xFF0000FF,
               makeVec2(pos.x + 108.5f * scale, pos.y + 45.f * scale), makeVec2(13.f * scale, 13.f * scale),
               8 * scale);
    drawButton(X, 'X', is_shadow ? 0x00000060 : 0xFFFFFFFF,
               makeVec2(pos.x + 167.5f * scale, pos.y + 30.f * scale), makeVec2(15.f * scale, 30.f * scale),
               8 * scale);
    drawButton(Y, 'Y', is_shadow ? 0x00000060 : 0xFFFFFFFF,
               makeVec2(pos.x + 130.f * scale, pos.y + 7.5f * scale), makeVec2(30.f * scale, 15.f * scale),
               8 * scale);
    drawButton(Z, 'Z', is_shadow ? 0x00000060 : 0x8A2BE2FF,
               makeVec2(pos.x + 167.5f * scale, pos.y + 7.5f * scale), makeVec2(15.f * scale, 15.f * scale),
               8 * scale);
    drawButton(START, ' ', is_shadow ? 0x00000060 : 0xFFFFFFFF,
               makeVec2(pos.x + 87.5f * scale, pos.y + 40.f * scale), makeVec2(10.f * scale, 10.f * scale),
               8 * scale);
    drawCross(is_shadow ? 0x00000060 : 0xFFFFFFFF, makeVec2(pos.x + 95.f * scale, pos.y + 10.f * scale),
              25.f * scale, x_ratio);

    // analog sticks
    drawStickOutline(is_shadow ? 0x00000060 : 0xFFFFFFFF,
                     makeVec2(pos.x + 17.5f * scale, pos.y + 30.f * scale), 35.0f * scale, x_ratio);
    drawStickOutline(is_shadow ? 0x00000060 : 0xFFD138FF,
                     makeVec2(pos.x + 62.5f * scale, pos.y + 30.f * scale), 35.0f * scale, x_ratio);
    drawEllipse(is_shadow ? 0x00000060 : 0xFFFFFFFF,
                makeVec2(pos.x + (17.5f + JUTGamePad::mPadMStick[0].mPosX * 10) * scale, pos.y + (30.f - JUTGamePad::mPadMStick[0].mPosY * 10) * scale),
                makeVec2(20.0f * scale, 20.0f * scale));
    drawEllipse(is_shadow ? 0x00000060 : 0xFFD138FF,
                makeVec2(pos.x + (62.5f + JUTGamePad::mPadSStick[0].mPosX * 10) * scale, pos.y + (30.f - JUTGamePad::mPadSStick[0].mPosY * 10) * scale),
                makeVec2(20.0f * scale, 20.0f * scale));

    // Analog triggers
    Draw::drawRectOutline(is_shadow ? 0x00000060 : 0xFFFFFFFF, makeVec2(pos.x, pos.y),
                          makeVec2(35.f * scale, 7.f * scale), OUTLINE_WIDTH);
    Draw::drawRect(is_shadow ? 0x00000060 : GZ_getButtonPressed(L) ? 0x00FF00FF : 0xFFFFFFFF,
                   makeVec2(pos.x, pos.y), makeVec2(35.f * JUTGamePad::mPadButton[0].mAnalogLf * scale, 7.f * scale));
    Draw::drawRectOutline(is_shadow ? 0x00000060 : 0xFFFFFFFF, makeVec2(pos.x + 45.f * scale, pos.y),
                          makeVec2(35.f * scale, 7.f * scale), OUTLINE_WIDTH);
    Draw::drawRect(is_shadow ? 0x00000060 : GZ_getButtonPressed(R) ? 0x00FF00FF : 0xFFFFFFFF,
                   makeVec2(pos.x + (45.f + 35.f * (1 - JUTGamePad::mPadButton[0].mAnalogRf)) * scale, pos.y),
                   makeVec2(35.f * JUTGamePad::mPadButton[0].mAnalogRf * scale, 7.f * scale));

    // stick inputs
    char control_x[5];  // control stick x
    char control_y[5];  // control stick y
    char c_x[5];        // c stick x
    char c_y[5];        // c stick y

    sprintf(control_x, "%3d", JUTGamePad::mPadStatus[0].stickX);
    sprintf(control_y, "%3d", JUTGamePad::mPadStatus[0].stickY);
    sprintf(c_x, "%3d", JUTGamePad::mPadStatus[0].substickX);
    sprintf(c_y, "%3d", JUTGamePad::mPadStatus[0].substickY);

    Font::GZ_drawStr(control_x, pos.x, pos.y + 65.f * scale, is_shadow ? 0x00000060 : 0xFFFFFFFF,
                     false, 13 * scale);
    Font::GZ_drawStr(control_y, pos.x + 23.f * scale, pos.y + 65.f * scale,
                     is_shadow ? 0x00000060 : 0xFFFFFFFF, false, 13 * scale);
    Font::GZ_drawStr(c_x, pos.x + 45.f * scale, pos.y + 65.f * scale,
                     is_shadow ? 0x00000060 : 0xFFD138FF, false, 13 * scale);
    Font::GZ_drawStr(c_y, pos.x + 70.f * scale, pos.y + 65.f * scale,
                     is_shadow ? 0x00000060 : 0xFFD138FF, false, 13 * scale);
#endif
#ifdef WII_PLATFORM
    drawButtonEllipse(C, 'c', is_shadow ? 0x00000060 : 0xFFFFFFFF,
                      makeVec2(pos.x + (42.5f * scale) * x_ratio, pos.y + 15.f * scale),
                      makeVec2(15.f * scale * x_ratio, 10.f * scale), 8 * scale);
    drawButton(Z, 'Z', is_shadow ? 0x00000060 : 0xFFFFFFFF,
               makeVec2(pos.x + 40.f * scale * x_ratio, pos.y + 30.f * scale),
               makeVec2(20.f * scale * x_ratio, 15.f * scale), 8 * scale);
    drawButtonEllipse(A, 'A', is_shadow ? 0x00000060 : 0xBFBFBFFF,
                      makeVec2(pos.x + 70.f * scale * x_ratio, pos.y + 42.5f * scale),
                      makeVec2(15.f * scale * x_ratio, 15.f * scale), 8 * scale);
    drawButton(B, 'B', is_shadow ? 0x00000060 : 0xFFFFFFFF,
               makeVec2(pos.x + 95.f * scale * x_ratio, pos.y + 40.f * scale),
               makeVec2(10.f * scale * x_ratio, 20.f * scale), 8 * scale);
    drawButtonEllipse(MINUS, '-', is_shadow ? 0x00000060 : 0xFFFFFFFF,
                      makeVec2(pos.x + 115.f * scale * x_ratio, pos.y + 12.5f * scale),
                      makeVec2(10.f * scale * x_ratio, 10.f * scale), 8 * scale);
    drawButtonEllipse(HOME, ' ', is_shadow ? 0x00000060 : 0x00BFFFFF,
                      makeVec2(pos.x + 130.f * scale * x_ratio, pos.y + 12.5f * scale),
                      makeVec2(10.f * scale * x_ratio, 10.f * scale), 8 * scale);
    drawButtonEllipse(PLUS, '+', is_shadow ? 0x00000060 : 0xFFFFFFFF,
                      makeVec2(pos.x + 145.f * scale * x_ratio, pos.y + 12.5f * scale),
                      makeVec2(10.f * scale * x_ratio, 10.f * scale), 8 * scale);
    drawButtonEllipse(ONE, '1', is_shadow ? 0x00000060 : 0xFFFFFFFF,
                      makeVec2(pos.x + 127.5f * scale * x_ratio, pos.y + 30.f * scale),
                      makeVec2(15.f * scale * x_ratio, 15.f * scale), 8 * scale);
    drawButtonEllipse(TWO, '2', is_shadow ? 0x00000060 : 0xFFFFFFFF,
                      makeVec2(pos.x + 127.5f * scale * x_ratio, pos.y + 50.f * scale),
                      makeVec2(15.f * scale * x_ratio, 15.f * scale), 8 * scale);
    drawCross(is_shadow ? 0x00000060 : 0xFFFFFFFF,
              makeVec2(pos.x + 75.f * scale * x_ratio, pos.y + 10.f * scale), 25.f * scale, x_ratio);

    // analog sticks
    drawStickOutline(is_shadow ? 0x00000060 : 0xFFFFFFFF,
                     makeVec2(pos.x + 17.5f * scale * x_ratio, pos.y + 30.f * scale), 35.0f * scale,
                     x_ratio);
    drawEllipse(is_shadow ? 0x00000060 : 0xFFFFFFFF,
                makeVec2(pos.x + (17.5f + GZ_getWiiPadStatus().ex_status.fs.stick.x * 10) * scale * x_ratio, pos.y + (30.f - GZ_getWiiPadStatus().ex_status.fs.stick.y * 10) * scale),
                makeVec2(20.0f * scale * x_ratio, 20.0f * scale));

    // stick inputs
    char control_x[5];  // control stick x
    char control_y[5];  // control stick y

    snprintf(control_x, sizeof(control_x), "%3d", (int8_t)(GZ_getWiiPadStatus().ex_status.fs.stick.x * 99.f));
    snprintf(control_y, sizeof(control_y), "%3d", (int8_t)(GZ_getWiiPadStatus().ex_status.fs.stick.y * 99.f));

    Font::GZ_drawStr(control_x, pos.x, pos.y + 65.f * scale, is_shadow ? 0x00000060 : 0xFFFFFFFF,
                     false, 10 * scale);
    Font::GZ_drawStr(control_y, pos.x + 23.f * scale * x_ratio, pos.y + 65.f * scale,
                     is_shadow ? 0x00000060 : 0xFFFFFFFF, false, 10 * scale);

    // Analog triggers
    Draw::drawRectOutline(is_shadow ? 0x00000060 : 0xFFFFFFFF, makeVec2(pos.x, pos.y),
                          makeVec2(35.f * scale * x_ratio, 7.f * scale), OUTLINE_WIDTH);
    Draw::drawRect(is_shadow ? 0x00000060 : GZ_getWiiPadStatus().ex_status.fs.acc_speed > 0.2 ? 0x00FF00FF : 0xFFFFFFFF,
                   makeVec2(pos.x, pos.y),
                   makeVec2(35.f * MIN(1.0f, GZ_getWiiPadStatus().ex_status.fs.acc_speed) * scale * x_ratio, 7.f * scale));
    Draw::drawRectOutline(is_shadow ? 0x00000060 : 0xFFFFFFFF,
                          makeVec2(pos.x + 117.5f * scale * x_ratio, pos.y),
                          makeVec2(35.f * scale * x_ratio, 7.f * scale), OUTLINE_WIDTH);
    Draw::drawRect(
        is_shadow ? 0x00000060 : GZ_getWiiPadStatus().acc_speed > 0.2 ? 0x00FF00FF : 0xFFFFFFFF,
        makeVec2(pos.x + (117.5f + 35.f * (1 - MIN(1.0f, GZ_getWiiPadStatus().acc_speed))) * scale * x_ratio, pos.y),
        makeVec2(35.f * MIN(1.0f, GZ_getWiiPadStatus().acc_speed) * scale * x_ratio, 7.f * scale));

#endif
}

#include "game_state.h"

KEEP_FUNC void InputViewer::draw() {
    Vec2 pos = GZ_getSpriteOffset(STNG_SPRITES_INPUT_VIEWER);
#ifdef GCN_PLATFORM
    float scale = 1.0f;
#elif defined(WII_PLATFORM)
    float scale = 1.4f;
#endif
    if (GZ_checkDropShadows())
        drawViewer(makeVec2(pos.x + 1.f, pos.y + 1.f), scale, true, isWidescreen);
    drawViewer(pos, scale, false, isWidescreen);
}
