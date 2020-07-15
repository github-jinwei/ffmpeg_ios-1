//
//  ReadAudio.h
//  ffmpeg_mac
//
//  Created by konglee on 2020/7/14.
//  Copyright © 2020 konglee. All rights reserved.
//

#ifndef ReadAudio_h
#define ReadAudio_h

#include <stdio.h>
#include "libavutil/avutil.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"

void ffmpegTestReadAudio(char *path);

#endif /* ReadAudio_h */
