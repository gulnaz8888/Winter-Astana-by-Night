
#define MINIAUDIO_IMPLEMENTATION
#include "MusicPlayer.h"

#include <iostream>

ChristmasMusicPlayer::ChristmasMusicPlayer(const char* path) {
    if (ma_engine_init(nullptr, &engine) != MA_SUCCESS) {
        std::cerr << "Audio: could not initialize engine\n";
        return;
    }

    engineReady = true;

    ma_uint32 flags =
        MA_SOUND_FLAG_STREAM |
        MA_SOUND_FLAG_NO_SPATIALIZATION;

    if (ma_sound_init_from_file(
            &engine, path, flags,
            nullptr, nullptr, &sound
        ) != MA_SUCCESS) {
        std::cerr << "Audio: could not open " << path << "\n";
        return;
    }

    soundReady = true;
    ma_sound_set_looping(&sound, MA_TRUE);
    ma_sound_set_volume(&sound, 0.35f);
}

ChristmasMusicPlayer::~ChristmasMusicPlayer() {
    if (soundReady) {
        ma_sound_uninit(&sound);
    }

    if (engineReady) {
        ma_engine_uninit(&engine);
    }
}

void ChristmasMusicPlayer::playFromStart() {
    if (!soundReady) return;

    ma_sound_stop(&sound);
    ma_sound_seek_to_pcm_frame(&sound, 0);
    ma_sound_start(&sound);
}

void ChristmasMusicPlayer::stopAndRewind() {
    if (!soundReady) return;

    ma_sound_stop(&sound);
    ma_sound_seek_to_pcm_frame(&sound, 0);
}
