#include "sound.hpp"
#include <SDL3_mixer/SDL_mixer.h>
#include <filesystem>
#include <iostream>

MIX_Mixer* GoSound::mixer = nullptr;

MIX_Audio* GoSound::tap_audio = nullptr;
MIX_Audio* GoSound::capture_audio = nullptr;

MIX_Track* GoSound::tap_track = nullptr;
MIX_Track* GoSound::capture_track = nullptr;

int GoSound::current_audio = 0;
std::vector<MIX_Audio*> GoSound::bg_audio_files = {};
MIX_Track* GoSound::bg_track = nullptr;

bool GoSound::play_music = true;

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

bool GoSound::loadMusicFiles () {
    namespace fs = std::filesystem;
    std::string music_path = "assets/music";
    if (!fs::exists(music_path) || !fs::is_directory(music_path)) {
        std::cerr << "Error: Directory '" << music_path << "' does not exist or is not a directory." << std::endl;
        return false;
    }

    for (const auto& entry : fs::directory_iterator(music_path)) {
        if (fs::is_regular_file(entry.status())) {
            std::string file_name = entry.path().filename().string();
            std::string file_path = music_path + "/" + file_name;

            MIX_Audio* audio = MIX_LoadAudio(nullptr, file_path.c_str(), false);
            if (!audio) {
                SDL_Log("Failed to load music file: %s", file_name.c_str());
                continue;
            }

            SDL_Log("Loaded file: %s", file_name.c_str());
            bg_audio_files.push_back(audio);
        }
    }

    bg_track = MIX_CreateTrack(mixer);
    MIX_SetTrackGain(bg_track, 0.3f);

    if (!bg_audio_files.empty()) {
        MIX_SetTrackStoppedCallback(bg_track, GoSound::playNextMusic, nullptr);


        MIX_SetTrackAudio(bg_track, bg_audio_files[current_audio]);
        MIX_PlayTrack(bg_track, 0);
    }
    return true;
}

void GoSound::playTap () {
    MIX_PlayTrack(tap_track, 0);
}

void GoSound::playCapture () {
    MIX_PlayTrack(capture_track, 0);
}

void GoSound::playNextMusic (void* userdata, MIX_Track* track) {
    current_audio = (current_audio+1)%bg_audio_files.size();

    SDL_Log("Playing music: %d", current_audio);
    MIX_SetTrackAudio(track, bg_audio_files[current_audio]);
    MIX_PlayTrack(track, 0);
}

bool GoSound::toggleMusic () {
    play_music = !play_music;
    MIX_SetTrackGain(bg_track, play_music ? 0.3f : 0.0f);
    return play_music;
}

void GoSound::destroy () {
    MIX_DestroyTrack(bg_track);
    for (int i = 0; i < bg_audio_files.size(); i++) {
        MIX_DestroyAudio(bg_audio_files[i]);
    }

    MIX_DestroyTrack(tap_track);
    MIX_DestroyTrack(capture_track);

    MIX_DestroyAudio(tap_audio);
    MIX_DestroyAudio(capture_audio);

    MIX_DestroyMixer(mixer);
    MIX_Quit();
}
