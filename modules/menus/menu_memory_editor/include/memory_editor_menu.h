#include "menus/menu.h"

class MemoryEditorMenu : public Menu {
public:
    MemoryEditorMenu(Cursor&);
    virtual ~MemoryEditorMenu();
    virtual void draw();

private:
    uint8_t line_length;

    void drawMemEditor();
};
