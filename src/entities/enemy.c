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
            enemy->size = (Vector2){280, 140};
            enemy->state = GetRandomValue(0, 2);
            break;
        case ENEMY_WOOD:
            enemy->position = (Vector2){(float)screenWidth, screenHeight * 0.785f - 140};
            enemy->size = (Vector2){280, 140};
            break;
        case ENEMY_POSTE:
            enemy->basePosition = (Vector2){(float)screenWidth, screenHeight * 0.785f - 450};
            enemy->position = enemy->basePosition;
            enemy->size = (Vector2){150, 150};
            enemy->headDetached = false;
            break;
        case ENEMY_FISH:
            enemy->basePosition = (Vector2){(float)screenWidth * 0.55f, screenHeight * 0.82f - 40};
            enemy->position = (Vector2){enemy->basePosition.x, (float)screenHeight + 200};
            enemy->size = (Vector2){120, 120};
            enemy->velocity.y = 0;
            enemy->velocity.x = -(4.0f + baseSpeed);
            enemy->state = 0;
            enemy->stateTimer = GetTime();
            break;
        case ENEMY_SAFE_POSTE:
            enemy->basePosition = (Vector2){(float)screenWidth, screenHeight * 0.785f - 450};
            enemy->position = enemy->basePosition;
            enemy->size = (Vector2){150, 450};
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
                    enemy->velocity.y = -5.0f;
                }
            }
            else
            {
                enemy->position.x += enemy->velocity.x;
                enemy->position.y += enemy->velocity.y;
                enemy->velocity.y += 0.4f;
            }

            if (enemy->basePosition.x < -150 && enemy->position.y > screenHeight + 100)
            {
                enemy->active = false;
            }
            break;

        case ENEMY_FISH:
            if (enemy->state == 0)
            {
                enemy->basePosition.x -= (7 + baseSpeed);
                enemy->position.x = enemy->basePosition.x;
                if (GetTime() - enemy->stateTimer > 0.8)
                {
                    enemy->state = 1;
                    enemy->position.y = enemy->basePosition.y;
                    enemy->velocity.y = -18.0f;
                }
            }
            else if (enemy->state == 1)
            {
                enemy->position.x += enemy->velocity.x;
                enemy->position.y += enemy->velocity.y;
                enemy->velocity.y += 0.55f;
            }
            
            if (enemy->position.y > screenHeight + 200 || enemy->position.x < -200)
            {
                enemy->active = false;
            }
            break;
            
        case ENEMY_SAFE_POSTE:
            enemy->position.x -= (7 + baseSpeed);
            if (enemy->position.x < -150)
            {
                enemy->active = false;
            }
            break;
    }
}

void DrawEnemy(Enemy *enemy, EnemyAssets *assets)
{
    if (!enemy->active)
    {
        return;
    }

    if (enemy->type == ENEMY_POSTE)
    {
        if (!enemy->headDetached)
        {
            if (assets->textures[ENEMY_POSTE].id > 0)
            {
                Rectangle source = { 0.0f, 0.0f, (float)assets->textures[ENEMY_POSTE].width, (float)assets->textures[ENEMY_POSTE].height };
                Rectangle dest = { enemy->basePosition.x, enemy->basePosition.y, 150.0f, 450.0f };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(assets->textures[ENEMY_POSTE], source, dest, origin, 0.0f, WHITE);
            }
        }
        else
        {
            if (assets->posteSemCabeca.id > 0)
            {
                Rectangle source = { 0.0f, 0.0f, (float)assets->posteSemCabeca.width, (float)assets->posteSemCabeca.height };
                Rectangle dest = { enemy->basePosition.x, enemy->basePosition.y + 75.0f, 150.0f, 375.0f };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(assets->posteSemCabeca, source, dest, origin, 0.0f, WHITE);
            }
            
            if (assets->posteCabecas.id > 0)
            {
                Rectangle source = { 0.0f, 0.0f, (float)assets->posteCabecas.width, (float)assets->posteCabecas.height };
                Rectangle dest = { enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(assets->posteCabecas, source, dest, origin, 0.0f, WHITE);
            }
        }
        return;
    }
    
    if (enemy->type == ENEMY_FISH)
    {
        if (enemy->state == 0)
        {
            UpdateAnimation(&assets->fishAnticipation, GetFrameTime());
            DrawAnimationFrame(&assets->fishAnticipation, enemy->basePosition, 2.5f, false, WHITE);
        }
        else if (enemy->state == 1)
        {
            Texture2D texture = assets->textures[ENEMY_FISH];
            if (texture.id > 0)
            {
                Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
                Rectangle dest = { enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
            }
            
            if (enemy->velocity.y < -5.0f && assets->fishWaterJump.id > 0)
            {
                Rectangle sSrc = { 0.0f, 0.0f, (float)assets->fishWaterJump.width, (float)assets->fishWaterJump.height };
                Rectangle sDst = { enemy->position.x + 20, enemy->position.y + 80, 76.0f, 76.0f };
                Vector2 sOrg = { 0.0f, 0.0f };
                DrawTexturePro(assets->fishWaterJump, sSrc, sDst, sOrg, 0.0f, WHITE);
            }
        }
        return;
    }
    
    if (enemy->type == ENEMY_SAFE_POSTE)
    {
        if (assets->textures[ENEMY_SAFE_POSTE].id > 0)
        {
            Rectangle source = { 170.0f, 0.0f, 36.0f, 260.0f };
            Rectangle dest = { enemy->position.x + 37.5f, enemy->position.y, 60.0f, 450.0f };
            Vector2 origin = { 0.0f, 0.0f };
            DrawTexturePro(assets->textures[ENEMY_SAFE_POSTE], source, dest, origin, 0.0f, WHITE);
        }
        return;
    }
    
    if (enemy->type == ENEMY_BIKE)
    {
        Texture2D texture = assets->textures[ENEMY_BIKE];
        if (enemy->state == 1) texture = assets->bikeSkin2;
        else if (enemy->state == 2) texture = assets->bikeSkinItau;
        
        if (texture.id > 0)
        {
            Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
            Rectangle dest = { enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y };
            Vector2 origin = { 0.0f, 0.0f };
            DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
        }
        return;
    }
    
    Texture2D texture = assets->textures[enemy->type];
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