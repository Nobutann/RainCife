#include "entities/hairy_leg.h"
#include "entities/player.h"

void InitHairyLeg(HairyLeg *leg, Vector2 startPosition, float groundY, float scale) {
    LoadHairyLegSprites(&leg->sprites);
    leg->currentAnim = &leg->sprites.idle;
    leg->groundY = groundY;
    float spriteH = (float)leg->currentAnim->sheet.height * scale;
    leg->rect = (Rectangle){ startPosition.x, groundY - spriteH, 70.0f * scale, spriteH };
    leg->state = HL_IDLE;
    leg->timer = 0.0f;
    leg->health = 100;
    leg->waveLeft.active = false;
    leg->waveRight.active = false;
    leg->isKickActive = false;
    leg->direction = -1;
}

void UpdateHairyLeg(HairyLeg *leg, Rectangle playerRect, float deltaTime, float groundY, float scale) {
    leg->groundY = groundY;
    float currentSpriteH = (float)leg->currentAnim->sheet.height * scale;
    float oldHeight = leg->rect.height;
    leg->rect.height = currentSpriteH;

    if (leg->state != HL_JUMPING_UP && leg->state != HL_FALLING && leg->state != HL_HANGING) {
        leg->rect.y = leg->groundY - currentSpriteH;
    }

    if (leg->state == HL_IDLE || leg->state == HL_VULNERABLE) {
        float centroRato = playerRect.x + (playerRect.width / 2.0f);
        float centroPerna = leg->rect.x + (leg->rect.width / 2.0f);
        if (centroRato > centroPerna) {
            leg->direction = 1;
        } else {
            leg->direction = -1;
        }
    }
    if (leg->waveLeft.active) leg->waveLeft.rect.x -= leg->waveLeft.speed.x * deltaTime;
    if (leg->waveRight.active) leg->waveRight.rect.x += leg->waveRight.speed.x * deltaTime;

    switch (leg->state) {
        case HL_IDLE:
            leg->timer += deltaTime;
            if (leg->timer > 3.0f) {
                int chance = GetRandomValue(1, 100);

                if (leg->health > 70) {
                    if (chance <= 60) { leg->state = HL_KICKING; leg->sprites.kick.currentFrame = 0; leg->sprites.kick.timer = 0.0f; }
                    else if (chance <= 80) { leg->state = HL_SWEEPING; leg->sprites.rasteira.currentFrame = 0; leg->sprites.rasteira.timer = 0.0f; }
                    else { leg->state = HL_JUMPING_UP; leg->sprites.jump.currentFrame = 0; leg->sprites.jump.timer = 0.0f; }
                }
                else if (leg->health > 40) {
                    if (chance <= 33) { leg->state = HL_KICKING; leg->sprites.kick.currentFrame = 0; leg->sprites.kick.timer = 0.0f; }
                    else if (chance <= 66) { leg->state = HL_SWEEPING; leg->sprites.rasteira.currentFrame = 0; leg->sprites.rasteira.timer = 0.0f; }
                    else { leg->state = HL_JUMPING_UP; leg->sprites.jump.currentFrame = 0; leg->sprites.jump.timer = 0.0f; }
                }
                else {
                    if (chance <= 40) { leg->state = HL_JUMPING_UP; leg->sprites.jump.currentFrame = 0; leg->sprites.jump.timer = 0.0f; }
                    else if (chance <= 80) { leg->state = HL_SWEEPING; leg->sprites.rasteira.currentFrame = 0; leg->sprites.rasteira.timer = 0.0f; }
                    else { leg->state = HL_KICKING; leg->sprites.kick.currentFrame = 0; leg->sprites.kick.timer = 0.0f; }
                }
                leg->timer = 0.0f;
            }
            break;

        case HL_JUMPING_UP:
            leg->rect.y -= 1500 * deltaTime;
            if (leg->rect.y < -leg->rect.height) {
                leg->state = HL_HANGING;
                leg->rect.x = playerRect.x;
            }
            break;

        case HL_HANGING:
            leg->timer += deltaTime;
            leg->rect.x = playerRect.x;

            if(leg->timer > 2.0f){
                leg->state = HL_FALLING;
                leg->sprites.fall.currentFrame = 0;
                leg->sprites.fall.timer = 0.0f;
                leg->timer = 0.0f;
            }

            break;


        case HL_FALLING:
            leg->rect.y += 2000 * deltaTime;
            if (leg->rect.y >= leg->groundY - currentSpriteH) {
                float waveW = 25.0f * scale;
                float waveH = 35.0f * scale;
                leg->rect.y = leg->groundY - currentSpriteH;
                leg->waveLeft = (Shockwave){ {leg->rect.x, leg->rect.y + currentSpriteH * 0.75f, waveW, waveH}, {600, 0}, true };
                leg->waveRight = (Shockwave){ {leg->rect.x + leg->rect.width, leg->rect.y + currentSpriteH * 0.75f, waveW, waveH}, {600, 0}, true };

                leg->state = HL_VULNERABLE;
                leg->sprites.idle.currentFrame = 0;
                leg->timer = 0.0f;
            }
            break;

        case HL_VULNERABLE:
            leg->timer += deltaTime;
            if (leg->timer > 2.5f) {
                leg->state = HL_IDLE;
                leg->sprites.idle.currentFrame = 0;
                leg->timer = 0.0f;
            }
            break;

        case HL_SWEEPING:
            leg->timer += deltaTime;
            if (leg->timer < 0.5f) {
                if (leg->rect.y < leg->groundY - currentSpriteH) leg->rect.y += 400 * deltaTime;
                leg->rect.x -= 100 * leg->direction * deltaTime;
            }
            else if (leg->timer < 1.2f) {
                leg->rect.x += 1000 * leg->direction * deltaTime;
            }
            else {
                leg->state = HL_VULNERABLE;
                leg->sprites.idle.currentFrame = 0;
                leg->timer = 0.0f;
            }
            break;

        case HL_KICKING:
            leg->timer += deltaTime;
            if (leg->timer < 0.4f) {
                leg->rect.x += 500 * leg->direction * deltaTime;
                leg->isKickActive = false;
            }
            else if (leg->timer < 0.7f) {
                float larguraHitbox = 80.0f * scale;
                float alturaHitbox = 40.0f * scale;
                float hitboxX = (leg->direction == 1) ? (leg->rect.x + leg->rect.width) : (leg->rect.x - larguraHitbox);
                leg->kickHitbox = (Rectangle){ hitboxX, leg->rect.y + currentSpriteH * 0.7f, larguraHitbox, alturaHitbox };
                leg->isKickActive = true;
            }
            else if (leg->timer < 1.2f) {
                leg->isKickActive = false;
                leg->rect.x -= 20 * leg->direction * deltaTime;
            }
            else {
                leg->state = HL_VULNERABLE;
                leg->sprites.idle.currentFrame = 0;
                leg->timer = 0.0f;
            }
            break;
    }

    switch (leg->state) {
        case HL_IDLE:
        case HL_VULNERABLE: leg->currentAnim = &leg->sprites.idle; break;
        case HL_JUMPING_UP: leg->currentAnim = &leg->sprites.jump; break;
        case HL_HANGING: leg->currentAnim = &leg->sprites.jump; break;
        case HL_FALLING: leg->currentAnim = &leg->sprites.fall; break;
        case HL_KICKING: leg->currentAnim = &leg->sprites.kick; break;
        case HL_SWEEPING: leg->currentAnim = &leg->sprites.rasteira; break;
    }

    bool isAttack = (leg->state == HL_KICKING || leg->state == HL_SWEEPING);
    bool isLastFrame = (leg->currentAnim->currentFrame >= leg->currentAnim->frameCount - 1);

    if (!(isAttack && isLastFrame)) {
        UpdateAnimation(leg->currentAnim, deltaTime);
    }
}

void DrawHairyLeg(HairyLeg *leg, float scale) {
    bool flipX = (leg->direction == 1);
    float offsetX = flipX ? -(scale * 75.0f) : -(scale * 100.0f);
    float currentSpriteH = (float)leg->currentAnim->sheet.height * scale;
    float offsetY = 0.0f;

    Vector2 posicaoBoss = { leg->rect.x + offsetX, leg->rect.y + offsetY };
    DrawAnimationFrame(leg->currentAnim, posicaoBoss, scale, flipX, WHITE);

    // Linhas de debug para hitboxes
    DrawRectangleLinesEx(leg->rect, 2.0f, RED); // Hitbox principal

    if (leg->isKickActive) {
        DrawRectangleLinesEx(leg->kickHitbox, 2.0f, ORANGE); // Hitbox do chute
    }

    if (leg->waveLeft.active) {
        DrawRectangleLinesEx(leg->waveLeft.rect, 2.0f, BLUE); // Onda de choque esquerda
    }
    if (leg->waveRight.active) {
        DrawRectangleLinesEx(leg->waveRight.rect, 2.0f, BLUE); // Onda de choque direita
    }
}

void UnloadHairyLeg(HairyLeg *leg) {
    UnloadHairyLegSprites(&leg->sprites);
}
