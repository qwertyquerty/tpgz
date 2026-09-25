#include "menus/menu.h"

struct PauseData {
    uint8_t l_scent_idx;
};

enum PauseIndex {
    EQUIPMENT_INDEX,
    GOLDEN_BUG_INDEX,
    HIDDEN_SKILL_INDEX,
    SCENT_INDEX,

    PAUSE_INDEX_COUNT
};

extern PauseData* pauseData;

class PauseMenu : public Menu {
public:
    PauseMenu(Cursor&);
    virtual ~PauseMenu();
    virtual void draw();

private:
};