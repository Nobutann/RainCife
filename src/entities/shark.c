#include "entities/shark.h"
#include "entities/player.h"
#include "raymath.h"
#include "core/sounds.h"

#define SHARK_DEATH_DURATION 2.3f
#define SHARK_DEATH_SINK_DISTANCE 100.0f
#define SHARK_SHOOT_FIRE_FRAME 7
#define SHARK_DASH_ANIM_TIME 0.12f
#define SHARK_HIT_FLASH_DURATION 0.12f

static float GetSharkIdleDelay(GameplayDifficulty difficulty) {
    switch (difficulty) {
        case GAMEPLAY_DIFFICULTY_HELENA: return 3.0f;
        case GAMEPLAY_DIFFICULTY_EASY: return 2.5f;
        case GAMEPLAY_DIFFICULTY_HARD: return 1.75f;
        case GAMEPLAY_DIFFICULTY_MEDIUM:
        default: return 2.0f;
    }
}

static float GetSharkDashSpeedMultiplier(GameplayDifficulty difficulty) {
    switch (difficulty) {
        case GAMEPLAY_DIFFICULTY_HELENA: return 0.75f;
        case GAMEPLAY_DIFFICULTY_EASY: return 0.90f;
        case GAMEPLAY_DIFFICULTY_HARD: return 1.08f;
        case GAMEPLAY_DIFFICULTY_MEDIUM:
        default: return 1.0f;
    }
}

static float GetSharkProjectileSpeedMultiplier(GameplayDifficulty difficulty) {
    switch (difficulty) {
        case GAMEPLAY_DIFFICULTY_HELENA: return 0.75f;
        case GAMEPLAY_DIFFICULTY_EASY: return 0.90f;
        case GAMEPLAY_DIFFICULTY_HARD: return 1.08f;
        case GAMEPLAY_DIFFICULTY_MEDIUM:
        default: return 1.0f;
    }
}

static int GetSharkTargetShootCount(GameplayDifficulty difficulty) {
    switch (difficulty) {
        case GAMEPLAY_DIFFICULTY_HELENA: return 2;
        case GAMEPLAY_DIFFICULTY_EASY: return GetRandomValue(2, 3);
        case GAMEPLAY_DIFFICULTY_HARD: return 4;
        case GAMEPLAY_DIFFICULTY_MEDIUM:
        default: return GetRandomValue(3, 4);
    }
}

static void FireProjectile(Shark *shark, Rectangle playerRect, GameplayDifficulty difficulty) {
    for (int i = 0; i < MAX_WATER_BALLS; i++) {
        if (!shark->balls[i].active) {
            float visualSize = 120.0f;
            float hitboxSize = 60.0f;
            float spawnX = shark->rect.x + shark->rect.width - 360;
            float spawnY = shark->rect.y - 150;
            shark->balls[i].rect = (Rectangle){ spawnX, spawnY, visualSize, visualSize };
            shark->balls[i].hitbox = (Rectangle){ spawnX + (visualSize - hitboxSize) / 2, spawnY + (visualSize - hitboxSize) / 2, hitboxSize, hitboxSize };
            
            Vector2 playerBody = { playerRect.x + playerRect.width / 2, playerRect.y + playerRect.height * 0.35f };
            Vector2 shootPos = { shark->balls[i].rect.x + visualSize / 2, shark->balls[i].rect.y + visualSize / 2 };
            Vector2 dir = Vector2Subtract(playerBody, shootPos);
            shark->balls[i].direction = Vector2Normalize(dir);
            shark->balls[i].speed = 24.0f * 60.0f * GetSharkProjectileSpeedMultiplier(difficulty);

            shark->balls[i].isWaterBubble = false;
            shark->balls[i].active = true;
            PlaySharkShootSound();
            break;
        }
    }
}

static void FireBubble(Shark *shark, GameplayDifficulty difficulty) {
    for (int i = 0; i < MAX_WATER_BALLS; i++) {
        if (!shark->balls[i].active) {
            float bubbleSize = 120.0f;
            float hitboxSize = 80.0f;
            shark->balls[i].rect = (Rectangle){ shark->rect.x + shark->rect.width / 2 - bubbleSize / 2, shark->rect.y + shark->rect.height / 2, bubbleSize, bubbleSize };
            shark->balls[i].hitbox = (Rectangle){
                shark->balls[i].rect.x + (bubbleSize - hitboxSize) / 2,
                shark->balls[i].rect.y + (bubbleSize - hitboxSize) / 2,
                hitboxSize,
                hitboxSize
            };
            shark->balls[i].direction = (Vector2){ 0, 1.0f }; 
            shark->balls[i].speed = 12.5f * 60.0f * GetSharkProjectileSpeedMultiplier(difficulty);
            shark->balls[i].isWaterBubble = true;
            shark->balls[i].active = true;
            PlaySharkBubbleSound();
            break;
        }
    }
}

void ResetShark(Shark *shark, int screenWidth, int screenHeight) {
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
    shark->dying = false;
    shark->health = 100;
    shark->hitFlashTimer = 0.0f;
    shark->deathTimer = 0.0f;
    shark->deathStartY = shark->rect.y;

    for (int i = 0; i < MAX_WATER_BALLS; i++) {
        shark->balls[i].active = false;
    }

    shark->animTimer = 0.0f;
    shark->animFrame = 0;
    shark->dashStartX = 0.0f;
    shark->dashSoundCount = 0;
}

void InitShark(Shark *shark, int screenWidth, int screenHeight) {
    ResetShark(shark, screenWidth, screenHeight);
    shark->texShoot = LoadTexture("assets/sprites/Boss/tubarao_shooting-Sheet.png");
    shark->texIdle = LoadTexture("assets/sprites/Boss/tubarao_idle-Sheet.png");
    shark->texDashLeft = LoadTexture("assets/sprites/Boss/Shark_dash-Sheet.png");
    shark->texDashRight = LoadTexture("assets/sprites/Boss/Shark_angry-Sheet.png");
    shark->texJump = LoadTexture("assets/sprites/Boss/tubarao_flying-Sheet.png");
    shark->texBubble = LoadTexture("assets/sprites/Boss/bubble.png");
    shark->texWaterBubble = LoadTexture("assets/sprites/Boss/water_bubble.png");
    shark->texDeath = LoadTexture("assets/sprites/Boss/Shark_death.png");
}

void UpdateShark(Shark *shark, Rectangle playerRect, float deltaTime, int screenWidth, int screenHeight, GameplayDifficulty difficulty) {
    if (shark->hitFlashTimer > 0.0f) {
        shark->hitFlashTimer -= deltaTime;
        if (shark->hitFlashTimer < 0.0f) {
            shark->hitFlashTimer = 0.0f;
        }
    }

    if (shark->dying) {
        shark->deathTimer += deltaTime;
        float progress = shark->deathTimer / SHARK_DEATH_DURATION;
        if (progress > 1.0f) progress = 1.0f;

        shark->rect.y = shark->deathStartY + SHARK_DEATH_SINK_DISTANCE * progress;
        if (shark->deathTimer >= SHARK_DEATH_DURATION) {
            shark->dying = false;
        }
        StopSharkSwimmingSound();
        return;
    }

    if (!shark->active) {
        StopSharkSwimmingSound();
        return;
    }

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

    if (shark->state == SHARK_IDLE) {
        shark->animTimer += deltaTime;
        if (shark->animTimer >= 0.1f) {
            shark->animTimer = 0.0f;
            shark->animFrame = (shark->animFrame + 1) % 9;
        }
    } else if (shark->state == SHARK_SHOOTING) {
        shark->animTimer += deltaTime;
        if (shark->animTimer >= 0.1f) {
            shark->animTimer = 0.0f;
            int nextFrame = (shark->animFrame + 1) % 9;

            if (nextFrame == 0 && shark->shootCount >= shark->targetShootCount) {
                shark->state = SHARK_IDLE;
                shark->timer = 0.0f;
                shark->animFrame = 0;
                shark->animTimer = 0.0f;
            } else {
                shark->animFrame = nextFrame;
                if (shark->animFrame == SHARK_SHOOT_FIRE_FRAME) {
                    if (shark->shootCount < shark->targetShootCount) {
                        FireProjectile(shark, playerRect, difficulty);
                        shark->shootCount++;
                    }
                }
            }
        }
    } else if (shark->state == SHARK_ARC_ATTACK) {
        shark->animTimer += deltaTime;
        if (shark->animTimer >= 0.08f) {
            shark->animTimer = 0.0f;
            shark->animFrame = (shark->animFrame + 1) % 12;
        }
    } else if (shark->state == SHARK_DASH_RIGHT) {
        shark->animTimer += deltaTime;
        if (shark->animTimer >= SHARK_DASH_ANIM_TIME) {
            shark->animTimer = 0.0f;
            shark->animFrame = (shark->animFrame + 1) % 6;
        }
    } else if (shark->state == SHARK_DASH_LEFT || shark->state == SHARK_PREP_LEFT) {
        shark->animTimer += deltaTime;
        if (shark->animTimer >= SHARK_DASH_ANIM_TIME) {
            shark->animTimer = 0.0f;
            shark->animFrame = (shark->animFrame + 1) % 6;
        }
    }

    float size = 300.0f;
    if (shark->state != SHARK_ARC_ATTACK && shark->state != SHARK_WAIT_RETURN) {
        shark->startPos.y = (float)screenHeight - size - 50.0f;

        float yOffset = 0.0f;
        if (shark->state == SHARK_PREP_LEFT || shark->state == SHARK_DASH_LEFT) {
            float progress = (float)(shark->animFrame % 6) + (shark->animTimer / SHARK_DASH_ANIM_TIME);
            float angle = (progress / 6.0f) * 2.0f * 3.14159265f;
            yOffset = 50.0f + 150.0f * sinf(angle);

            float diff = shark->rect.x - shark->startPos.x;
            float distance = diff < 0.0f ? -diff : diff;
            float damp = distance / 400.0f;
            if (damp > 1.0f) damp = 1.0f;
            yOffset *= damp;
        } else if (shark->state == SHARK_DASH_RIGHT) {
            float progress = (float)(shark->animFrame % 6) + (shark->animTimer / SHARK_DASH_ANIM_TIME);
            float angle = (progress / 6.0f) * 2.0f * 3.14159265f;
            yOffset = 50.0f + 150.0f * sinf(angle);
        }
        
        shark->rect.y = shark->startPos.y + yOffset;
    }
    shark->rect.width = size;
    shark->rect.height = size;

    switch (shark->state) {
        case SHARK_IDLE:
            shark->timer += deltaTime;
            if (shark->timer > GetSharkIdleDelay(difficulty)) {
                int chance = GetRandomValue(1, 100);
                if (chance <= 33) {
                    shark->state = SHARK_PREP_LEFT;
                    shark->animFrame = 0;
                    shark->animTimer = 0.0f;
                    shark->dashStartX = shark->rect.x;
                    shark->dashSoundCount = 0;
                } else if (chance <= 66) {
                    shark->state = SHARK_SHOOTING;
                    shark->shootCount = 0;
                    shark->targetShootCount = GetSharkTargetShootCount(difficulty);
                    shark->animFrame = 0;
                    shark->animTimer = 0.0f;
                } else {
                    shark->state = SHARK_ARC_ATTACK;
                    shark->velocity.x = -850.0f * GetSharkDashSpeedMultiplier(difficulty);
                    shark->velocity.y = -1472.0f * GetSharkDashSpeedMultiplier(difficulty);
                    shark->arcDrops = 0;
                    shark->animFrame = 0;
                    shark->animTimer = 0.0f;
                    PlaySharkJumpSound();
                }
                shark->timer = 0.0f;
            }
            break;

        case SHARK_PREP_LEFT: {
            shark->rect.x -= 1500.0f * GetSharkDashSpeedMultiplier(difficulty) * deltaTime;
            float endX = -1200.0f;
            float dist = shark->dashStartX - endX;
            if (dist > 0.0f) {
                float t = (shark->dashStartX - shark->rect.x) / dist;
                if (shark->dashSoundCount == 0 && t >= 0.0f) { PlaySharkSwimmingSound(); shark->dashSoundCount++; }
                else if (shark->dashSoundCount == 1 && t >= 0.33f) { PlaySharkSwimmingSound(); shark->dashSoundCount++; }
                else if (shark->dashSoundCount == 2 && t >= 0.66f) { PlaySharkSwimmingSound(); shark->dashSoundCount++; }
            }
            if (shark->rect.x < -1200.0f) {
                shark->rect.x = -1200.0f;
                shark->state = SHARK_DASH_WAIT;
                shark->timer = 0.0f;
            }
            break;
        }

        case SHARK_DASH_WAIT:
            shark->timer += deltaTime;
            if (shark->timer >= 0.5f) {
                shark->state = SHARK_DASH_RIGHT;
                shark->timer = 0.0f;
                shark->animFrame = 0;
                shark->animTimer = 0.0f;
                shark->dashStartX = shark->rect.x;
                shark->dashSoundCount = 0;
            }
            break;

        case SHARK_DASH_RIGHT: {
            shark->rect.x += 2000.0f * GetSharkDashSpeedMultiplier(difficulty) * deltaTime;
            float endX = (float)screenWidth + 100.0f;
            float dist = endX - shark->dashStartX;
            if (dist > 0.0f) {
                float t = (shark->rect.x - shark->dashStartX) / dist;
                if (shark->dashSoundCount == 0 && t >= 0.0f) { PlaySharkSwimmingSound(); shark->dashSoundCount++; }
                else if (shark->dashSoundCount == 1 && t >= 0.33f) { PlaySharkSwimmingSound(); shark->dashSoundCount++; }
                else if (shark->dashSoundCount == 2 && t >= 0.66f) { PlaySharkSwimmingSound(); shark->dashSoundCount++; }
            }
            if (shark->rect.x > (float)screenWidth + 100.0f) {
                shark->state = SHARK_DASH_LEFT;
                shark->timer = 0.0f;
                shark->animFrame = 0;
                shark->animTimer = 0.0f;
            }
            break;
        }

        case SHARK_DASH_LEFT:
            shark->startPos.x = (float)screenWidth - size - 50.0f;
            float returnSpeed = 1600.0f * GetSharkDashSpeedMultiplier(difficulty);
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
                shark->animFrame = 0;
                shark->animTimer = 0.0f;
            }
            break;

        case SHARK_SHOOTING:
            break;

        case SHARK_ARC_ATTACK:
            shark->rect.x += shark->velocity.x * deltaTime;
            shark->rect.y += shark->velocity.y * deltaTime;
            shark->velocity.y += 1200.0f * deltaTime; 
            shark->timer += deltaTime;

            if (shark->arcDrops == 0 && shark->timer >= 0.2f) {
                FireBubble(shark, difficulty);
                shark->arcDrops++;
            } else if (shark->arcDrops == 1 && shark->timer >= 0.5f) {
                FireBubble(shark, difficulty);
                shark->arcDrops++;
            } else if (shark->arcDrops == 2 && shark->timer >= 0.8f) {
                FireBubble(shark, difficulty);
                shark->arcDrops++;
            } else if (difficulty != GAMEPLAY_DIFFICULTY_HELENA && shark->arcDrops == 3 && shark->timer >= 1.2f) {
                FireBubble(shark, difficulty);
                shark->arcDrops++;
            }

            if (shark->rect.y > (float)screenHeight + 100.0f) {
                shark->rect.y = shark->startPos.y;
                shark->state = SHARK_DASH_RIGHT;
                shark->timer = 0.0f;
                shark->dashStartX = shark->rect.x;
                shark->dashSoundCount = 0;
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
                shark->animFrame = 0;
                shark->animTimer = 0.0f;
            }
            break;
    }
}

static void DrawSharkTexture(Texture2D tex, Rectangle dest, bool flipX, Color tint) {
    if (tex.id == 0) return;
    float texWidth = (float)tex.width;
    float texHeight = (float)tex.height;
    Rectangle source = { flipX ? texWidth : 0.0f, 0.0f, flipX ? -texWidth : texWidth, texHeight };
    DrawTexturePro(tex, source, dest, (Vector2){ 0, 0 }, 0.0f, tint);
}

static void DrawSharkFrame(Texture2D sheet, int frame, int totalFrames, Rectangle dest, bool flipX, Color tint) {
    if (sheet.id == 0) return;
    float frameWidth = (float)sheet.width / (float)totalFrames;
    float frameHeight = (float)sheet.height;
    float frameOffsetX = frame * frameWidth;
    Rectangle source = { flipX ? frameOffsetX + frameWidth : frameOffsetX, 0.0f, flipX ? -frameWidth : frameWidth, frameHeight };
    DrawTexturePro(sheet, source, dest, (Vector2){ 0, 0 }, 0.0f, tint);
}

void DrawShark(Shark *shark) {
    if (!shark->active && !shark->dying) return;

    if (shark->dying) {
        Rectangle destDeath = { shark->rect.x - 192.0f, shark->rect.y - 256.0f, 800.0f, 714.7f };
        DrawSharkTexture(shark->texDeath, destDeath, false, shark->hitFlashTimer > 0.0f ? RED : WHITE);
        return;
    }

    for (int i = 0; i < MAX_WATER_BALLS; i++) {
        if (!shark->balls[i].active) continue;
        Rectangle ballRect = shark->balls[i].rect;
        Texture2D tex = shark->balls[i].isWaterBubble ? shark->texWaterBubble : shark->texBubble;
        Rectangle bubbleSource = { 0, 0, (float)tex.width, (float)tex.height };
        DrawTexturePro(tex, bubbleSource, ballRect, (Vector2){ 0, 0 }, 0.0f, WHITE);
    }

    if (shark->state == SHARK_WAIT_RETURN) return;

    float shootSpriteWidth = 800.0f, shootSpriteHeight = 800.0f;
    Rectangle destShoot = { shark->rect.x - 300.0f, shark->rect.y - 450.0f, shootSpriteWidth, shootSpriteHeight };

    float idleSpriteWidth = 800.0f, idleSpriteHeight = 800.0f;
    Rectangle destIdle = { shark->rect.x - 300.0f, shark->rect.y - 450.0f, idleSpriteWidth, idleSpriteHeight };

    float dashSpriteWidth = 1150.0f, dashSpriteHeight = 650.0f;
    Rectangle destDash = { shark->rect.x - 225.0f, shark->rect.y - 230.0f, dashSpriteWidth, dashSpriteHeight };

    float jumpSpriteWidth = 1550.0f, jumpSpriteHeight = 1050.0f;
    Rectangle destJump = { shark->rect.x - 400.0f, shark->rect.y - 200.0f, jumpSpriteWidth, jumpSpriteHeight };
    Color bossTint = shark->hitFlashTimer > 0.0f ? RED : WHITE;

    switch (shark->state) {
        case SHARK_IDLE:
            DrawSharkFrame(shark->texIdle, shark->animFrame, 9, destIdle, false, bossTint);
            break;

        case SHARK_PREP_LEFT:
            DrawSharkFrame(shark->texDashLeft, shark->animFrame, 6, destDash, true, bossTint);
            break;

        case SHARK_DASH_RIGHT:
            DrawSharkFrame(shark->texDashRight, shark->animFrame, 6, destDash, false, bossTint);
            break;

        case SHARK_DASH_LEFT:
            DrawSharkFrame(shark->texDashLeft, shark->animFrame, 6, destDash, true, bossTint);
            break;

        case SHARK_SHOOTING:
            DrawSharkFrame(shark->texShoot, shark->animFrame, 9, destShoot, false, bossTint);
            break;

        case SHARK_ARC_ATTACK:
            DrawSharkFrame(shark->texJump, shark->animFrame, 12, destJump, false, bossTint);
            break;

        default: break;
    }
}

void UnloadShark(Shark *shark) {
    UnloadTexture(shark->texShoot);
    UnloadTexture(shark->texIdle);
    UnloadTexture(shark->texDashLeft);
    UnloadTexture(shark->texDashRight);
    UnloadTexture(shark->texJump);
    UnloadTexture(shark->texBubble);
    UnloadTexture(shark->texWaterBubble);
    UnloadTexture(shark->texDeath);
}

void DamageShark(Shark *shark, int damage) {
    if (damage <= 0) return;
    if (!shark->active || shark->dying) return;

    shark->health -= damage;
    shark->hitFlashTimer = SHARK_HIT_FLASH_DURATION;
    if (shark->health <= 0) {
        shark->health = 0;
        shark->active = false;
        shark->dying = true;
        shark->deathTimer = 0.0f;
        shark->deathStartY = shark->rect.y;
        shark->velocity = (Vector2){0.0f, 0.0f};

        for (int i = 0; i < MAX_WATER_BALLS; i++) {
            shark->balls[i].active = false;
        }
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
            hitboxOffset = (Vector2){ 5.0f, -25.0f };
            hitboxSize = (Vector2){ 540.0f, 480.0f };
            break;
        case SHARK_DASH_RIGHT:
            hitboxOffset = (Vector2){ -60.0f, -60.0f };
            hitboxSize = (Vector2){ 650.0f, 540.0f };
            break;
        case SHARK_DASH_LEFT:
            hitboxOffset = (Vector2){ 5.0f, -25.0f };
            hitboxSize = (Vector2){ 540.0f, 480.0f };
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
