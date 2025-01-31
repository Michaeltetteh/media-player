#include "audio_player.hpp"
#include <iostream>


AudioPlayer::AudioPlayer() : m_audioDevice(0) {}

AudioPlayer::~AudioPlayer() {
    stop();
}

bool AudioPlayer::init(int sampleRate, int channels) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_AudioSpec desiredSpec, obtainedSpec;
    desiredSpec.freq = sampleRate;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = channels;
    desiredSpec.samples = 1024;
    desiredSpec.callback = nullptr;
    desiredSpec.userdata = nullptr;

    m_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec, 0);
    if (m_audioDevice == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_PauseAudioDevice(m_audioDevice, 0);
    return true;
}

void AudioPlayer::play(const uint8_t* audioData, int dataSize) {
    SDL_QueueAudio(m_audioDevice, audioData, dataSize);
}

void AudioPlayer::stop() {
    if (m_audioDevice) {
        SDL_CloseAudioDevice(m_audioDevice);
        m_audioDevice = 0;
    }
    SDL_Quit();
}