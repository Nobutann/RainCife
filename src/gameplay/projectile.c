#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include "gameplay/projectile.h"

void InitProjectileSystem(ProjectileSystem *ps)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        ps->items[i].active = false;
    }

    ps->sprites = LoadTexture("assets/sprites/Player/attack/Pistol/Nerf_bullet.png");
}

void UnloadProjectileSystem(ProjectileSystem *ps)
{
    UnloadTexture(ps->sprites);
}

void SpawnProjectile(ProjectileSystem *ps, Vector2 origin, Vector2 target)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!ps->items[i].active)
        {
            Vector2 dir = Vector2Normalize(Vector2Subtract(target, origin));
            ps->items[i].position = origin;
            ps->items[i].velocity = Vector2Scale(dir, PROJECTILE_SPEED);
            ps->items[i].angle = atan2f(dir.y, dir.x) * RAD2DEG;
            ps->items[i].active = true;
            break;
        }
    }
}

void UpdateProjectile(ProjectileSystem *ps, float dt, int screenWidth, int screenHeight)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!ps->items[i].active)
        {
            continue;
        }

        ps->items[i].position.x += ps->items[i].velocity.x * dt;
        ps->items[i].position.y += ps->items[i].velocity.y * dt;

        if (ps->items[i].position.x < -100 || ps->items[i].position.x > screenWidth + 100 || ps->items[i].position.y < -100 || ps->items[i].position.y > screenHeight + 100)
        {
            ps->items[i].active = false;
        }
    }
}

Rectangle GetProjectileHitbox(Projectile *p)
{
    return (Rectangle){ p->position.x - 10, p->position.y - 6, 20, 12 };
}

void DrawProjectiles(ProjectileSystem *ps)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!ps->items[i].active)
        {
            continue;
        }

        Projectile *p = &ps->items[i];

        if (ps->sprites.id > 0)
        {
            float width = (float)ps->sprites.width;
            float height = (float)ps->sprites.height;
            Rectangle src = 
            {
                0,
                0,
                width,
                height
            };

            Rectangle dest = 
            {
                p->position.x,
                p->position.y,
                width,
                height
            };

            Vector2 origin = 
            {
                width * 0.5f,
                height * 0.5f
            };
            DrawTexturePro(ps->sprites, src, dest, origin, p->angle, WHITE);
        }
        else
        {
            DrawCircle((int)p->position.x, (int)p->position.y, 6, ORANGE);
        }
    }
}