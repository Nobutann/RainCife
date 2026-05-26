#ifndef MIDNIGHT_MAN_H
#define MIDNIGHT_MAN_H

#include "graphics/sprites.h"
#include "raylib.h"

#define MM_HAND_COUNT 3
#define MM_MAX_UMBRELLAS 16

typedef enum {
  MM_IDLE,
  MM_GROUND_TELEGRAPH,
  MM_GROUND_RISE,
  MM_GROUND_RETREAT,
  MM_GROUND_PHASE2_TELEGRAPH,
  MM_GROUND_PHASE2_RISE,
  MM_GROUND_PHASE2_PAUSE,
  MM_GROUND_PHASE2_RETREAT,
  MM_CEILING_TELEGRAPH,
  MM_CEILING_SLAM,
  MM_CEILING_RETREAT,
  MM_FOLLOW_TELEGRAPH,
  MM_FOLLOW_SLAM,
  MM_FOLLOW_RETREAT,
  MM_ARM_STORM_ENTER,
  MM_ARM_STORM_ACTIVE,
  MM_ARM_STORM_RETREAT,
  MM_DEAD
} MidnightManState;

typedef struct {
  Vector2 position;
  Vector2 velocity;
  float scale;
  bool active;
  bool isBig;
  float animTimer;
  int animFrame;
} MMUmbrella;

typedef struct {
  Rectangle rect;
  Rectangle hitbox;
  Vector2 speed;
  bool active;
  int currentFrame;
  float frameTimer;
} MMShockwave;

struct Player;
typedef struct Player Player;

typedef struct {
  bool active;
  int health;
  float hitFlashTimer;
  MidnightManState state;
  float timer;

  Texture2D texHandOpen;
  Texture2D texFist;      // Punch.png
  Texture2D texUmbrella;  // sombrinha-Sheet.png
  Texture2D texArm;       // Arms_idle.png
  Texture2D texShockwave; // shockwave.png

  float handXPositions[MM_HAND_COUNT];
  float handsY;
  float telegraphY;
  float riseStopY;
  float handDrawWidth;
  float handDrawHeight;

  // Hitboxes for player collision and player attacks
  Rectangle handHitboxes[MM_HAND_COUNT];
  bool handActive[MM_HAND_COUNT];

  // Sweep attack details
  float sweepX;
  float sweepY;
  float sweepWidth;
  float sweepHeight;
  int sweepDirection; // 1 = Left to Right, -1 = Right to Left

  // Homing fist 2 details
  float follow2X;
  float follow2Y;
  float follow2VelX;
  float follow2VelY;
  float follow2Angle;

  // Sequential shockwaves and phases for Ceiling Slam
  MMShockwave waveLeft;
  MMShockwave waveRight;
  int ceilingPhase;

  // Umbrella storm details
  MMUmbrella umbrellas[MM_MAX_UMBRELLAS];
  float umbrellaSpawnTimer;
  int attackCycle; // to cycle or track attacks

  Animation animShadow; // Hairy Leg style warning shadow alert
} MidnightMan;

void InitMidnightMan(MidnightMan *mm, int screenWidth, int screenHeight,
                     float groundY);
void UpdateMidnightMan(MidnightMan *mm, Rectangle playerRect, float deltaTime,
                       int screenWidth, int screenHeight, float groundY);
void DrawMidnightMan(const MidnightMan *mm);
void UnloadMidnightMan(MidnightMan *mm);

bool TryDamageMidnightManFromPlayerAttack(MidnightMan *mm, Player *player,
                                          float playerScale);
bool IsMidnightManColliding(const MidnightMan *mm, Rectangle playerHitbox);

#endif
