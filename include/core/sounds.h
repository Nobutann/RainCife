#ifndef SOUNDS_H
#define SOUNDS_H

void InitSoundSystem(void);
void UnloadSoundSystem(void);
void SetSoundSystemVolume(float volume);
void SetMusicSystemVolume(float volume);

typedef enum
{
    SOUNDTRACK_NONE,
    SOUNDTRACK_LEVEL1_RUN,
    SOUNDTRACK_HAIRY_LEG
} SoundtrackId;

void PlaySoundtrack(SoundtrackId soundtrack);
void StopSoundtrack(void);
void UpdateSoundtrack(void);

void PlayJumpSound(void);
void PlayAttackSwordSound(void);
void PlayAttackHammerSound(void);
void PlayShootSound(void);

// Boss: Tubarao (Shark) sounds
void PlaySharkShootSound(void);
void PlaySharkSwimmingSound(void);
void StopSharkSwimmingSound(void);
void PlaySharkJumpSound(void);
void PlaySharkBubbleSound(void);

// Boss: Perna (Hairy Leg) sounds
void PlayLegStompSound(void);
void PlayLegJumpSound(void);
void PlayLegSweepSound(void);
void StopLegSweepSound(void);
void PlayLegShockwaveSound(void);

#endif
