#include <raylib.h>
#include "gameplay/weapon.h"
#include "entities/player.h"
#include "graphics/sprites.h"
#include "core/screens.h"

static const char *GetPlayerGunSpritePath(int characterId)
{
    static const char *gunSpritePaths[] =
    {
        "assets/sprites/Player/Spr_Mouse/Spr_rat/Mouse_Arm_gun.png",
        "assets/sprites/Player/Spr_Capibara/Capibara_Arm_gun.png",
        "assets/sprites/Player/Spr_Guaiamum/Guaiamum_Arm_gun.png"
    };
    int count = (int)(sizeof(gunSpritePaths) / sizeof(gunSpritePaths[0]));
    int index = characterId - 1;

    if (index < 0 || index >= count)
    {
        index = 0;
    }
    return gunSpritePaths[index];
}

static void UseRunningAttackPose(Player *player)
{
    LayeredAnimation *runningAnim = player->currentAnim;

    if (!player->isBossFighting && player->onGround && player->velocity.x != 0 && runningAnim && runningAnim->layerCount >= 3)
    {
        player->sprites.attack.layers[0] = runningAnim->layers[0];
        player->sprites.attack.layers[2] = runningAnim->layers[2];
        return;
    }

    player->sprites.attack.layers[0] = player->sprites.walkFront.layers[0];
    player->sprites.attack.layers[0].currentFrame = 0;
    player->sprites.attack.layers[0].timer = 0.0f;
    player->sprites.attack.layers[2] = player->sprites.walkFront.layers[2];
    player->sprites.attack.layers[2].currentFrame = 0;
    player->sprites.attack.layers[2].timer = 0.0f;
}

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
        UseRunningAttackPose(player);
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
        UseRunningAttackPose(player);
    }

    player->currentAnim = &player->sprites.attack;
    player->sprites.attack.layers[1].currentFrame = 0;
    player->sprites.attack.layers[1].timer = 0.0f;
}

void UsePistol(Player *player)
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
        UseRunningAttackPose(player);
    }

    player->currentAnim = &player->sprites.attack;
    player->sprites.attack.layers[1].currentFrame = 0;
    player->sprites.attack.layers[1].timer = 0.0f;
}

void EquipWeapon(Player *player, WeaponType type)
{
    UnloadAttackAnimation(&player->sprites);

    int characterId = GetSelectedCharacterId();
    const PlayerSpriteSet *sprites = GetPlayerSpriteSet(characterId);
    bool clothed = GetSelectedClothingId() == 2;

    player->weapon.type = type;
    player->weapon.cooldownTimer = 0.0f;
    player->weapon.hitConnected = false;
    float swordCooldown = player->isBossFighting ? 0.5f : 0.85f;

    switch (type)
    {
        case WEAPON_BAT:
            player->weapon.damage = 4.0f;
            player->weapon.cooldown = swordCooldown;
            player->weapon.breakPower = 1;
            player->weapon.attack = UseBat;
            player->weapon.attackDuration = LoadAttackAnimation(
                &player->sprites,
                characterId,
                clothed ? sprites->attackSwordClothed : sprites->attackSword,
                5,
                0.08f
            );
            break;
        case WEAPON_HAMMER:
            player->weapon.damage = 8.0f;
            player->weapon.cooldown = swordCooldown * 2.0f;
            player->weapon.breakPower = 3;
            player->weapon.attack = UseHammer;
            player->weapon.attackDuration = LoadAttackAnimation(
                &player->sprites,
                characterId,
                clothed ? sprites->attackHammerClothed : sprites->attackHammer,
                5,
                0.08f
            );
            break;
        case WEAPON_PISTOL:
            player->weapon.damage = 3.0f;
            player->weapon.cooldown = 0.45f;
            player->weapon.breakPower = 1;
            player->weapon.attack = UsePistol;
            player->weapon.attackDuration = LoadAttackAnimation(
                &player->sprites,
                characterId,
                GetPlayerGunSpritePath(characterId),
                1,
                0.12f
            );
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

bool CanWeaponBreakEnemy(WeaponType weaponType, EnemyType enemyType)
{
    if (enemyType == ENEMY_SAFE_POSTE)
        return false;
    if (weaponType == WEAPON_HAMMER)
        return true;
    return enemyType == ENEMY_BIRD1 ||
           enemyType == ENEMY_BIRD2 ||
           enemyType == ENEMY_FISH  ||
           enemyType == ENEMY_POSTE;
}
