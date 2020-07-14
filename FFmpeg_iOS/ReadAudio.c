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
    
    // creat file
    // wb+ w = write b = binary + = add
    FILE *outfile = fopen(path, "wb+");
    
    while (( ret = av_read_frame(fmt_ctx, &pkt) == 0) && count++ < 500) {
        printf("pkt size %d", pkt.size);
        fwrite(pkt.data, pkt.size, 1, outfile);
        av_packet_unref(&pkt);
    }
    if (ret < 0) {
        get_error_text(ret);
    }
    
    avformat_close_input(&fmt_ctx);
    fclose(outfile);
    av_log_set_level(AV_LOG_DEBUG);
    av_log(NULL, AV_LOG_DEBUG, __func__);
    av_log(NULL, AV_LOG_DEBUG, "\n");
}
