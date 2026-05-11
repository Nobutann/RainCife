#include <raylib.h>
#include "gameplay/weapon.h"
#include "entities/player.h"
#include "graphics/sprites.h"
#include <stdio.h>

void UseBat(Player *player)
{
    bool isIdle = (player->isBossFighting && player->velocity.x == 0);

    if (isIdle)
    {
        player->sprites.attack.layers[0] = player->sprites.idleLegs.layers[0];
        player->sprites.attack.layers[0].currentFrame = 0;
        player->sprites.attack.layers[0].timer = 0.0f;
        player->sprites.attack.layers[2] = player->sprites.idleHead.layers[0];
        player->sprites.attack.layers[2].currentFrame = 0;
        player->sprites.attack.layers[2].timer = 0.0f;
    }
    else
    {
        player->sprites.attack.layers[0] = player->sprites.walkFront.layers[0];
        player->sprites.attack.layers[0].currentFrame = 0;
        player->sprites.attack.layers[0].timer = 0.0f;
        player->sprites.attack.layers[2] = player->sprites.walkFront.layers[2];
        player->sprites.attack.layers[2].currentFrame = 0;
        player->sprites.attack.layers[2].timer = 0.0f;
    }

    player->currentAnim = &player->sprites.attack;
    player->sprites.attack.layers[1].currentFrame = 0;
    player->sprites.attack.layers[1].timer = 0.0f;
}

void UseHammer(Player *player)
{
    
    bool isIdle = (player->isBossFighting && player->velocity.x == 0);

    if (isIdle)
    {
        player->sprites.attack.layers[0] = player->sprites.idleLegs.layers[0];
        player->sprites.attack.layers[0].currentFrame = 0;
        player->sprites.attack.layers[0].timer = 0.0f;
        player->sprites.attack.layers[2] = player->sprites.idleHead.layers[0];
        player->sprites.attack.layers[2].currentFrame = 0;
        player->sprites.attack.layers[2].timer = 0.0f;
    }
    else
    {
        player->sprites.attack.layers[0] = player->sprites.walkFront.layers[0];
        player->sprites.attack.layers[0].currentFrame = 0;
        player->sprites.attack.layers[0].timer = 0.0f;
        player->sprites.attack.layers[2] = player->sprites.walkFront.layers[2];
        player->sprites.attack.layers[2].currentFrame = 0;
        player->sprites.attack.layers[2].timer = 0.0f;
    }
    
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
    player->weapon.hitConnected = false;

    switch (type)
    {
        case WEAPON_BAT:
            player->weapon.damage = 2.0f;
            if (player->isBossFighting)
            {
               player->weapon.cooldown = 0.5f; 
            }
            else
            {
                player->weapon.cooldown = 0.85f;
            }
            player->weapon.breakPower = 1;
            player->weapon.attack = UseBat;
            player->weapon.attackDuration = LoadAttackAnimation(&player->sprites, "assets/sprites/Player/attack/melee/Attack_sword-Sheet.png", 5, 0.08f);
            break;
        case WEAPON_HAMMER:
            player->weapon.damage = 2.0f;
            player->weapon.cooldown = 1.5f;
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
    player->weapon.hitConnected = false;
    player->weapon.attackTimer = player->weapon.attackDuration;
    player->weapon.attack(player);
}
