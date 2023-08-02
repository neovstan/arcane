#include "CAEAudioEnvironment.h"

sReverbEnvironment(&gAudioZoneToReverbEnvironmentMap)[NUM_AUDIO_ENVIRONMENTS] = *(sReverbEnvironment(*)[NUM_AUDIO_ENVIRONMENTS])0x8AD670;
float(&gSoundDistAttenuationTable)[NUM_SOUND_DIST_ATTENUATION_ENTRIES] = *(float(*)[NUM_SOUND_DIST_ATTENUATION_ENTRIES])0x8AC270;

float CAEAudioEnvironment::GetDopplerRelativeFrequency(float prevDist,
                                                       float curDist,
                                                       unsigned int prevTime,
                                                       unsigned int curTime,
                                                       float timeScale) {
    return plugin::CallAndReturn<float, 0x4D7E40, float, float, unsigned int, unsigned int, float>(prevDist, curDist, prevTime, curTime, timeScale);
}

float CAEAudioEnvironment::GetDistanceAttenuation(float dist) {
    return plugin::CallAndReturn<float, 0x4D7F20, float>(dist);
}

float CAEAudioEnvironment::GetDirectionalMikeAttenuation(CVector const& soundDir) {
    return plugin::CallAndReturn<float, 0x4D7F60, const CVector&>(soundDir);
}

void CAEAudioEnvironment::GetReverbEnvironmentAndDepth(signed char* reverbEnv, int* depth) {
    plugin::Call<0x4D8010, signed char*, int*>(reverbEnv, depth);
}

void CAEAudioEnvironment::GetPositionRelativeToCamera(CVector* vecOut, CVector* vecPos) {
    plugin::Call<0x4D80B0, CVector*, CVector*>(vecOut, vecPos);
}

void CAEAudioEnvironment::GetPositionRelativeToCamera(CVector* vecOut, CPlaceable* placeable) {
    plugin::Call<0x4D8340, CVector*, CPlaceable*>(vecOut, placeable);
}
