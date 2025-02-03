#pragma once

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/opt.h>
}
#include <string>


class MPDecoder {
    
public:
    MPDecoder();
    ~MPDecoder();
    MPDecoder (const MPDecoder &) =delete;
    MPDecoder (MPDecoder &&) =delete;
    MPDecoder& operator=(const MPDecoder &) =delete;

    bool open(const std::string& filePath);
    void close();
    bool decodeFrame();
    AVFrame* getVideoFrame() const;
    AVFrame* getAudioFrame() const;
    int getVideoWidth() const;
    int getVideoHeight() const;
    int getAudioSampleRate() const;
    int getAudioChannels() const;
    AVSampleFormat getAudioFormat() const;
    AVRational getFrameRate() const;

private:
    AVFormatContext* m_formatContext;
    AVCodecContext* m_videoCodecContext;
    AVCodecContext* m_audioCodecContext;
    AVFrame* m_videoFrame;
    AVFrame* m_audioFrame;
    AVPacket* m_packet;
    int m_videoStreamIndex;
    int m_audioStreamIndex;
    SwsContext* m_swsContext;
    SwrContext* m_swrContext;
    AVFrame* m_rgbFrame;
    uint8_t* m_videoBuffer;
    uint8_t* m_audioBuffer;

    void initSWSContext();
    void initSWRContext();
};

