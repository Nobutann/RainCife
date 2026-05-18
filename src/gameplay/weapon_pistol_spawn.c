#include <stddef.h>
#include "gameplay/projectile.h"
#include "gameplay/weapon.h"
#include "entities/player.h"

static ProjectileSystem *g_projectileSystem = NULL;

void SetProjectileSystem(ProjectileSystem *ps)
{
    g_projectileSystem = ps;
}

void SpawnPistolProjectile(Player *player)
{
    if (!g_projectileSystem)
    {
        return;
    }

    Vector2 origin = player->armPivot;
    Vector2 target = GetMousePosition();
    SpawnProjectile(g_projectileSystem, origin, target);
}