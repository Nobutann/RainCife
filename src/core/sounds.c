#include "core/sounds.h"
#include <raylib.h>
#include <stddef.h>

static Sound jumpSound;
static Sound attackSwordSound;
static Sound attackHammerSound;
static Sound shootSound;

static Sound sharkShootSound;
static Sound sharkSwimmingSound;
static Sound sharkJumpSound;
static Sound sharkBubbleSound;

static Sound legStompSound;
static Sound legJumpSound;
static Sound legSweepSound;
static Sound legShockwaveSound;

// Midnight Man Boss Sounds
static Sound midnightManUmbrellaSound;
static Sound midnightManShockwaveSound;
static Sound midnightManArmMoveSound;

static float currentVolume = 1.0f;
static float currentMusicVolume = 1.0f;
static Music menuMusic;
static Music level1RunMusic;
static Music level2RunMusic;
static Music level3RunMusic;
static Music hairyLegMusic;
static Music sharkMusic;
static Music midnightManMusic;
static SoundtrackId currentSoundtrack = SOUNDTRACK_NONE;

static Music *GetSoundtrackMusic(SoundtrackId soundtrack)
{
    switch (soundtrack)
    {
        case SOUNDTRACK_MENU: return &menuMusic;
        case SOUNDTRACK_LEVEL1_RUN: return &level1RunMusic;
        case SOUNDTRACK_LEVEL2_RUN: return &level2RunMusic;
        case SOUNDTRACK_LEVEL3_RUN: return &level3RunMusic;
        case SOUNDTRACK_HAIRY_LEG: return &hairyLegMusic;
        case SOUNDTRACK_SHARK: return &sharkMusic;
        case SOUNDTRACK_MIDNIGHT_MAN: return &midnightManMusic;
        default: return NULL;
    }
}

void InitSoundSystem(void)
{
    InitAudioDevice();
    jumpSound = LoadSound("assets/sprites/Player/Soms/Pulo.wav");
    attackHammerSound = LoadSound("assets/sprites/Player/Soms/Som_ataque_marreta.wav");
    attackSwordSound = LoadSound("assets/sprites/Player/Soms/Som_ataque_sword.wav");
    shootSound = LoadSound("assets/sprites/Player/Soms/Som_tiro.wav");

    sharkShootSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/atirando_bolas.wav");
    sharkSwimmingSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/nadando.wav");
    sharkJumpSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/salto.wav");
    sharkBubbleSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/gota_caindo.wav");

    legStompSound = LoadSound("assets/sprites/Boss/Sons_bosses/perna/pesada.wav");
    legJumpSound = LoadSound("assets/sprites/Boss/Sons_bosses/perna/pesada.wav");
    legSweepSound = LoadSound("assets/sprites/Boss/Sons_bosses/perna/rasteira.wav");
    legShockwaveSound = LoadSound("assets/sprites/Boss/Sons_bosses/perna/shockwave.wav");

    midnightManUmbrellaSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/gota_caindo.wav");
    midnightManShockwaveSound = LoadSound("assets/sprites/Boss/Sons_bosses/perna/shockwave.wav");
    midnightManArmMoveSound = LoadSound("assets/sprites/Boss/Sons_bosses/tubarao/nadando.wav");

    menuMusic = LoadMusicStream("assets/soundtrack/MoF Stage 3 - The Gensokyo the Gods Loved.mp3");
    menuMusic.looping = true;
    level1RunMusic = LoadMusicStream("assets/soundtrack/ZUN_Touhou_15_OST_-_Unforgettable_the_Nostalgic_Greenery_1_Stage_theme_(SkySound.cc).mp3");
    level1RunMusic.looping = true;
    level2RunMusic = LoadMusicStream("assets/soundtrack/13. Emotional Skyscraper ~ Cosmic Mind.mp3");
    level2RunMusic.looping = true;
    level3RunMusic = LoadMusicStream("assets/soundtrack/SA Stage 2 - The Bridge People No Longer Cross.mp3");
    level3RunMusic.looping = true;
    hairyLegMusic = LoadMusicStream("assets/soundtrack/Touhou_-_Hartmann_s_Youkai_Girl_Koishi_Komeiji_s_theme_(mp3.pm).mp3");
    hairyLegMusic.looping = true;
    sharkMusic = LoadMusicStream("assets/soundtrack/Demetori_-_Solar_Sect_of_Mystic_Wisdom_-Nuclear_Fusion_(mp3.pm).mp3");
    sharkMusic.looping = true;
    midnightManMusic = LoadMusicStream("assets/soundtrack/LoLK Junko's Theme_ Pure Furies Whereabouts of the Heart.mp3");
    midnightManMusic.looping = true;

    SetSoundSystemVolume(currentVolume);
    SetMusicSystemVolume(currentMusicVolume);
}

void SetSoundSystemVolume(float volume)
{
    currentVolume = volume;
    SetSoundVolume(jumpSound, volume);
    SetSoundVolume(attackHammerSound, volume);
    SetSoundVolume(attackSwordSound, volume);
    SetSoundVolume(shootSound, volume);

    SetSoundVolume(sharkShootSound, volume);
    SetSoundVolume(sharkSwimmingSound, volume);
    SetSoundVolume(sharkJumpSound, volume);
    SetSoundVolume(sharkBubbleSound, volume);

    SetSoundVolume(legStompSound, volume);
    SetSoundVolume(legJumpSound, volume);
    SetSoundVolume(legSweepSound, volume);
    SetSoundVolume(legShockwaveSound, volume);

    // Set Midnight Man sounds volume
    SetSoundVolume(midnightManUmbrellaSound, volume);
    SetSoundVolume(midnightManShockwaveSound, volume);
    SetSoundVolume(midnightManArmMoveSound, volume);
}

void SetMusicSystemVolume(float volume)
{
    currentMusicVolume = volume;
    SetMusicVolume(menuMusic, volume);
    SetMusicVolume(level1RunMusic, volume);
    SetMusicVolume(level2RunMusic, volume);
    SetMusicVolume(level3RunMusic, volume);
    SetMusicVolume(hairyLegMusic, volume);
    SetMusicVolume(sharkMusic, volume);
    SetMusicVolume(midnightManMusic, volume);
}

void PlaySoundtrack(SoundtrackId soundtrack)
{
    if (currentSoundtrack == soundtrack)
    {
        Music *music = GetSoundtrackMusic(soundtrack);
        if (music != NULL && !IsMusicStreamPlaying(*music))
        {
            PlayMusicStream(*music);
        }
        return;
    }

    StopSoundtrack();
    currentSoundtrack = soundtrack;

    Music *music = GetSoundtrackMusic(soundtrack);
    if (music != NULL)
    {
        SetMusicVolume(*music, currentMusicVolume);
        PlayMusicStream(*music);
    }
}

void StopSoundtrack(void)
{
    Music *music = GetSoundtrackMusic(currentSoundtrack);
    if (music != NULL)
    {
        StopMusicStream(*music);
    }
    currentSoundtrack = SOUNDTRACK_NONE;
}

void UpdateSoundtrack(void)
{
    Music *music = GetSoundtrackMusic(currentSoundtrack);
    if (music != NULL)
    {
        UpdateMusicStream(*music);
    }
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

// Midnight Man Sound Play Wrappers
void PlayMidnightManUmbrellaSound(void)
{
    PlaySound(midnightManUmbrellaSound);
}

void PlayMidnightManShockwaveSound(void)
{
    PlaySound(midnightManShockwaveSound);
}

void PlayMidnightManArmMoveSound(void)
{
    PlaySound(midnightManArmMoveSound);
}

void UnloadSoundSystem(void)
{
    StopSoundtrack();
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(level1RunMusic);
    UnloadMusicStream(level2RunMusic);
    UnloadMusicStream(level3RunMusic);
    UnloadMusicStream(hairyLegMusic);
    UnloadMusicStream(sharkMusic);
    UnloadMusicStream(midnightManMusic);

    UnloadSound(jumpSound);
    UnloadSound(attackHammerSound);
    UnloadSound(attackSwordSound);
    UnloadSound(shootSound);

    UnloadSound(sharkShootSound);
    UnloadSound(sharkSwimmingSound);
    UnloadSound(sharkJumpSound);
    UnloadSound(sharkBubbleSound);

    UnloadSound(legStompSound);
    UnloadSound(legJumpSound);
    UnloadSound(legSweepSound);
    UnloadSound(legShockwaveSound);

    // Unload Midnight Man sounds
    UnloadSound(midnightManUmbrellaSound);
    UnloadSound(midnightManShockwaveSound);
    UnloadSound(midnightManArmMoveSound);

    CloseAudioDevice();
}
