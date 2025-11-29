#include "sound.hpp"
#include <SDL3_mixer/SDL_mixer.h>

MIX_Mixer* GoSound::mixer = nullptr;

MIX_Audio* GoSound::tap_audio = nullptr;
MIX_Audio* GoSound::capture_audio = nullptr;

MIX_Track* GoSound::tap_track = nullptr;
MIX_Track* GoSound::capture_track = nullptr;

bool GoSound::init () {
    if (!MIX_Init()) {
        SDL_Log("MIX_Init failed: %s", SDL_GetError());
        return false;
    }

    SDL_AudioSpec desired;
    SDL_zero(desired);
    desired.freq = 44100;
    desired.channels = 2;
    desired.format = SDL_AUDIO_F32;

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired);
    if (!mixer) {
        SDL_Log("MIX_CreateMixerDevice failed: %s", SDL_GetError());
        return false;
    }

    tap_audio = MIX_LoadAudio(nullptr, "assets/sound/tap_short.wav", false);
    capture_audio  = MIX_LoadAudio(nullptr, "assets/sound/capture.wav", false);

    if (!tap_audio || !capture_audio) {
        SDL_Log("Failed to load WAV: %s", SDL_GetError());
        return false;
    }

    tap_track = MIX_CreateTrack(mixer);
    capture_track  = MIX_CreateTrack(mixer);

    MIX_SetTrackAudio(tap_track, tap_audio);
    MIX_SetTrackAudio(capture_track, capture_audio);

    MIX_SetTrackGain(capture_track, 0.45f);

    return true;
}

void GoSound::playTap() {
    MIX_PlayTrack(tap_track, 0);
}

void GoSound::playCapture() {
    MIX_PlayTrack(capture_track, 0);
}

void GoSound::destroy() {
    MIX_DestroyTrack(tap_track);
    MIX_DestroyTrack(capture_track);

    MIX_DestroyAudio(tap_audio);
    MIX_DestroyAudio(capture_audio);

    MIX_DestroyMixer(mixer);
    MIX_Quit();
}
