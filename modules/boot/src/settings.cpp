#include "defines.h"
#include "settings.h"
#include <cstdio>
#include <algorithm>

ListMember g_font_opt[] = {"consola",   "calamity-bold",  "lib-sans",      "lib-sans-bold",
                           "lib-serif", "lib-serif-bold", "press-start-2p"};

tpgz::containers::deque<GZSettingEntry*> g_settings;

KEEP_FUNC void GZStng_add(GZSettingID id, void* data, size_t size) {
    tpgz::containers::deque<GZSettingEntry*>::iterator it = g_settings.begin();
    for (; it != g_settings.end(); ++it) {
        if ((*it)->id == id) {
            break;
        }
    }
    if (it == g_settings.end()) {
        GZSettingEntry* entry = new GZSettingEntry(id, size, data);
        g_settings.push_back(entry);
    } else {
        GZSettingEntry* entry = *it;
        void* old_data = entry->data;
        delete[] (uint8_t*)old_data;
        entry->data = data;
        entry->size = size;
    }
}

KEEP_FUNC void GZStng_remove(GZSettingID id) {
    tpgz::containers::deque<GZSettingEntry*>::iterator it = g_settings.begin();
    for (; it != g_settings.end(); ++it) {
        if ((*it)->id == id) {
            break;
        }
    }
    if (it != g_settings.end()) {
        GZSettingEntry* entry = *it;
        void* data = entry->data;
        delete[] (uint8_t*)data;
        g_settings.erase(it);
        delete entry;
    }
}

KEEP_FUNC GZSettingEntry* GZStng_get(GZSettingID id) {
    tpgz::containers::deque<GZSettingEntry*>::iterator it = g_settings.begin();
    for (; it != g_settings.end(); ++it) {
        if ((*it)->id == id) {
            break;
        }
    }
    GZSettingEntry* entry = NULL;
    if (it != g_settings.end()) {
        entry = *it;
    }
    return entry;
}

KEEP_FUNC tpgz::containers::deque<GZSettingID>* GZStng_getList() {
    tpgz::containers::deque<GZSettingID>* list = new tpgz::containers::deque<GZSettingID>;
    for (tpgz::containers::deque<GZSettingEntry*>::iterator it = g_settings.begin(); it != g_settings.end(); ++it) {
        list->push_back((*it)->id);
    }
    return list;
}

void GZ_initFont() {
    uint32_t fontType = GZStng_getData(STNG_FONT, 0);
    if (fontType >= 0 && fontType < FONT_OPTIONS_COUNT) {
        char buf[40] = {0};
        snprintf(buf, sizeof(buf), "tpgz/fonts/%s.fnt", g_font_opt[fontType].member);
        Font::loadFont(buf);
    }
}