#include "video_decoder.hpp"
#include "renderer.hpp"
#include "audio_player.hpp"
#include <iostream>

int main() {
    MPDecoder decoder;
    Renderer renderer;
    AudioPlayer audioPlayer;

    if (!decoder.open("video.mp4")) {
        std::cerr << "Failed to open media file." << std::endl;
        return -1;
    }

    if (!renderer.init(decoder.getVideoWidth(), decoder.getVideoHeight())) {
        std::cerr << "Failed to initialize OpenGL renderer." << std::endl;
        return -1;
    }

    // if (decoder.getAudioChannels() > 0) {
    //     if (!audioPlayer.init(decoder.getAudioSampleRate(), decoder.getAudioChannels())) {
    //         std::cerr << "Failed to initialize audio player." << std::endl;
    //         return -1;
    //     }
    // }

    while (!glfwWindowShouldClose(renderer.getWindow())) {
        if (decoder.decodeFrame()) {
            if (decoder.getVideoFrame()) {
                renderer.renderFrame(decoder.getVideoFrame()->data[0], decoder.getVideoWidth(), decoder.getVideoHeight());
            }
            // if (decoder.getAudioFrame()) {
            //     audioPlayer.play(decoder.getAudioFrame()->data[0], decoder.getAudioFrame()->linesize[0]);
            // }
        }
    }

    return 0;
}