#ifndef TPGZ_MODULES_MENUS_MENU_RUPEE_FLAGS_INCLUDE_RUPEE_FLAGS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_RUPEE_FLAGS_INCLUDE_RUPEE_FLAGS_MENU_H
#include "menus/menu.h"

struct RupeeFlagsData {
    u16 l_donationAmount;
    u16 l_fundraisingAmount;
    bool l_fundraising1;
    bool l_fundraising2;
    bool l_rupeeFlag;
};

enum GeneralFlagsIndex {
    DONATION_AMT_INDEX,
    FUNDRAISING_AMT_INDEX,
    FUNDRAISING_1_INDEX,
    FUNDRAISING_2_INDEX,
    RUPEE_CS_FLAG_INDEX,

    RUPEE_FLAGS_COUNT
};

extern RupeeFlagsData* rupeeFlagsData;

class RupeeFlagsMenu : public Menu {
public:
    RupeeFlagsMenu(Cursor&);
    virtual ~RupeeFlagsMenu();
    virtual void draw();

private:
};

#endif
