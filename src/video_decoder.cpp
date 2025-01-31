#include "video_decoder.hpp"
#include <iostream>


MPDecoder::MPDecoder()
    : m_formatContext(nullptr), m_videoCodecContext(nullptr), m_audioCodecContext(nullptr),
      m_videoFrame(nullptr), m_audioFrame(nullptr), m_packet(nullptr),
      m_videoStreamIndex(-1), m_audioStreamIndex(-1),
      m_swsContext(nullptr), m_swrContext(nullptr),
      m_rgbFrame(nullptr), m_videoBuffer(nullptr), m_audioBuffer(nullptr) {
}

MPDecoder::~MPDecoder() {
    close();
}

bool MPDecoder::open(const std::string& filePath) {
    // Open the input file
    if (avformat_open_input(&m_formatContext, filePath.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Couldn't open file." << std::endl;
        return false;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) {
        std::cerr << "Couldn't find stream information." << std::endl;
        return false;
    }

    // Find video and audio streams
    for (int i = 0; i < m_formatContext->nb_streams; i++) {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && m_videoStreamIndex == -1) {
            m_videoStreamIndex = i;
        } else if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && m_audioStreamIndex == -1) {
            m_audioStreamIndex = i;
        }
    }

    if (m_videoStreamIndex == -1 && m_audioStreamIndex == -1) {
        std::cerr << "Couldn't find a video or audio stream." << std::endl;
        return false;
    }

    // Initialize video codec context
    if (m_videoStreamIndex != -1) {
        AVCodecParameters* videoCodecParameters = m_formatContext->streams[m_videoStreamIndex]->codecpar;
        const AVCodec* videoCodec = avcodec_find_decoder(videoCodecParameters->codec_id);
        if (!videoCodec) {
            std::cerr << "Unsupported video codec." << std::endl;
            return false;
        }

        m_videoCodecContext = avcodec_alloc_context3(videoCodec);
        avcodec_parameters_to_context(m_videoCodecContext, videoCodecParameters);

        if (avcodec_open2(m_videoCodecContext, videoCodec, nullptr) < 0) {
            std::cerr << "Couldn't open video codec." << std::endl;
            return false;
        }

        m_videoFrame = av_frame_alloc();
        initSWSContext();
    }

    // Initialize audio codec context
    if (m_audioStreamIndex != -1) {
        AVCodecParameters* audioCodecParameters = m_formatContext->streams[m_audioStreamIndex]->codecpar;
        const AVCodec* audioCodec = avcodec_find_decoder(audioCodecParameters->codec_id);
        if (!audioCodec) {
            std::cerr << "Unsupported audio codec." << std::endl;
            return false;
        }

        m_audioCodecContext = avcodec_alloc_context3(audioCodec);
        avcodec_parameters_to_context(m_audioCodecContext, audioCodecParameters);

        if (avcodec_open2(m_audioCodecContext, audioCodec, nullptr) < 0) {
            std::cerr << "Couldn't open audio codec." << std::endl;
            return false;
        }

        m_audioFrame = av_frame_alloc();
        initSWRContext();
    }

    m_packet = av_packet_alloc();
    return true;
}

void MPDecoder::close() {
    if (m_swsContext) sws_freeContext(m_swsContext);
    if (m_swrContext) swr_free(&m_swrContext);
    if (m_rgbFrame) av_frame_free(&m_rgbFrame);
    if (m_videoBuffer) av_free(m_videoBuffer);
    if (m_audioBuffer) av_free(m_audioBuffer);
    if (m_videoFrame) av_frame_free(&m_videoFrame);
    if (m_audioFrame) av_frame_free(&m_audioFrame);
    if (m_videoCodecContext) avcodec_free_context(&m_videoCodecContext);
    if (m_audioCodecContext) avcodec_free_context(&m_audioCodecContext);
    if (m_formatContext) avformat_close_input(&m_formatContext);
    if (m_packet) av_packet_free(&m_packet);
}

bool MPDecoder::decodeFrame() {
    while (av_read_frame(m_formatContext, m_packet) >= 0) {
        if (m_packet->stream_index == m_videoStreamIndex) {
            if (avcodec_send_packet(m_videoCodecContext, m_packet) == 0) {
                if (avcodec_receive_frame(m_videoCodecContext, m_videoFrame) == 0) {
                    sws_scale(m_swsContext, m_videoFrame->data, m_videoFrame->linesize, 0, m_videoCodecContext->height, m_rgbFrame->data, m_rgbFrame->linesize);
                    av_packet_unref(m_packet);
                    return true;
                }
            }
        } else if (m_packet->stream_index == m_audioStreamIndex) {
            if (avcodec_send_packet(m_audioCodecContext, m_packet) == 0) {
                if (avcodec_receive_frame(m_audioCodecContext, m_audioFrame) == 0) {
                    av_packet_unref(m_packet);
                    return true;
                }
            }
        }
        av_packet_unref(m_packet);
    }
    return false;
}

AVFrame* MPDecoder::getVideoFrame() const {
    return m_rgbFrame;
}

AVFrame* MPDecoder::getAudioFrame() const {
    return m_audioFrame;
}

int MPDecoder::getVideoWidth() const {
    return m_videoCodecContext->width;
}

int MPDecoder::getVideoHeight() const {
    return m_videoCodecContext->height;
}

int MPDecoder::getAudioSampleRate() const {
    return m_audioCodecContext->sample_rate;
}

int MPDecoder::getAudioChannels() const {
    return m_audioCodecContext->ch_layout.nb_channels;
}

AVSampleFormat MPDecoder::getAudioFormat() const {
    return m_audioCodecContext->sample_fmt;
}

void MPDecoder::initSWSContext() {
    m_swsContext = sws_getContext(
        m_videoCodecContext->width, m_videoCodecContext->height, m_videoCodecContext->pix_fmt,
        m_videoCodecContext->width, m_videoCodecContext->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    m_rgbFrame = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_videoCodecContext->width, m_videoCodecContext->height, 1);
    m_videoBuffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(m_rgbFrame->data, m_rgbFrame->linesize, m_videoBuffer, AV_PIX_FMT_RGB24, m_videoCodecContext->width, m_videoCodecContext->height, 1);
}

void MPDecoder::initSWRContext() {
    m_swrContext = swr_alloc();
    av_opt_set_chlayout(m_swrContext, "in_chlayout", &m_audioCodecContext->ch_layout, 0);
    av_opt_set_int(m_swrContext, "in_sample_rate", m_audioCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(m_swrContext, "in_sample_fmt", m_audioCodecContext->sample_fmt, 0);

    AVChannelLayout out_chlayout = AV_CHANNEL_LAYOUT_STEREO; // Example: Stereo output
    av_opt_set_chlayout(m_swrContext, "out_chlayout", &out_chlayout, 0);
    av_opt_set_int(m_swrContext, "out_sample_rate", m_audioCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(m_swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

    swr_init(m_swrContext);

    m_audioBuffer = (uint8_t*)av_malloc(av_samples_get_buffer_size(nullptr, out_chlayout.nb_channels, m_audioCodecContext->frame_size, AV_SAMPLE_FMT_S16, 1));
}