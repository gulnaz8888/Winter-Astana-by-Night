
#pragma once

#include "miniaudio.h"

class ChristmasMusicPlayer {
public:
    explicit ChristmasMusicPlayer(const char* path);
    ~ChristmasMusicPlayer();

    void playFromStart();
    void stopAndRewind();

    ChristmasMusicPlayer(const ChristmasMusicPlayer&) = delete;
    ChristmasMusicPlayer& operator=(const ChristmasMusicPlayer&) = delete;

private:
    ma_engine engine{};
    ma_sound sound{};

    bool engineReady = false;
    bool soundReady = false;
};
