#include <raylib.h>
#include "entities/enemy.h"

void InitEnemy(Enemy *enemy, EnemyType type, int screenWidth, int screenHeight, int baseSpeed)
{
    enemy->type = type;
    enemy->active = true;
    enemy->state = 0;
    enemy->velocity.x = 0;
    enemy->velocity.y = 0;

    switch (type)
    {
        case ENEMY_BIRD1:
            enemy->position = (Vector2){(float)screenWidth, screenHeight * 0.4f};
            enemy->size = (Vector2){100, 100};
            break;
        case ENEMY_BIRD2:
            enemy->position = (Vector2){(float)screenWidth, screenHeight * 0.35f};
            enemy->size = (Vector2){100, 100};
            enemy->velocity.y = 7.0f;
            break;
        case ENEMY_BIKE:
            enemy->position = (Vector2){(float)screenWidth, screenHeight * 0.785f - 140};
            enemy->size = (Vector2){140, 140};
            break;
        case ENEMY_WOOD:
            enemy->position = (Vector2){(float)screenWidth, screenHeight * 0.785f - 140};
            enemy->size = (Vector2){280, 140};
            break;
        case ENEMY_POSTE:
            enemy->basePosition = (Vector2){(float)screenWidth, screenHeight * 0.785f - 450};
            enemy->position = enemy->basePosition;
            enemy->size = (Vector2){150, 150}; // Only head has hitbox
            enemy->headDetached = false;
            break;
        case ENEMY_FISH:
            enemy->position = (Vector2){(float)screenWidth * 0.42f, screenHeight * 0.80f - 120};
            enemy->size = (Vector2){120, 120};
            enemy->velocity.y = -18.0f;
            enemy->velocity.x = -(4.0f + baseSpeed);
            break;
    }
}

void UpdateEnemy(Enemy *enemy, int screenWidth, int screenHeight, int baseSpeed, Rectangle playerHitbox)
{
    if (!enemy->active)
    {
        return;
    }

    switch(enemy->type)
    {
        case ENEMY_BIRD1:
            enemy->position.x -= (7 + baseSpeed);
            if (enemy->position.x < -50)
            {
                enemy->active = false;
            } 
            break;

        case ENEMY_BIRD2:
            enemy->position.x -= (8 + baseSpeed);
            enemy->position.y += enemy->velocity.y;
            if (enemy->position.y < screenHeight * 0.20f) 
            {
                enemy->velocity.y = 7.0f;
            }    
            else if (enemy->position.y > screenHeight * 0.70f) 
            {
                enemy->velocity.y = -7.0f;
            }
            if (enemy->position.x < -50)
            {
                enemy->active = false;
            }
            break;

        case ENEMY_BIKE:
            enemy->position.x -= (7 + baseSpeed);
        if (enemy->position.x < -70) 
        {
            enemy->active = false;
        }
            break;
        case ENEMY_WOOD:
            enemy->position.x -= (7 + baseSpeed);
            if (enemy->position.x < -140)
            {
                enemy->active = false;
            }
            break;

        case ENEMY_POSTE:
            enemy->basePosition.x -= (7 + baseSpeed);
            if (!enemy->headDetached)
            {
                enemy->position = enemy->basePosition;
                if (enemy->position.x - playerHitbox.x < 350.0f && enemy->position.x > playerHitbox.x)
                {
                    enemy->headDetached = true;
                    enemy->velocity.x = -8.0f;
                    enemy->velocity.y = -5.0f; // Jump up a bit then fall
                }
            }
            else
            {
                enemy->position.x += enemy->velocity.x;
                enemy->position.y += enemy->velocity.y;
                enemy->velocity.y += 0.4f; // Gravity
            }

            if (enemy->basePosition.x < -150 && enemy->position.y > screenHeight + 100)
            {
                enemy->active = false;
            }
            break;

        case ENEMY_FISH:
            enemy->position.x += enemy->velocity.x;
            enemy->position.y += enemy->velocity.y;
            enemy->velocity.y += 0.55f;
            if (enemy->position.y > screenHeight || enemy->position.x < -100)
            {
                enemy->active = false;
            }
            break;
    }
}

void DrawEnemy(Enemy *enemy, Texture2D *textures, Texture2D extraTex1, Texture2D extraTex2)
{
    if (!enemy->active)
    {
        return;
    }

    if (enemy->type == ENEMY_POSTE)
    {
        if (!enemy->headDetached)
        {
            // Draw complete post
            if (textures[ENEMY_POSTE].id > 0)
            {
                Rectangle source = { 0.0f, 0.0f, (float)textures[ENEMY_POSTE].width, (float)textures[ENEMY_POSTE].height };
                Rectangle dest = { enemy->basePosition.x, enemy->basePosition.y, 150.0f, 450.0f };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(textures[ENEMY_POSTE], source, dest, origin, 0.0f, WHITE);
            }
        }
        else
        {
            // Draw headless body
            if (extraTex1.id > 0)
            {
                Rectangle source = { 0.0f, 0.0f, (float)extraTex1.width, (float)extraTex1.height };
                // Headless body starts a bit lower to overlap/match the missing head
                Rectangle dest = { enemy->basePosition.x, enemy->basePosition.y + 75.0f, 150.0f, 375.0f };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(extraTex1, source, dest, origin, 0.0f, WHITE);
            }
            
            // Draw head
            if (extraTex2.id > 0)
            {
                Rectangle source = { 0.0f, 0.0f, (float)extraTex2.width, (float)extraTex2.height };
                Rectangle dest = { enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(extraTex2, source, dest, origin, 0.0f, WHITE);
            }
        }
        return; // Done drawing poste
    }
    
    Texture2D texture = textures[enemy->type];
    if (texture.id > 0)
    {
        Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        Rectangle dest = { enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
    }
    else
    {
        DrawRectangleV(enemy->position, enemy->size, RED);
    }
}