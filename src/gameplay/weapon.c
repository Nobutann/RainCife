#include <raylib.h>
#include "gameplay/weapon.h"
#include "entities/player.h"
#include "graphics/sprites.h"

void UseBat(Player *player)
{
    player->currentAnim = &player->sprites.attack;
    player->sprites.attack.layers[1].currentFrame = 0;
    player->sprites.attack.layers[1].timer = 0.0f;
}

void UseHammer(Player *player)
{
    player->currentAnim = &player->sprites.attack;
    player->sprites.attack.layers[1].currentFrame = 0;
    player->sprites.attack.layers[1].timer = 0.0f;
}

void UsePistol(Player *player)
{
    player->currentAnim = &player->sprites.attack;
    player->sprites.attack.layers[1].currentFrame = 0;
    player->sprites.attack.layers[1].timer = 0.0f;
}

void EquipWeapon(Player *player, WeaponType type)
{
    UnloadAttackAnimation(&player->sprites);

    player->weapon.type = type;
    player->weapon.cooldownTimer = 0.0f;

    switch (type)
    {
        case WEAPON_BAT:
            player->weapon.damage = 30.0f;
            player->weapon.cooldown = 0.2f;
            player->weapon.breakPower = 1;
            player->weapon.attack = UseBat;
            player->weapon.attackDuration = LoadAttackAnimation(&player->sprites, "assets/sprites/Player/attack/melee/Attack_sword-Sheet.png", 5, 0.08f);
            break;
        case WEAPON_HAMMER:
            player->weapon.damage = 30.0f;
            player->weapon.cooldown = 0.2f;
            player->weapon.breakPower = 3;
            player->weapon.attack = UseHammer;
            player->weapon.attackDuration = LoadAttackAnimation(&player->sprites, "assets/sprites/Player/attack/melee/Attack_hammer-Sheet.png", 5, 0.15f);
            break;
        case WEAPON_PISTOL:
            break;
        default:
            break;
    }
}

void UseWeapon(Player *player)
{
    if (player->weapon.cooldownTimer > 0)
    {
        return;
    }

    player->weapon.cooldownTimer = player->weapon.cooldown;
    player->weapon.attacking = true;
    player->weapon.attackTimer = player->weapon.attackDuration;
    player->weapon.attack(player);
}