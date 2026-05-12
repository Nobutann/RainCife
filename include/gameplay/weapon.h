#ifndef WEAPON_H
#define WEAPON_H

struct Player;
typedef struct Player Player;

typedef enum
{
    WEAPON_BAT,
    WEAPON_PISTOL,
    WEAPON_HAMMER
} WeaponType;

typedef struct 
{
    WeaponType type;
    float damage;
    float cooldown;
    float cooldownTimer;
    int breakPower;
    bool attacking;
    bool hitConnected;
    float attackDuration;
    float attackTimer;
    void (*attack) (struct Player *player);
} Weapon;

void UseBat(struct Player *player);
void UseHammer(struct Player *player);
void UsePistol(struct Player *player);
void EquipWeapon(struct Player *player, WeaponType type);
void UseWeapon(struct Player *player);

#endif
