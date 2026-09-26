#ifndef TPGZ_MODULES_BOOT_INCLUDE_UTILS_LINES_H
#define TPGZ_MODULES_BOOT_INCLUDE_UTILS_LINES_H
#include "font.h"
#include <cstdio>
#include <cstdarg>

#define MAX_RENDER_LINES 15

#define MAX_LIST_MEMBER_LENGTH 40
#define MAX_LIST_ITEMS 20
#define MAX_LINE_LENGTH 50
#define MAX_DESCRIPTION_LENGTH 100

extern uint32_t g_cursorColor;

struct ListMember {
    char member[MAX_LIST_MEMBER_LENGTH];
};

struct RngPresetMember {
    char member[MAX_LIST_MEMBER_LENGTH];
    int r0;
    int r1;
    int r2;
};

struct Line {
    char line[50];
    uint32_t idx;
    char description[MAX_DESCRIPTION_LENGTH];
    bool toggleable;
    bool (*active)();
    uint8_t max_y_cursor_options;
    char value[50];
    bool disabled;

    int printf(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        int ret = vsnprintf(value, sizeof(value), fmt, args);
        va_end(args);
        return ret;
    }
};

float maxF(float a, float b);
float minF(float a, float b);
void menu_anim(int idx);
void GZ_drawMenuLines(Line input_lines[], uint32_t cursor, uint32_t LINES);
void GZ_drawRngLines(Line input_lines[], uint32_t cursor, uint32_t LINES);

#endif
