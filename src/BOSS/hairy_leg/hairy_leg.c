#include "hairy_leg.h"

void InitHairyLeg(HairyLeg *leg, Vector2 startPosition) {
    leg->rect = (Rectangle){ startPosition.x, startPosition.y, 80, 200 };
    leg->state = HL_IDLE;
    leg->timer = 0.0f;
    leg->health = 100;
    leg->waveLeft.active = false;
    leg->waveRight.active = false;
    leg->isKickActive = false;
}

void UpdateHairyLeg(HairyLeg *leg, Rectangle playerRect, float deltaTime) {
    if (leg->waveLeft.active) leg->waveLeft.rect.x -= leg->waveLeft.speed.x * deltaTime;
    if (leg->waveRight.active) leg->waveRight.rect.x += leg->waveRight.speed.x * deltaTime;

    switch (leg->state) {
        case HL_IDLE:
            leg->timer += deltaTime;
            if (leg->timer > 3.0f) {
                leg->state = HL_JUMPING_UP;
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
            break;

        case HL_KICKING:
            break;
    }
}

void DrawHairyLeg(HairyLeg *leg) {
    Color legColor = (leg->state == HL_VULNERABLE) ? BLUE : DARKBROWN;
    DrawRectangleRec(leg->rect, legColor);

    if (leg->waveLeft.active) DrawRectangleRec(leg->waveLeft.rect, RED);
    if (leg->waveRight.active) DrawRectangleRec(leg->waveRight.rect, RED);
}
