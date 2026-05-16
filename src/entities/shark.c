#include "entities/shark.h"
#include "entities/player.h"
#include "raymath.h"

static void FireProjectile(Shark *shark, Rectangle playerRect) {
    for (int i = 0; i < MAX_WATER_BALLS; i++) {
        if (!shark->balls[i].active) {
            float visualSize = 120.0f;
            float hitboxSize = 60.0f;
            float spawnX = shark->rect.x + shark->rect.width - 360;
            float spawnY = shark->rect.y - 150;
            shark->balls[i].rect = (Rectangle){ spawnX, spawnY, visualSize, visualSize };
            shark->balls[i].hitbox = (Rectangle){ spawnX + (visualSize - hitboxSize) / 2, spawnY + (visualSize - hitboxSize) / 2, hitboxSize, hitboxSize };
            
            Vector2 playerCenter = { playerRect.x + playerRect.width / 2, playerRect.y + playerRect.height / 2 };
            Vector2 shootPos = { shark->balls[i].rect.x, shark->balls[i].rect.y };
            Vector2 dir = Vector2Subtract(playerCenter, shootPos);
            shark->balls[i].direction = Vector2Normalize(dir);
            shark->balls[i].speed = 20.0f * 60.0f; 
            
            shark->balls[i].active = true;
            break;
        }
    }
}

static void FireBubble(Shark *shark) {
    for (int i = 0; i < MAX_WATER_BALLS; i++) {
        if (!shark->balls[i].active) {
            float bubbleSize = 120.0f;
            shark->balls[i].rect = (Rectangle){ shark->rect.x + shark->rect.width / 2 - bubbleSize / 2, shark->rect.y + shark->rect.height / 2, bubbleSize, bubbleSize };
            shark->balls[i].hitbox = shark->balls[i].rect;
            shark->balls[i].direction = (Vector2){ 0, 1.0f }; 
            shark->balls[i].speed = 12.5f * 60.0f; 
            shark->balls[i].active = true;
            break;
        }
    }
}

void InitShark(Shark *shark, int screenWidth, int screenHeight) {
    float size = 300.0f;
    shark->startPos = (Vector2){ (float)screenWidth - size - 50.0f, (float)screenHeight - size - 50.0f };
    shark->rect = (Rectangle){ shark->startPos.x, shark->startPos.y, size, size };
    shark->velocity = (Vector2){ 0, 0 };
    shark->state = SHARK_IDLE;
    shark->timer = 0.0f;
    shark->shootTimer = 0.0f;
    shark->shootCount = 0;
    shark->arcDrops = 0;
    shark->active = true;
    shark->health = 100;

    for (int i = 0; i < MAX_WATER_BALLS; i++) {
        shark->balls[i].active = false;
    }

    shark->texShoot = LoadTexture("assets/sprites/Boss/Shark_attack_bubble.png");
    shark->texDash = LoadTexture("assets/sprites/Boss/Shark_dash.png");
    shark->texDashRight = LoadTexture("assets/sprites/Boss/Shark_vermelho.png");
    shark->texJump = LoadTexture("assets/sprites/Boss/tubarao_flying-Sheet.png");
    shark->texBubble = LoadTexture("assets/sprites/Boss/bubble.png");
    shark->animTimer = 0.0f;
    shark->animFrame = 0;
}

void UpdateShark(Shark *shark, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight) {
    if (!shark->active) return;

    for (int i = 0; i < MAX_WATER_BALLS; i++) {
        if (shark->balls[i].active) {
            float dx = shark->balls[i].direction.x * shark->balls[i].speed * deltaTime;
            float dy = shark->balls[i].direction.y * shark->balls[i].speed * deltaTime;
            shark->balls[i].rect.x += dx;
            shark->balls[i].rect.y += dy;
            shark->balls[i].hitbox.x += dx;
            shark->balls[i].hitbox.y += dy;

            if (shark->balls[i].rect.x < -200 || shark->balls[i].rect.x > screenWidth + 200 ||
                shark->balls[i].rect.y < -200 || shark->balls[i].rect.y > screenHeight + 200) {
                shark->balls[i].active = false;
            }
        }
    }

    if (shark->state == SHARK_SHOOTING) {
        shark->animTimer += deltaTime;
        if (shark->animTimer >= 0.1f) {
            shark->animTimer = 0.0f;
            shark->animFrame = (shark->animFrame + 1) % 3;
        }
    } else if (shark->state == SHARK_ARC_ATTACK) {
        shark->animTimer += deltaTime;
        if (shark->animTimer >= 0.08f) {
            shark->animTimer = 0.0f;
            shark->animFrame = (shark->animFrame + 1) % 12;
        }
    }

    float size = 300.0f;
    if (shark->state != SHARK_ARC_ATTACK && shark->state != SHARK_WAIT_RETURN) {
        shark->startPos.y = (float)screenHeight - size - 50.0f;
        shark->rect.y = shark->startPos.y;
    }
    shark->rect.width = size;
    shark->rect.height = size;

    switch (shark->state) {
        case SHARK_IDLE:
            shark->timer += deltaTime;
            if (shark->timer > 2.0f) {
                int chance = GetRandomValue(1, 100);
                if (chance <= 33) {
                    shark->state = SHARK_PREP_LEFT;
                } else if (chance <= 66) {
                    shark->state = SHARK_SHOOTING;
                    shark->shootCount = 0;
                    shark->targetShootCount = GetRandomValue(3, 4);
                    shark->shootTimer = 1.5f;
                    shark->animFrame = 0;
                    shark->animTimer = 0.0f;
                } else {
                    shark->state = SHARK_ARC_ATTACK;
                    shark->velocity.x = -850.0f;
                    shark->velocity.y = -1472.0f;
                    shark->arcDrops = 0;
                    shark->animFrame = 0;
                    shark->animTimer = 0.0f;
                }
                shark->timer = 0.0f;
            }
            break;

        case SHARK_PREP_LEFT:
            shark->rect.x -= 1500.0f * deltaTime;
            if (shark->rect.x < -shark->rect.width) {
                shark->rect.x = -1050.0f;
                shark->state = SHARK_DASH_WAIT;
                shark->timer = 0.0f;
            }
            break;

        case SHARK_DASH_WAIT:
            shark->timer += deltaTime;
            if (shark->timer >= 1.0f) {
                shark->state = SHARK_DASH_RIGHT;
                shark->timer = 0.0f;
            }
            break;

        case SHARK_DASH_RIGHT:
            shark->rect.x += 2000.0f * deltaTime; 
            if (shark->rect.x > (float)screenWidth + 100.0f) {
                shark->state = SHARK_DASH_LEFT;
                shark->timer = 0.0f;
            }
            break;

        case SHARK_DASH_LEFT:
            shark->startPos.x = (float)screenWidth - size - 50.0f;
            float returnSpeed = 1600.0f; 
            if (shark->rect.x > shark->startPos.x) {
                shark->rect.x -= returnSpeed * deltaTime;
                if (shark->rect.x < shark->startPos.x) shark->rect.x = shark->startPos.x;
            } else {
                shark->rect.x += returnSpeed * deltaTime;
                if (shark->rect.x > shark->startPos.x) shark->rect.x = shark->startPos.x;
            }

            if (shark->rect.x == shark->startPos.x) {
                shark->state = SHARK_IDLE;
                shark->timer = 0.0f;
            }
            break;

        case SHARK_SHOOTING:
            shark->shootTimer += deltaTime;

            if (shark->shootTimer >= 1.1f) {
                if (shark->shootCount < shark->targetShootCount) {
                    FireProjectile(shark, playerRect);
                    shark->shootCount++;
                    shark->shootTimer = 0.0f;
                } else {
                    shark->state = SHARK_IDLE;
                    shark->timer = 0.0f;
                }
            }
            break;

        case SHARK_ARC_ATTACK:
            shark->rect.x += shark->velocity.x * deltaTime;
            shark->rect.y += shark->velocity.y * deltaTime;
            shark->velocity.y += 1200.0f * deltaTime; 
            shark->timer += deltaTime;

            if (shark->arcDrops == 0 && shark->timer >= 0.4f) {
                FireBubble(shark);
                shark->arcDrops++;
            } else if (shark->arcDrops == 1 && shark->timer >= 0.8f) {
                FireBubble(shark);
                shark->arcDrops++;
            } else if (shark->arcDrops == 2 && shark->timer >= 1.35f) {
                FireBubble(shark);
                shark->arcDrops++;
            }

            if (shark->rect.y > (float)screenHeight + 100.0f) {
                shark->rect.y = shark->startPos.y;
                shark->state = SHARK_DASH_RIGHT;
                shark->timer = 0.0f;
            }
            break;

        case SHARK_WAIT_RETURN:
            shark->timer += deltaTime;
            if (shark->timer > 2.0f) {
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

static void DrawSharkTexture(Texture2D tex, Rectangle dest, bool flipX) {
    if (tex.id == 0) return;
    float texWidth = (float)tex.width;
    float texHeight = (float)tex.height;
    Rectangle source = { flipX ? texWidth : 0.0f, 0.0f, flipX ? -texWidth : texWidth, texHeight };
    DrawTexturePro(tex, source, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
}

static void DrawSharkFrame(Texture2D sheet, int frame, int totalFrames, Rectangle dest, bool flipX) {
    if (sheet.id == 0) return;
    float frameWidth = (float)sheet.width / (float)totalFrames;
    float frameHeight = (float)sheet.height;
    float frameOffsetX = frame * frameWidth;
    Rectangle source = { flipX ? frameOffsetX + frameWidth : frameOffsetX, 0.0f, flipX ? -frameWidth : frameWidth, frameHeight };
    DrawTexturePro(sheet, source, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
}

void DrawShark(Shark *shark) {
    if (!shark->active) return;

    for (int i = 0; i < MAX_WATER_BALLS; i++) {
        if (!shark->balls[i].active) continue;
        Rectangle ballRect = shark->balls[i].rect;
        Rectangle bubbleSource = { 0, 0, (float)shark->texBubble.width, (float)shark->texBubble.height };
        DrawTexturePro(shark->texBubble, bubbleSource, ballRect, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawRectangleLinesEx(shark->balls[i].hitbox, 2.0f, RED);
    }

    if (shark->state == SHARK_WAIT_RETURN) return;

    DrawRectangleLinesEx(GetSharkHitbox(shark), 2.0f, RED);

    float shootSpriteWidth = 800.0f, shootSpriteHeight = 700.0f;
    Rectangle destShoot = { shark->rect.x - 300.0f, shark->rect.y - 400.0f, shootSpriteWidth, shootSpriteHeight };

    float dashSpriteWidth = 1150.0f, dashSpriteHeight = 650.0f;
    Rectangle destDash = { shark->rect.x - 225.0f, shark->rect.y - 230.0f, dashSpriteWidth, dashSpriteHeight };

    float jumpSpriteWidth = 1550.0f, jumpSpriteHeight = 1050.0f;
    Rectangle destJump = { shark->rect.x - 400.0f, shark->rect.y - 200.0f, jumpSpriteWidth, jumpSpriteHeight };

    switch (shark->state) {
        case SHARK_IDLE:
            DrawSharkFrame(shark->texShoot, 0, 3, destShoot, false);
            break;

        case SHARK_PREP_LEFT:
            DrawSharkTexture(shark->texDash, destDash, true);
            break;

        case SHARK_DASH_RIGHT:
            DrawSharkTexture(shark->texDashRight, destDash, false);
            break;

        case SHARK_DASH_LEFT:
            DrawSharkTexture(shark->texDash, destDash, true);
            break;

        case SHARK_SHOOTING:
            DrawSharkFrame(shark->texShoot, shark->animFrame, 3, destShoot, false);
            break;

        case SHARK_ARC_ATTACK:
            DrawSharkFrame(shark->texJump, shark->animFrame, 12, destJump, false);
            break;

        default: break;
    }
}

void UnloadShark(Shark *shark) {
    UnloadTexture(shark->texShoot);
    UnloadTexture(shark->texDash);
    UnloadTexture(shark->texDashRight);
    UnloadTexture(shark->texJump);
    UnloadTexture(shark->texBubble);
}

void DamageShark(Shark *shark, int damage) {
    if (damage <= 0) return;
    shark->health -= damage;
    if (shark->health <= 0) {
        shark->health = 0;
        shark->active = false;
    }
}

bool TryDamageSharkFromPlayerAttack(Shark *shark, Player *player, float playerScale) {
    if (!shark->active) return false;
    if (!IsPlayerAttackHitboxActive(player) || player->weapon.hitConnected) return false;

    Rectangle attackHitbox = GetPlayerAttackHitbox(player, playerScale);
    if (!CheckCollisionRecs(attackHitbox, GetSharkHitbox(shark))) return false;

    DamageShark(shark, (int)player->weapon.damage);
    player->weapon.hitConnected = true;
    return true;
}

Rectangle GetSharkHitbox(Shark *shark) {
    Vector2 hitboxOffset, hitboxSize;

    switch (shark->state) {
        case SHARK_PREP_LEFT:
        case SHARK_DASH_RIGHT:
        case SHARK_DASH_LEFT:
            hitboxOffset = (Vector2){ -60.0f, -60.0f };
            hitboxSize = (Vector2){ 650.0f, 540.0f };
            break;
        case SHARK_ARC_ATTACK:
            hitboxOffset = (Vector2){ 40.0f, -140.0f };
            hitboxSize = (Vector2){ 630.0f, 380.0f };
            break;
        default:
            hitboxOffset = (Vector2){ -120.0f, -160.0f };
            hitboxSize = (Vector2){ 460.0f, 460.0f };
            break;
    }

    return (Rectangle){
        shark->rect.x + hitboxOffset.x,
        shark->rect.y + hitboxOffset.y,
        hitboxSize.x,
        hitboxSize.y
    };
}
