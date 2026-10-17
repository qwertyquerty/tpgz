#include "menus/menu.h"

struct HiddenSkillsData {
    bool l_ebFlag;
    bool l_sbFlag;
    bool l_bsFlag;
    bool l_hsFlag;
    bool l_mdFlag;
    bool l_jsFlag;
    bool l_gsFlag;
};

enum HiddenSkillsIndex {
    ENDING_BLOW_INDEX,
    SHIELD_BASH_INDEX,
    BACKSLICE_INDEX,
    HELM_SPLITTER_INDEX,
    MORTAL_DRAW_INDEX,
    JUMP_STRIKE_INDEX,
    GREAT_SPIN_INDEX,

    HIDDEN_SKILLS_INDEX_COUNT
};

extern HiddenSkillsData* hiddenSkillsData;

class HiddenSkillsMenu : public Menu {
public:
    HiddenSkillsMenu(Cursor&);
    virtual ~HiddenSkillsMenu();
    virtual void draw();

private:
};