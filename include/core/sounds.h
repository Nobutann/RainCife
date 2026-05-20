#ifndef SOUNDS_H
#define SOUNDS_H

void InitSoundSystem(void);
void UnloadSoundSystem(void);
void SetSoundSystemVolume(float volume);

void PlayJumpSound(void);
void PlayAttackSwordSound(void);
void PlayAttackHammerSound(void);
void PlayShootSound(void);

#endif
