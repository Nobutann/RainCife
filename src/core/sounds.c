#include "core/sounds.h"
#include <raylib.h>

static Sound jumpSound;
static Sound attackSwordSound;
static Sound attackHammerSound;
static Sound shootSound;

// Shark Boss Sounds
static Sound sharkShootSound;
static Sound sharkSwimmingSound;
static Sound sharkJumpSound;
static Sound sharkBubbleSound;

// Hairy Leg Boss Sounds
static Sound legStompSound;
static Sound legJumpSound;
static Sound legSweepSound;
static Sound legShockwaveSound;

static float currentVolume = 1.0f;

void InitSoundSystem(void)
{
    InitAudioDevice();
    jumpSound = LoadSound("assets/sprites/Player/Soms/Pulo.wav");
    attackHammerSound = LoadSound("assets/sprites/Player/Soms/Som_ataque_marreta.wav");
    attackSwordSound = LoadSound("assets/sprites/Player/Soms/Som_ataque_sword.wav");
    shootSound = LoadSound("assets/sprites/Player/Soms/Som_tiro.wav");

    // Load Shark sounds
    sharkShootSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/atirando_bolas.wav");
    sharkSwimmingSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/nadando.wav");
    sharkJumpSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/salto.wav");
    sharkBubbleSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/gota_caindo.wav");

    // Load Hairy Leg sounds
    legStompSound = LoadSound("assets/sprites/Boss/Sons_bosses/perna/pesada.wav");
    legJumpSound = LoadSound("assets/sprites/Boss/Sons_bosses/perna/pesada.wav");
    legSweepSound = LoadSound("assets/sprites/Boss/Sons_bosses/perna/rasteira.wav");
    legShockwaveSound = LoadSound("assets/sprites/Boss/Sons_bosses/perna/shockwave.wav");

    // Apply the initial volume setting to all newly loaded sounds
    SetSoundSystemVolume(currentVolume);
}

void SetSoundSystemVolume(float volume)
{
    currentVolume = volume;
    SetSoundVolume(jumpSound, volume);
    SetSoundVolume(attackHammerSound, volume);
    SetSoundVolume(attackSwordSound, volume);
    SetSoundVolume(shootSound, volume);

    // Set Shark sounds volume
    SetSoundVolume(sharkShootSound, volume);
    SetSoundVolume(sharkSwimmingSound, volume);
    SetSoundVolume(sharkJumpSound, volume);
    SetSoundVolume(sharkBubbleSound, volume);

    // Set Hairy Leg sounds volume
    SetSoundVolume(legStompSound, volume);
    SetSoundVolume(legJumpSound, volume);
    SetSoundVolume(legSweepSound, volume);
    SetSoundVolume(legShockwaveSound, volume);
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

// Shark Sound Play/Stop Wrappers
void PlaySharkShootSound(void)
{
    PlaySound(sharkShootSound);
}

void PlaySharkSwimmingSound(void)
{
    PlaySound(sharkSwimmingSound);
}

void StopSharkSwimmingSound(void)
{
    if (IsSoundPlaying(sharkSwimmingSound))
    {
        StopSound(sharkSwimmingSound);
    }
}

void PlaySharkJumpSound(void)
{
    PlaySound(sharkJumpSound);
}

void PlaySharkBubbleSound(void)
{
    PlaySound(sharkBubbleSound);
}

// Hairy Leg Sound Play Wrappers
void PlayLegStompSound(void)
{
    PlaySound(legStompSound);
}

void PlayLegJumpSound(void)
{
    PlaySound(legJumpSound);
}

void PlayLegSweepSound(void)
{
    PlaySound(legSweepSound);
}

void StopLegSweepSound(void)
{
    if (IsSoundPlaying(legSweepSound))
    {
        StopSound(legSweepSound);
    }
}

void PlayLegShockwaveSound(void)
{
    PlaySound(legShockwaveSound);
}

void UnloadSoundSystem(void)
{
    UnloadSound(jumpSound);
    UnloadSound(attackHammerSound);
    UnloadSound(attackSwordSound);
    UnloadSound(shootSound);

    // Unload Shark sounds
    UnloadSound(sharkShootSound);
    UnloadSound(sharkSwimmingSound);
    UnloadSound(sharkJumpSound);
    UnloadSound(sharkBubbleSound);

    // Unload Hairy Leg sounds
    UnloadSound(legStompSound);
    UnloadSound(legJumpSound);
    UnloadSound(legSweepSound);
    UnloadSound(legShockwaveSound);

    CloseAudioDevice();
}
