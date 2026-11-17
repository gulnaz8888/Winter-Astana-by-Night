#pragma once

#include "miniaudio.h"

class ChristmasMusicPlayer {
public:
    explicit ChristmasMusicPlayer(const char* path);
    ~ChristmasMusicPlayer();

    void playFromStart();
    void stopAndRewind();

    // LRT announcements
    void playWelcome();
    void playStation(int sceneIndex);
    void update();

    ChristmasMusicPlayer(const ChristmasMusicPlayer&) = delete;
    ChristmasMusicPlayer& operator=(const ChristmasMusicPlayer&) = delete;

private:
    ma_engine engine{};
    ma_sound sound{};

    ma_sound welcomeSound{};
    ma_sound baiterekSound{};
    ma_sound expoSound{};
    ma_sound ishimSound{};

    bool engineReady = false;
    bool soundReady = false;

    bool welcomeReady = false;
    bool baiterekReady = false;
    bool expoReady = false;
    bool ishimReady = false;

    bool musicPlaying = false;
    bool announcementPlaying = false;

    ma_sound* currentAnnouncement = nullptr;

    void stopAnnouncement();
    void playAnnouncement(ma_sound* announcement);
};