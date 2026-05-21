#ifndef SOUNDS_H
#define SOUNDS_H

void InitSoundSystem(void);
void UnloadSoundSystem(void);
void SetSoundSystemVolume(float volume);

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
