#ifndef GO_SOUND_H
#define GO_SOUND_H

#include <SDL3_mixer/SDL_mixer.h>

class GoSound {
    static MIX_Mixer* mixer;

    static MIX_Audio* tap_audio;
    static MIX_Audio* capture_audio;

    static MIX_Track* tap_track;
    static MIX_Track* capture_track;

public:
    static bool init ();
    static void playTap ();
    static void playCapture ();
    static void destroy ();

    // Prevent instantiation
    GoSound() = delete;
    GoSound(const GoSound&) = delete;
    GoSound& operator=(const GoSound&) = delete;
};

#endif
