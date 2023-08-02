#include "CAESoundManager.h"

#include "CAEAudioEnvironment.h"
#include "CAEAudioHardware.h"

CAESoundManager& AESoundManager = *(CAESoundManager*)0xB62CB0;

bool CAESoundManager::Initialise() {
    return plugin::CallMethodAndReturn<bool, 0x5B9690, CAESoundManager*>(this);
}

void CAESoundManager::Terminate() {
    plugin::CallMethod<0x4EFAA0, CAESoundManager*>(this);
}

void CAESoundManager::Reset() {
    plugin::CallMethod<0x4EF4D0, CAESoundManager*>(this);
}

void CAESoundManager::PauseManually(uchar bPause) {
    plugin::CallMethod<0x4EF510, CAESoundManager*, uchar>(this, bPause);
}

void CAESoundManager::Service() {
    plugin::CallMethod<0x4F0000, CAESoundManager*>(this);
}

CAESound* CAESoundManager::RequestNewSound(CAESound* pSound) {
    return plugin::CallMethodAndReturn<CAESound*, 0x4EFB10, CAESoundManager*, CAESound*>(this, pSound);
}

int16_t CAESoundManager::AreSoundsPlayingInBankSlot(short bankSlot) {
    return plugin::CallMethodAndReturn<int16_t, 0x4EF520, CAESoundManager*, short>(this, bankSlot);
}

int16_t CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity(short eventId, CAEAudioEntity* audioEntity) {
    return plugin::CallMethodAndReturn<int16_t, 0x4EF570, CAESoundManager*, short, CAEAudioEntity*>(this, eventId, audioEntity);
}

int16_t CAESoundManager::AreSoundsOfThisEventPlayingForThisEntityAndPhysical(short eventId,
                                                                             CAEAudioEntity* audioEntity,
                                                                             CPhysical* physical) {
    return plugin::CallMethodAndReturn<int16_t, 0x4EF5D0, CAESoundManager*, short, CAEAudioEntity*, CPhysical*>(this, eventId, audioEntity, physical);
}

void CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntity(short eventId, CAEAudioEntity* audioEntity) {
    plugin::CallMethod<0x4EF630, CAESoundManager*, short, CAEAudioEntity*>(this, eventId, audioEntity);
}

void CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(short eventId,
                                                                             CAEAudioEntity* audioEntity,
                                                                             CPhysical* physical) {
    plugin::CallMethod<0x4EFBF0, CAESoundManager*, short, CAEAudioEntity*, CPhysical*>(this, eventId, audioEntity, physical);
}

void CAESoundManager::CancelSoundsInBankSlot(short bankSlot, uchar bFullStop) {
    plugin::CallMethod<0x4EFC60, CAESoundManager*, short, uchar>(this, bankSlot, bFullStop);
}

void CAESoundManager::CancelSoundsOwnedByAudioEntity(CAEAudioEntity* audioEntity, uchar bFullStop) {
    plugin::CallMethod<0x4EFB90, CAESoundManager*, CAEAudioEntity*, uchar>(this, audioEntity, bFullStop);
}