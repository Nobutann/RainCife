#include "entities/shark.h"
#include "raymath.h"

static void FireProjectile(Shark *shark, Rectangle playerRect)
{
    for (int i = 0; i < MAX_WATER_BALLS; i++)
    {
        if (!shark->balls[i].active)
        {
            float ballSize = 60.0f;
            shark->balls[i].rect = (Rectangle){shark->rect.x + shark->rect.width / 2 - ballSize / 2, shark->rect.y + shark->rect.height / 2 - ballSize / 2, ballSize, ballSize};

            Vector2 playerCenter = {playerRect.x + playerRect.width / 2, playerRect.y + playerRect.height / 2};
            Vector2 shootPos = {shark->balls[i].rect.x, shark->balls[i].rect.y};
            Vector2 dir = Vector2Subtract(playerCenter, shootPos);
            shark->balls[i].direction = Vector2Normalize(dir);
            shark->balls[i].speed = 20.0f * 60.0f;

            shark->balls[i].active = true;
            break;
        }
    }
}

static void FireBubble(Shark *shark)
{
    for (int i = 0; i < MAX_WATER_BALLS; i++)
    {
        if (!shark->balls[i].active)
        {
            float bubbleSize = 80.0f;
            shark->balls[i].rect = (Rectangle){shark->rect.x + shark->rect.width / 2 - bubbleSize / 2, shark->rect.y + shark->rect.height / 2, bubbleSize, bubbleSize};
            shark->balls[i].direction = (Vector2){0, 1.0f};
            shark->balls[i].speed = 9.5f * 60.0f;
            shark->balls[i].active = true;
            break;
        }
    }
}

void InitShark(Shark *shark, int screenWidth, int screenHeight)
{
    float size = 300.0f;
    shark->startPos = (Vector2){(float)screenWidth - size - 50.0f, (float)screenHeight - size - 50.0f};
    shark->rect = (Rectangle){shark->startPos.x, shark->startPos.y, size, size};
    shark->velocity = (Vector2){0, 0};
    shark->state = SHARK_IDLE;
    shark->timer = 0.0f;
    shark->shootTimer = 0.0f;
    shark->shootCount = 0;
    shark->arcDrops = 0;
    shark->active = true;
    shark->health = 200;

    for (int i = 0; i < MAX_WATER_BALLS; i++)
    {
        shark->balls[i].active = false;
    }
}

void UpdateShark(Shark *shark, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight)
{
    if (!shark->active) return;

    for (int i = 0; i < MAX_WATER_BALLS; i++)
    {
        if (shark->balls[i].active)
        {
            shark->balls[i].rect.x += shark->balls[i].direction.x * shark->balls[i].speed * deltaTime;
            shark->balls[i].rect.y += shark->balls[i].direction.y * shark->balls[i].speed * deltaTime;

            if (shark->balls[i].rect.x < -200 || shark->balls[i].rect.x > screenWidth + 200 ||
                shark->balls[i].rect.y < -200 || shark->balls[i].rect.y > screenHeight + 200)
            {
                shark->balls[i].active = false;
            }
        }
    }

    float size = 300.0f;
    if (shark->state != SHARK_ARC_ATTACK && shark->state != SHARK_WAIT_RETURN)
    {
        shark->startPos.y = (float)screenHeight - size - 50.0f;
        shark->rect.y = shark->startPos.y;
    }
    shark->rect.width = size;
    shark->rect.height = size;

    switch (shark->state)
    {
        case SHARK_IDLE:
            shark->timer += deltaTime;
            if (shark->timer > 2.0f)
            {
                int chance = GetRandomValue(1, 100);
                if (chance <= 33)
                {
                    shark->state = SHARK_PREP_LEFT;
                }
                else if (chance <= 66)
                {
                    shark->state = SHARK_SHOOTING;
                    shark->shootCount = 0;
                    shark->targetShootCount = GetRandomValue(3, 4);
                    shark->shootTimer = 1.5f;
                }
                else
                {
                    shark->state = SHARK_ARC_ATTACK;
                    shark->velocity.x = -850.0f;
                    shark->velocity.y = -1472.0f;
                    shark->arcDrops = 0;
                }
                shark->timer = 0.0f;
            }
            break;

        case SHARK_PREP_LEFT:
            shark->rect.x -= 1500.0f * deltaTime;
            if (shark->rect.x < -shark->rect.width)
            {
                shark->state = SHARK_DASH_RIGHT;
                shark->timer = 0.0f;
            }
            break;

        case SHARK_DASH_RIGHT:
            shark->rect.x += 1500.0f * deltaTime;
            if (shark->rect.x > (float)screenWidth + 100.0f)
            {
                shark->state = SHARK_DASH_LEFT;
                shark->timer = 0.0f;
            }
            break;

        case SHARK_DASH_LEFT:
            shark->startPos.x = (float)screenWidth - size - 50.0f;
            float returnSpeed = 1200.0f;
            if (shark->rect.x > shark->startPos.x)
            {
                shark->rect.x -= returnSpeed * deltaTime;
                if (shark->rect.x < shark->startPos.x) shark->rect.x = shark->startPos.x;
            }
            else
            {
                shark->rect.x += returnSpeed * deltaTime;
                if (shark->rect.x > shark->startPos.x) shark->rect.x = shark->startPos.x;
            }

            if (shark->rect.x == shark->startPos.x)
            {
                shark->state = SHARK_IDLE;
                shark->timer = 0.0f;
            }
            break;

        case SHARK_SHOOTING:
            shark->shootTimer += deltaTime;

            if (shark->shootTimer >= 1.5f)
            {
                if (shark->shootCount < shark->targetShootCount)
                {
                    FireProjectile(shark, playerRect);
                    shark->shootCount++;
                    shark->shootTimer = 0.0f;
                }
                else
                {
                    shark->state = SHARK_IDLE;
                    shark->timer = 0.0f;
                }
            }
            break;

        case SHARK_ARC_ATTACK:
            shark->rect.x += shark->velocity.x * deltaTime;
            shark->rect.y += shark->velocity.y * deltaTime;
            shark->velocity.y += 1300.0f * deltaTime;
            shark->timer += deltaTime;

            if (shark->arcDrops == 0 && shark->timer >= 0.4f)
            {
                FireBubble(shark);
                shark->arcDrops++;
            }
            else if (shark->arcDrops == 1 && shark->timer >= 0.8f)
            {
                FireBubble(shark);
                shark->arcDrops++;
            }
            else if (shark->arcDrops == 2 && shark->timer >= 1.2f)
            {
                FireBubble(shark);
                shark->arcDrops++;
            }

            if (shark->rect.y > (float)screenHeight + 100.0f)
            {
                shark->rect.y = shark->startPos.y;
                shark->state = SHARK_DASH_RIGHT;
                shark->timer = 0.0f;
            }
            break;

        case SHARK_WAIT_RETURN:
            shark->timer += deltaTime;
            if (shark->timer > 2.0f)
            {
                shark->startPos.x = (float)screenWidth - size - 50.0f;
                shark->startPos.y = (float)screenHeight - size - 50.0f;
                shark->rect.x = shark->startPos.x;
                shark->rect.y = shark->startPos.y;
                shark->state = SHARK_IDLE;
                shark->timer = 0.0f;
            }
            break;
    }
}

void DrawShark(Shark *shark)
{
    if (!shark->active) return;

    for (int i = 0; i < MAX_WATER_BALLS; i++)
    {
        if (shark->balls[i].active)
        {
            DrawRectangleLinesEx(shark->balls[i].rect, 3.0f, BLUE);
            Color bColor = BLUE;
            bColor.a = 150;
            DrawRectangleRec(shark->balls[i].rect, bColor);
        }
    }

    Color color = SKYBLUE;
    const char* stateText = "IDLE";

    switch (shark->state)
    {
        case SHARK_PREP_LEFT: color = GOLD; stateText = "PREPARING DASH..."; break;
        case SHARK_DASH_RIGHT: color = RED; stateText = "DASH ATTACK!"; break;
        case SHARK_DASH_LEFT: color = BLUE; stateText = "RETURNING"; break;
        case SHARK_SHOOTING: color = DARKBLUE; stateText = "SHOOTING!"; break;
        case SHARK_ARC_ATTACK: color = PURPLE; stateText = "ARC ATTACK!"; break;
        case SHARK_WAIT_RETURN: return;
        default: break;
    }

    DrawRectangleLinesEx(shark->rect, 5.0f, color);
    Color fillColor = color;
    fillColor.a = 100;
    DrawRectangleRec(shark->rect, fillColor);

    DrawText("SHARK BOSS", (int)shark->rect.x + 15, (int)shark->rect.y + 50, 20, color);
    DrawText(stateText, (int)shark->rect.x + 15, (int)shark->rect.y + 80, 15, color);
}

void UnloadShark(Shark *shark)
{
}
