#include "core/sounds.h"
#include <raylib.h>

static Sound jumpSound;
static Sound attackSwordSound;
static Sound attackHammerSound;
static Sound shootSound;

static float currentVolume = 1.0f;

void InitSoundSystem(void)
{
    InitAudioDevice();
    jumpSound = LoadSound("assets/sprites/Player/Soms/Pulo.wav");
    attackHammerSound = LoadSound("assets/sprites/Player/Soms/Som_ataque_marreta.wav");
    attackSwordSound = LoadSound("assets/sprites/Player/Soms/Som_ataque_sword.wav");
    shootSound = LoadSound("assets/sprites/Player/Soms/Som_tiro.wav");
}

void SetSoundSystemVolume(float volume)
{
    currentVolume = volume;
    SetSoundVolume(jumpSound, volume);
    SetSoundVolume(attackHammerSound, volume);
    SetSoundVolume(attackSwordSound, volume);
    SetSoundVolume(shootSound, volume);
}

void PlayJumpSound(void)
{
    PlaySound(jumpSound);
}

void PlayAttackSwordSound(void)
{
    PlaySound(attackSwordSound);
}

void PlayAttackHammerSound(void)
{
    PlaySound(attackHammerSound);
}

void PlayShootSound(void)
{
    PlaySound(shootSound);
}

void UnloadSoundSystem(void)
{
    UnloadSound(jumpSound);
    UnloadSound(attackHammerSound);
    UnloadSound(attackSwordSound);
    UnloadSound(shootSound);
    CloseAudioDevice();
}
