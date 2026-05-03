#include <raylib.h>
#include "enemy.h"

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
            enemy->position = (Vector2){screenWidth, screenHeight * 0.6f};
            enemy->size = (Vector2){50, 50};
            break;
        case ENEMY_BIRD2:
            enemy->position = (Vector2){screenWidth, screenHeight * 0.55f};
            enemy->size = (Vector2){50, 50};
            enemy->velocity.y = 7.0f;
            break;
        case ENEMY_BIKE:
            enemy->position = (Vector2){screenWidth, screenHeight * 0.75 - 50};
            enemy->size = (Vector2){70, 70};
            break;
        case ENEMY_WOOD:
            enemy->position = (Vector2){screenWidth, screenHeight * 0.75f - 50};
            enemy->size = (Vector2){140, 70};
            break;
        case ENEMY_CABOCLO:
            enemy->position = (Vector2){screenWidth, screenHeight * 0.75 - 80};
            enemy->size = (Vector2){60, 80};
            break;
        case ENEMY_FISH:
            enemy->position = (Vector2){screenWidth * 0.42, screenHeight * 0.80 - 80};
            enemy->size = (Vector2){60, 60};
            enemy->velocity.y = -18.0f;
            enemy->velocity.x = -(4.0f + baseSpeed);
            break;
    }
}

void UpdateEnemy(Enemy *enemy, int screenWidth, int screenHeight, int baseSpeed)
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

        case ENEMY_CABOCLO:
            if (enemy->state == 0)
            {
                enemy->position.x -= (4 + baseSpeed);
                if (enemy->position.x < screenWidth * 0.7f)
                {
                    enemy->state = 1;
                    enemy->stateTimer = GetTime();
                }
            }
            else if (enemy->state == 1)
            {
                if (GetTime() - enemy->stateTimer > 1.0)
                {
                    enemy->state = 2;
                } 
            }
            else if (enemy->state == 2)
            {
                enemy->position.x -= 20;
            }
            if (enemy->position.x < -100)
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

void DrawEnemy(Enemy *enemy, Texture2D texture)
{
    if (!enemy->active)
    {
        return;
    }

    if (enemy->type == ENEMY_CABOCLO && enemy->state == 1)
    {
        DrawText("!", enemy->position.x + 20, enemy->position.y - 40, 40, ORANGE);
    }
    
    if (texture.id > 0)
    {
        DrawTextureV(texture, enemy->position, WHITE);
    }
    else
    {
        DrawRectangleV(enemy->position, enemy->size, RED);
    }
}