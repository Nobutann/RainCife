#include <raylib.h>
#include "entities/enemy.h"

void InitEnemy(Enemy *enemy, EnemyType type, int screenWidth, int screenHeight, int baseSpeed)
{
    enemy->type = type;
    enemy->active = true;
    enemy->dying = false;
    enemy->state = 0;
    enemy->velocity.x = 0;
    enemy->velocity.y = 0;
    enemy->headDestroyed = false;
    enemy->animationTimer = 0.0f;
    enemy->currentFrame = 0;

    switch (type)
    {
        case ENEMY_BIRD1:
            enemy->position = (Vector2){(float)screenWidth, screenHeight * 0.4f};
            enemy->size = (Vector2){100, 100};
            enemy->hitboxOffset = (Vector2){18, 28};
            enemy->hitboxSize  = (Vector2){58, 42};
            break;
        case ENEMY_BIRD2:
            enemy->position = (Vector2){(float)screenWidth, screenHeight * 0.35f};
            enemy->size = (Vector2){100, 100};
            enemy->velocity.y = 7.0f;
            enemy->hitboxOffset = (Vector2){18, 28};
            enemy->hitboxSize  = (Vector2){58, 42};
            break;
        case ENEMY_BIKE:
            enemy->position = (Vector2){(float)screenWidth, screenHeight * 0.785f - 185};
            enemy->size = (Vector2){350, 250};
            enemy->state = GetRandomValue(0, 2);
            enemy->hitboxOffset = (Vector2){55, 65};
            enemy->hitboxSize  = (Vector2){230, 125};
            break;
        case ENEMY_WOOD:
            enemy->position = (Vector2){(float)screenWidth, screenHeight * 0.785f - 360};
            enemy->size = (Vector2){550, 450};
            enemy->hitboxOffset = (Vector2){150, 150};
            enemy->hitboxSize  = (Vector2){275, 225};
            break;
        case ENEMY_POSTE:
            enemy->basePosition = (Vector2){(float)screenWidth, screenHeight * 0.785f - 640};
            enemy->position = enemy->basePosition;
            enemy->size = (Vector2){200, 200};
            enemy->headDetached = false;
            enemy->headLanded = false;
            enemy->hitboxOffset = (Vector2){29, 24};
            enemy->hitboxSize  = (Vector2){142, 127};
            break;
        case ENEMY_FISH:
            enemy->basePosition = (Vector2){(float)screenWidth * 0.85f, screenHeight * 0.82f - 40};
            enemy->position = (Vector2){enemy->basePosition.x, (float)screenHeight + 200};
            enemy->size = (Vector2){120, 120};
            enemy->velocity.y = 0;
            enemy->velocity.x = -(8.0f + baseSpeed);
            enemy->state = 0;
            enemy->stateTimer = GetTime();
            enemy->hitboxOffset = (Vector2){15, 22};
            enemy->hitboxSize  = (Vector2){84, 68};
            break;
        case ENEMY_SAFE_POSTE:
            enemy->basePosition = (Vector2){(float)screenWidth, screenHeight * 0.785f - 640};
            enemy->position = enemy->basePosition;
            enemy->size = (Vector2){200, 600};
            enemy->hitboxOffset = (Vector2){56, 53};
            enemy->hitboxSize  = (Vector2){87, 520};
            break;
        case ENEMY_COUNT:
            break;
    }
}

Rectangle GetEnemyHitbox(Enemy *enemy)
{
    if (enemy->dying)
        return (Rectangle){0.0f, 0.0f, 0.0f, 0.0f};
    if (enemy->type == ENEMY_POSTE && enemy->headDestroyed)
        return (Rectangle){0.0f, 0.0f, 0.0f, 0.0f};

    return (Rectangle){
        enemy->position.x + enemy->hitboxOffset.x,
        enemy->position.y + enemy->hitboxOffset.y,
        enemy->hitboxSize.x,
        enemy->hitboxSize.y
    };
}

void UpdateEnemy(Enemy *enemy, int screenWidth, int screenHeight, int baseSpeed, Rectangle playerHitbox)
{
    (void)screenWidth;

    if (!enemy->active && !enemy->dying)
    {
        return;
    }

    if (enemy->dying)
    {
        if (enemy->type == ENEMY_WOOD || enemy->type == ENEMY_BIKE || enemy->type == ENEMY_POSTE)
        {
            if (enemy->type == ENEMY_POSTE)
                enemy->basePosition.x -= (15 + baseSpeed);

            enemy->animationTimer += 1.0f;
            if (enemy->animationTimer >= 6.0f)
            {
                enemy->animationTimer = 0.0f;
                if (enemy->currentFrame < 5)
                {
                    enemy->currentFrame++;
                }
                else
                {
                    enemy->dying = false;
                    if (enemy->type == ENEMY_POSTE)
                        enemy->headDestroyed = true;
                    else
                        enemy->active = false;
                }
            }
            return;
        }

        float landY = (enemy->type == ENEMY_BIRD1 || enemy->type == ENEMY_BIRD2)
                      ? screenHeight * 0.785f - 140.0f
                      : screenHeight * 0.785f - 120.0f;

        if (enemy->position.y < landY)
        {
            enemy->position.x += enemy->velocity.x;
            enemy->position.y += enemy->velocity.y;
            enemy->velocity.y += 0.4f;
            if (enemy->position.y >= landY)
            {
                enemy->position.y = landY;
                enemy->velocity.y = 0.0f;
                enemy->velocity.x = -(15 + baseSpeed);
            }
        }
        else
        {
            enemy->position.x += enemy->velocity.x;
        }

        if (enemy->position.x < -(enemy->size.x + 200.0f))
        {
            enemy->active = false;
            enemy->dying = false;
        }
        return;
    }

    switch(enemy->type)
    {
        case ENEMY_BIRD1:
            enemy->position.x -= (15 + baseSpeed);
            if (enemy->position.x < -enemy->size.x)
            {
                enemy->active = false;
            } 
            break;

        case ENEMY_BIRD2:
            enemy->position.x -= (12 + baseSpeed);
            enemy->position.y += enemy->velocity.y;
            if (enemy->position.y < screenHeight * 0.20f) 
            {
                enemy->velocity.y = 7.0f;
            }    
            else if (enemy->position.y > screenHeight * 0.70f) 
            {
                enemy->velocity.y = -7.0f;
            }
            if (enemy->position.x < -enemy->size.x)
            {
                enemy->active = false;
            }
            break;

        case ENEMY_BIKE:
            enemy->position.x -= (15 + baseSpeed);
        if (enemy->position.x < -enemy->size.x) 
        {
            enemy->active = false;
        }
            break;
        case ENEMY_WOOD:
            enemy->position.x -= (15 + baseSpeed);
            if (enemy->position.x < -enemy->size.x)
            {
                enemy->active = false;
            }
            break;

        case ENEMY_POSTE:
            enemy->basePosition.x -= (15 + baseSpeed);
            if (enemy->headDestroyed)
            {
                enemy->position = enemy->basePosition;
            }
            else if (!enemy->headDetached)
            {
                enemy->position = enemy->basePosition;
                if (enemy->position.x - playerHitbox.x < 350.0f && enemy->position.x > playerHitbox.x)
                {
                    enemy->headDetached = true;
                    enemy->velocity.x = -8.0f;
                    enemy->velocity.y = -2.0f;
                }
            }
            else if (!enemy->headLanded)
            {
                enemy->position.x += enemy->velocity.x;
                enemy->position.y += enemy->velocity.y;
                enemy->velocity.y += 0.4f;

                float landY = screenHeight * 0.785f - 180;
                if (enemy->position.y >= landY)
                {
                    enemy->position.y = landY;
                    enemy->velocity.y = 0;
                    enemy->velocity.x = -(15 + baseSpeed);
                    enemy->headLanded = true;
                }
            }
            else
            {
                enemy->position.x += enemy->velocity.x;
            }

            if (enemy->headLanded && !enemy->headDestroyed ? enemy->position.x < -enemy->size.x : enemy->basePosition.x < -enemy->size.x)
            {
                enemy->active = false;
            }
            break;

        case ENEMY_FISH:
            if (enemy->state == 0)
            {
                enemy->basePosition.x -= (15 + baseSpeed);
                enemy->position.x = enemy->basePosition.x;
                if (GetTime() - enemy->stateTimer > 1.1)
                {
                    enemy->state = 1;
                    enemy->position.y = enemy->basePosition.y;
                    enemy->velocity.y = -26.0f;
                    enemy->jumpOrigin = enemy->basePosition;
                }
            }
            else if (enemy->state == 1)
            {
                enemy->position.x += enemy->velocity.x;
                enemy->position.y += enemy->velocity.y;
                enemy->velocity.y += 0.45f;
            }
            
            if (enemy->position.x < -enemy->size.x)
            {
                enemy->active = false;
            }
            else if (enemy->state == 1 && enemy->position.y > screenHeight + enemy->size.y)
            {
                enemy->active = false;
            }
            break;

        case ENEMY_SAFE_POSTE:
            enemy->position.x -= (15 + baseSpeed);
            if (enemy->position.x < -enemy->size.x)
            {
                enemy->active = false;
            }
            break;
        case ENEMY_COUNT:
            break;
    }
}

void DrawEnemy(Enemy *enemy, EnemyAssets *assets)
{
    if (!enemy->active && !enemy->dying)
    {
        return;
    }

    if (enemy->dying)
    {
        if (enemy->type == ENEMY_POSTE && assets->posteSemCabeca.id > 0)
        {
            Rectangle src = { 0.0f, 0.0f, (float)assets->posteSemCabeca.width, (float)assets->posteSemCabeca.height };
            Rectangle dest = { enemy->basePosition.x, enemy->basePosition.y + 100.0f, 200.0f, 500.0f };
            DrawTexturePro(assets->posteSemCabeca, src, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        if (enemy->type == ENEMY_BIRD1 || enemy->type == ENEMY_BIRD2)
        {
            if (assets->birdDeath.id > 0)
            {
                Rectangle src = { 0.0f, 0.0f, (float)assets->birdDeath.width, (float)assets->birdDeath.height };
                Rectangle dest = { enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y };
                DrawTexturePro(assets->birdDeath, src, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }
        else if (enemy->type == ENEMY_FISH)
        {
            if (assets->fishDeath.id > 0)
            {
                Rectangle src = { 0.0f, 0.0f, (float)assets->fishDeath.width, (float)assets->fishDeath.height };
                Rectangle dest = { enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y };
                DrawTexturePro(assets->fishDeath, src, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }
        else
        {
            if (assets->destroyedSheet.id > 0)
            {
                float frameW = (float)assets->destroyedSheet.width / 6.0f;
                Rectangle src = { enemy->currentFrame * frameW, 0.0f, frameW, (float)assets->destroyedSheet.height };
                Rectangle dest = { enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y };
                DrawTexturePro(assets->destroyedSheet, src, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }
        return;
    }

    if (enemy->type == ENEMY_POSTE)
    {
        if (!enemy->headDetached && !enemy->headDestroyed)
        {
            if (assets->textures[ENEMY_POSTE].id > 0)
            {
                Rectangle source = { 0.0f, 0.0f, (float)assets->textures[ENEMY_POSTE].width, (float)assets->textures[ENEMY_POSTE].height };
                Rectangle dest = { enemy->basePosition.x, enemy->basePosition.y, 200.0f, 600.0f };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(assets->textures[ENEMY_POSTE], source, dest, origin, 0.0f, WHITE);
            }
        }
        else
        {
            if (assets->posteSemCabeca.id > 0)
            {
                Rectangle source = { 0.0f, 0.0f, (float)assets->posteSemCabeca.width, (float)assets->posteSemCabeca.height };
                Rectangle dest = { enemy->basePosition.x, enemy->basePosition.y + 100.0f, 200.0f, 500.0f };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(assets->posteSemCabeca, source, dest, origin, 0.0f, WHITE);
            }

            if (!enemy->headDestroyed && assets->posteCabecas.id > 0)
            {
                float halfW = (float)assets->posteCabecas.width / 2.0f;
                float srcX = enemy->headLanded ? halfW : 0.0f;
                Rectangle source = { srcX, 0.0f, halfW, (float)assets->posteCabecas.height };
                Rectangle dest = { enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(assets->posteCabecas, source, dest, origin, 0.0f, WHITE);
            }
        }
        DrawRectangleLinesEx(GetEnemyHitbox(enemy), 2.0f, RED);
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
            
        }
        DrawRectangleLinesEx(GetEnemyHitbox(enemy), 2.0f, RED);
        return;
    }
    
    if (enemy->type == ENEMY_SAFE_POSTE)
    {
        if (assets->textures[ENEMY_SAFE_POSTE].id > 0)
        {
            Rectangle source = { 170.0f, 0.0f, 36.0f, 260.0f };
            Rectangle dest = { enemy->position.x + 50.0f, enemy->position.y, 80.0f, 600.0f };
            Vector2 origin = { 0.0f, 0.0f };
            DrawTexturePro(assets->textures[ENEMY_SAFE_POSTE], source, dest, origin, 0.0f, WHITE);
        }
        DrawRectangleLinesEx(GetEnemyHitbox(enemy), 2.0f, DARKGRAY);
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
        DrawRectangleLinesEx(GetEnemyHitbox(enemy), 2.0f, RED);
        return;
    }
    
    if (enemy->type == ENEMY_BIRD1 || enemy->type == ENEMY_BIRD2)
    {
        UpdateAnimation(&assets->birdAnimation, GetFrameTime());
        float scale = enemy->size.x / (float)assets->birdAnimation.frameWidth;
        DrawAnimationFrame(&assets->birdAnimation, enemy->position, scale, false, WHITE);
        DrawRectangleLinesEx(GetEnemyHitbox(enemy), 2.0f, RED);
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
    DrawRectangleLinesEx(GetEnemyHitbox(enemy), 2.0f, RED);
}
