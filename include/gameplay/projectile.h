#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <raylib.h>

#define MAX_PROJECTILES 16
#define PROJECTILE_SPEED 1200.0f

typedef struct 
{
    Vector2 position;
    Vector2 velocity;
    float angle;
    bool active;
} Projectile;

typedef struct 
{
    Projectile items[MAX_PROJECTILES];
    Texture2D sprites;
} ProjectileSystem;

void InitProjectileSystem(ProjectileSystem *ps);
void UnloadProjectileSystem(ProjectileSystem *ps);
void SpawnProjectile(ProjectileSystem *ps, Vector2 origin, Vector2 target);
void UpdateProjectile(ProjectileSystem *ps, float dt, int screenWidth, int screenHeight);
Rectangle GetProjectileHitbox(Projectile *p);
void DrawProjectiles(ProjectileSystem *ps);

#endif