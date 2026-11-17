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

    // Background Christmas music
    if (ma_sound_init_from_file(
            &engine,
            path,
            flags,
            nullptr,
            nullptr,
            &sound
        ) == MA_SUCCESS) {

        soundReady = true;

        ma_sound_set_looping(&sound, MA_TRUE);
        ma_sound_set_volume(&sound, 0.35f);

    } else {
        std::cerr << "Audio: could not open " << path << "\n";
    }

    // Welcome announcement
    if (ma_sound_init_from_file(
            &engine,
            "assets/audio/welcome.mp3",
            flags,
            nullptr,
            nullptr,
            &welcomeSound
        ) == MA_SUCCESS) {

        welcomeReady = true;
        ma_sound_set_volume(&welcomeSound, 1.0f);

    } else {
        std::cerr << "Audio: welcome.mp3 not found\n";
    }

    // Baiterek station
    if (ma_sound_init_from_file(
            &engine,
            "assets/audio/Baiterek_station.mp3",
            flags,
            nullptr,
            nullptr,
            &baiterekSound
        ) == MA_SUCCESS) {

        baiterekReady = true;
        ma_sound_set_volume(&baiterekSound, 1.0f);

    } else {
        std::cerr << "Audio: Baiterek_station.mp3 not found\n";
    }

    // EXPO station
    if (ma_sound_init_from_file(
            &engine,
            "assets/audio/Expo_station.mp3",
            flags,
            nullptr,
            nullptr,
            &expoSound
        ) == MA_SUCCESS) {

        expoReady = true;
        ma_sound_set_volume(&expoSound, 1.0f);

    } else {
        std::cerr << "Audio: Expo_station.mp3 not found\n";
    }

    // Ishim Riverfront station
    if (ma_sound_init_from_file(
            &engine,
            "assets/audio/Ishim_station.mp3",
            flags,
            nullptr,
            nullptr,
            &ishimSound
        ) == MA_SUCCESS) {

        ishimReady = true;
        ma_sound_set_volume(&ishimSound, 1.0f);

    } else {
        std::cerr << "Audio: Ishim_station.mp3 not found\n";
    }
}

ChristmasMusicPlayer::~ChristmasMusicPlayer() {

    stopAnnouncement();

    if (welcomeReady)
        ma_sound_uninit(&welcomeSound);

    if (baiterekReady)
        ma_sound_uninit(&baiterekSound);

    if (expoReady)
        ma_sound_uninit(&expoSound);

    if (ishimReady)
        ma_sound_uninit(&ishimSound);

    if (soundReady)
        ma_sound_uninit(&sound);

    if (engineReady)
        ma_engine_uninit(&engine);
}

void ChristmasMusicPlayer::playFromStart() {

    if (!soundReady)
        return;

    ma_sound_stop(&sound);
    ma_sound_seek_to_pcm_frame(&sound, 0);

    musicPlaying = true;

    if (announcementPlaying) {
        ma_sound_set_volume(&sound, 0.08f);
    } else {
        ma_sound_set_volume(&sound, 0.35f);
    }

    ma_sound_start(&sound);
}

void ChristmasMusicPlayer::stopAndRewind() {

    if (!soundReady)
        return;

    ma_sound_stop(&sound);
    ma_sound_seek_to_pcm_frame(&sound, 0);

    musicPlaying = false;
}

void ChristmasMusicPlayer::stopAnnouncement() {

    if (currentAnnouncement != nullptr) {
        ma_sound_stop(currentAnnouncement);
        ma_sound_seek_to_pcm_frame(currentAnnouncement, 0);
    }

    currentAnnouncement = nullptr;
    announcementPlaying = false;
}

void ChristmasMusicPlayer::playAnnouncement(
    ma_sound* announcement
) {

    if (!announcement)
        return;

    stopAnnouncement();

    currentAnnouncement = announcement;
    announcementPlaying = true;

    // Lower background music during announcements
    if (soundReady && musicPlaying) {
        ma_sound_set_volume(&sound, 0.08f);
    }

    ma_sound_seek_to_pcm_frame(announcement, 0);
    ma_sound_start(announcement);
}

void ChristmasMusicPlayer::playWelcome() {

    if (!welcomeReady)
        return;

    playAnnouncement(&welcomeSound);

    std::cout << "LRT: Welcome to Astana\n";
}

void ChristmasMusicPlayer::playStation(int sceneIndex) {

    switch (sceneIndex) {

        case 0:
            if (baiterekReady)
                playAnnouncement(&baiterekSound);

            std::cout << "LRT: Next station Baiterek\n";
            break;

        case 1:
            if (expoReady)
                playAnnouncement(&expoSound);

            std::cout << "LRT: Next station EXPO\n";
            break;

        case 2:
            if (ishimReady)
                playAnnouncement(&ishimSound);

            std::cout << "LRT: Next station Ishim Riverfront\n";
            break;

        default:
            break;
    }
}

void ChristmasMusicPlayer::update() {

    if (!announcementPlaying || !currentAnnouncement)
        return;

    if (ma_sound_at_end(currentAnnouncement)) {

        stopAnnouncement();

        // Restore background music volume
        if (soundReady && musicPlaying) {
            ma_sound_set_volume(&sound, 0.35f);
        }
    }
}