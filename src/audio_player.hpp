#pragma once

#include <SDL.h>
#include <libavcodec/avcodec.h>

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();

    bool init(int sampleRate, int channels);
    void play(const uint8_t* audioData, int dataSize);
    void stop();

private:
    SDL_AudioDeviceID m_audioDevice;
};
