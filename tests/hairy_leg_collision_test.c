#include <assert.h>
#include <stdbool.h>

#include "entities/hairy_leg.h"

extern bool IsHairyLegKickColliding(const HairyLeg *leg, Rectangle playerHitbox);

int main(void)
{
    HairyLeg leg = {0};
    Rectangle playerHitbox = {20.0f, 30.0f, 10.0f, 10.0f};

    leg.isKickActive = true;
    leg.kickHitbox = (Rectangle){10.0f, 20.0f, 40.0f, 30.0f};
    leg.waveLeft.active = true;
    leg.waveLeft.rect = (Rectangle){200.0f, 200.0f, 10.0f, 10.0f};

    assert(IsHairyLegKickColliding(&leg, playerHitbox));

    leg.isKickActive = false;
    assert(!IsHairyLegKickColliding(&leg, playerHitbox));

    leg.isKickActive = true;
    playerHitbox = (Rectangle){200.0f, 200.0f, 5.0f, 5.0f};
    assert(!IsHairyLegKickColliding(&leg, playerHitbox));

    return 0;
}
