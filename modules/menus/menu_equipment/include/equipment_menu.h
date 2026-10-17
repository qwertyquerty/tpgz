#include "menus/menu.h"

struct EquipmentData {
    uint8_t l_ordonSword_idx;
    uint8_t l_masterSword_idx;
    uint8_t l_woodShield_idx;
    uint8_t l_hyShield_idx;
    uint8_t l_tunic_idx;
    uint8_t l_zoraArmor_idx;
    uint8_t l_magicArmor_idx;
    uint8_t l_bombCap_idx;
    uint8_t l_wallet_idx;
    uint8_t l_arrowCap_idx;
};

enum EquipmentIndex {
    ORDON_SWORD_INDEX,
    MASTER_SWORD_INDEX,
    WOOD_SHIELD_INDEX,
    HYLIAN_SHIELD_INDEX,
    HERO_TUNIC_INDEX,
    ZORA_ARMOR_INDEX,
    MAGIC_ARMOR_INDEX,
    BOMB_CAPACITY_INDEX,
    WALLET_INDEX,
    ARROW_CAPACITY_INDEX,

    EQUIPMENT_INDEX_COUNT
};

extern EquipmentData* equipmentData;

class EquipmentMenu : public Menu {
public:
    EquipmentMenu(Cursor&);
    virtual ~EquipmentMenu();
    virtual void draw();

private:

    void resetIndex();
    void getEquipment();
    void setEquipment();
};