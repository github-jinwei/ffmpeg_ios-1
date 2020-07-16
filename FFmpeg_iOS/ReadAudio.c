//
//  ReadAudio.c
//  ffmpeg_mac
//
//  Created by konglee on 2020/7/14.
//  Copyright © 2020 konglee. All rights reserved.
//

#include "ReadAudio.h"

static char *const get_error_text(const int error) {
    static char error_buffer[255];
    av_strerror(error, error_buffer, sizeof(error_buffer));
    return error_buffer;
}

void resample() {
    /// 重采样的上下文
    SwrContext *swr_ctx = NULL;
    swr_ctx = swr_alloc_set_opts(NULL,/*如果已初始化好上下文传给他即可，如果没有传 NULL*/
                       AV_CH_LAYOUT_STEREO,/*输出布局参数，AV_CH_LAYOUT_STEREO 左前方，右前方，相对于就是一个立体声了*/
                       AV_SAMPLE_FMT_S16,// 输出采样格式
                       44100,// 采样率
                       AV_CH_LAYOUT_STEREO, // 输入 channel 布局
                       AV_SAMPLE_FMT_FLT, // 输入采样格式
                       44100, // 输出采样率
                       0,
                       NULL);
    if (!swr_ctx) {
        return;
    }
    
    if (swr_init(swr_ctx) < 0) {
        return;
    }
    
    
    return;
}

void ffmpegTestReadAudio(char *path) {
    
    AVFormatContext *fmt_ctx = NULL;
    
    AVDictionary *options = NULL;
    
    /// [[ video device] : [audio device]]
    char *devicename = ":0";
    
    // 1. register audio device
    avdevice_register_all();
    
    // 2. get format
    AVInputFormat *iFormate = av_find_input_format("avfoundation");
    int ret = avformat_open_input(&fmt_ctx, devicename, iFormate, &options);
    if (ret < 0) {
        // error
        get_error_text(ret);
        return;
    }

    AVPacket pkt;
    int count = 0;
    
    /// 重采样的上下文
    SwrContext *swr_ctx = NULL;
    swr_ctx = swr_alloc_set_opts(NULL,/*如果已初始化好上下文传给他即可，如果没有传 NULL*/
                       AV_CH_LAYOUT_STEREO,/*输出布局参数，AV_CH_LAYOUT_STEREO 左前方，右前方，相对于就是一个立体声了*/
                       AV_SAMPLE_FMT_S16,// 输出采样格式
                       44100,// 采样率
                       AV_CH_LAYOUT_STEREO, // 输入 channel 布局
                       AV_SAMPLE_FMT_FLT, // 输入采样格式
                       44100, // 输出采样率
                       0,
                       NULL);
    if (!swr_ctx) {
        return;
    }
    
    if (swr_init(swr_ctx) < 0) {
        return;
    }
    uint8_t **src_data = NULL;
    int line_size = 0;
    // why 512 4096/ 4 = 1024, 1024 /2  = 512，512 是单通道的个数
    av_samples_alloc_array_and_samples(&src_data,// 输出缓冲区大小
                                       &line_size,// 缓冲区大小
                                       2,// 通道个数
                                       256,// 单通道采样个数
                                       AV_SAMPLE_FMT_FLT,// 采样格式
                                       0);
    
    uint8_t **dst_data = NULL;
    int dst_lineSize = 0;
    
    av_samples_alloc_array_and_samples(&dst_data, &dst_lineSize, 2, 256, AV_SAMPLE_FMT_S16, 0);
    
    
    // creat file
    // wb+ w = write b = binary + = add
    FILE *outfile = fopen(path, "wb+");
    
    // 打开编码器
    //avcodec_find_encoder(AV_CODEC_ID_AAC);
    AVCodec *codec = avcodec_find_encoder_by_name("libfdk_aac");
    
    // 创建编码器上下文
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    
    /// 设置数据采样格式
    codec_ctx -> sample_fmt = AV_SAMPLE_FMT_S16;
    codec_ctx -> channel_layout = AV_CH_LAYOUT_STEREO;
    codec_ctx -> sample_rate = 44100;
    /// 输出码率
    codec_ctx -> bit_rate = 0;
    
    /// aac_lc 128K aac_he 64k aac_hev2 = 32k
    codec_ctx -> profile = FF_PROFILE_AAC_HE_V2;
    /// 如果已设置 profile bit_rate 设为 0
    
    /// 分配 AVFrame
    AVFrame *frame = av_frame_alloc();
    frame -> nb_samples = 256; // 单通道一个音频帧的采样数
    frame -> format = AV_SAMPLE_FMT_S16; // 每个采样大小
    frame -> channel_layout = AV_CH_LAYOUT_STEREO; // channel layout
    av_frame_get_buffer(frame, 0); // 256 * 2 * 16 = 1024Byte;
    if (!frame || frame -> buf[0]) {
        return;
    }
    
    AVPacket *encodePacket = av_packet_alloc();
    if (!encodePacket) {
        return;
    }
    
    
    ret = avcodec_open2(codec_ctx, codec, NULL);
    if (ret < 0) {
        get_error_text(ret);
        return;
    }
    
    while (( ret = av_read_frame(fmt_ctx, &pkt) == 0) && count++ < 500) {
        printf("pkt size %d", pkt.size);
        
        // 把 pkt 的数据 copy 到输入中
        memcpy((void *)src_data[0], pkt.data, pkt.size);
        
        // 开始重采样
        swr_convert(swr_ctx, /// 重采样上下文
                    dst_data, /// 输出结果缓冲区
                    256, /// 每个通道的采样数
                    (const uint8_t **)src_data,
                    256);
        
        fwrite(dst_data[0], 1, dst_lineSize, outfile);
        av_packet_unref(&pkt);
        
    }
    
    /// 释放重采样缓冲区
    if (src_data) {
        av_freep(&src_data[0]);
    }
    av_freep(src_data);
    
    if (dst_data) {
        av_freep(&dst_data[0]);
    }
    av_freep(&dst_data);
    
    /// 释放重采样的上下文
    swr_free(&swr_ctx);
    
    if (ret < 0) {
        get_error_text(ret);
    }
    
    avformat_close_input(&fmt_ctx);
    fclose(outfile);
    av_log_set_level(AV_LOG_DEBUG);
    av_log(NULL, AV_LOG_DEBUG, __func__);
    av_log(NULL, AV_LOG_DEBUG, "\n");
}
