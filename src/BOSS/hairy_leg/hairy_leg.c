#include "hairy_leg.h"

void InitHairyLeg(HairyLeg *leg, Vector2 startPosition) {
    leg->rect = (Rectangle){ startPosition.x, startPosition.y, 80, 200 };
    leg->state = HL_IDLE;
    leg->timer = 0.0f;
    leg->health = 100;
    leg->waveLeft.active = false;
    leg->waveRight.active = false;
    leg->isKickActive = false;
    leg->direction = -1;
}

void UpdateHairyLeg(HairyLeg *leg, Rectangle playerRect, float deltaTime) {
    if (leg->waveLeft.active) leg->waveLeft.rect.x -= leg->waveLeft.speed.x * deltaTime;
    if (leg->waveRight.active) leg->waveRight.rect.x += leg->waveRight.speed.x * deltaTime;

    switch (leg->state) {
        case HL_IDLE:
        if (playerRect.x > leg->rect.x) {
            leg->direction = 1;  // rato ta na direita fi
        } else {
            leg->direction = -1; // rato ta na esquerda
        }
        leg->timer += deltaTime;
        if (leg->timer > 3.0f) {
            int chance = GetRandomValue(1, 100);

            if (leg->health > 70) {
                if (chance <= 60) {
                    leg->state = HL_KICKING;     // 60% de chance
                } else if (chance <= 80) {
                    leg->state = HL_SWEEPING;    // 20% de chance
                } else {
                    leg->state = HL_JUMPING_UP;  // 20% de chance
                }
            }
            else if (leg->health > 40) {
                if (chance <= 33) {
                    leg->state = HL_KICKING;     // ~33% de chance
                } else if (chance <= 66) {
                    leg->state = HL_SWEEPING;    // ~33% de chance
                } else {
                    leg->state = HL_JUMPING_UP;  // ~34% de chance
                }
            }
            else {
                if (chance <= 40) {
                    leg->state = HL_JUMPING_UP;  // 40% de chance
                } else if (chance <= 80) {
                    leg->state = HL_SWEEPING;    // 40% de chance
                } else {
                    leg->state = HL_KICKING;     // 20% de chance
                }
            }
            leg->timer = 0.0f;
            }
            break;

        case HL_JUMPING_UP:
            leg->rect.y -= 500 * deltaTime;
            if (leg->rect.y < -leg->rect.height) {
                leg->state = HL_FALLING;
                leg->rect.x = playerRect.x;
            }
            break;

        case HL_FALLING:
            leg->rect.y += 800 * deltaTime;

            if (leg->rect.y >= 500 - leg->rect.height) {
                leg->rect.y = 500 - leg->rect.height;

                leg->waveLeft = (Shockwave){ {leg->rect.x, 480, 40, 20}, {400, 0}, true };
                leg->waveRight = (Shockwave){ {leg->rect.x + leg->rect.width, 480, 40, 20}, {400, 0}, true };

                leg->state = HL_VULNERABLE;
                leg->timer = 0.0f;
            }
            break;

        case HL_VULNERABLE:
            leg->timer += deltaTime;
            if (leg->timer > 2.5f) {
                leg->state = HL_IDLE;
                leg->timer = 0.0f;
            }
            break;

        case HL_SWEEPING:
        leg->timer += deltaTime;

        if (leg->timer < 0.5f) {
            if (leg->rect.y < 500 - leg->rect.height) {
                leg->rect.y += 400 * deltaTime;
            }
            leg->rect.x -= 100 * leg->direction * deltaTime;
        }
        else if (leg->timer < 1.2f) {
            leg->rect.x += 1000 * leg->direction * deltaTime;
        }
        else {
            leg->state = HL_VULNERABLE;
            leg->timer = 0.0f;
        }
        break;

        case HL_KICKING:
            leg->timer += deltaTime;

            if (leg->timer < 0.4f) {
                leg->rect.x -= 80 * leg->direction * deltaTime;
                leg->isKickActive = false;
            }
            else if (leg->timer < 0.7f) {
                float hitboxX;
                if (leg->direction == 1) {
                    hitboxX = leg->rect.x + leg->rect.width;
                } else {
                    hitboxX = leg->rect.x - 80;
                }

                leg->kickHitbox = (Rectangle){ hitboxX, leg->rect.y + 100, 80, 50 };
                leg->isKickActive = true;
            }
            else if (leg->timer < 1.2f) {
                leg->isKickActive = false;
                leg->rect.x -= 20 * leg->direction * deltaTime;
            }
            else {
                leg->state = HL_VULNERABLE;
                leg->timer = 0.0f;
            }
        break;
    }
}

void DrawHairyLeg(HairyLeg *leg) {
    Color legColor = (leg->state == HL_VULNERABLE) ? BLUE : DARKBROWN;
    DrawRectangleRec(leg->rect, legColor);

    if (leg->waveLeft.active) DrawRectangleRec(leg->waveLeft.rect, RED);
    if (leg->waveRight.active) DrawRectangleRec(leg->waveRight.rect, RED);
    if (leg->isKickActive) {
        DrawRectangleRec(leg->kickHitbox, ORANGE);
    }
}
