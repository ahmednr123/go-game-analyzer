#ifndef GO_SOUND_H
#define GO_SOUND_H

#include <SDL3_mixer/SDL_mixer.h>
#include <vector>

class GoSound {
    static MIX_Mixer* mixer;

    static MIX_Audio* tap_audio;
    static MIX_Audio* capture_audio;

    static MIX_Track* tap_track;
    static MIX_Track* capture_track;

    static int current_audio;
    static std::vector<MIX_Audio*> bg_audio_files;
    static MIX_Track* bg_track;

public:
    static bool init ();
    static bool loadMusicFiles ();

    static void playTap ();
    static void playCapture ();
    static void playNextMusic (void* userdata, MIX_Track* track);

    static void destroy ();

    // Prevent instantiation
    GoSound() = delete;
    GoSound(const GoSound&) = delete;
    GoSound& operator=(const GoSound&) = delete;
};

#endif
