#include "menus/menu.h"
#include "d/d_save.h"

#define ITEM_WHEEL_SLOTS 24

struct ItemWheelData {
    unsigned int l_listIdx;
};

struct ItemLookup {
    enum ItemTable item_id;
    char name[40];
};

class ItemWheelMenu : public Menu {
public:
    ItemWheelMenu(Cursor&, ItemWheelData&);
    virtual ~ItemWheelMenu();
    virtual void draw();
    void updateListIdx();
    void fixSpecialItems(int i);

private:
    unsigned int& l_listIdx;

    Line lines[ITEM_WHEEL_SLOTS];
};