//
//  ViewController.m
//  FFmpeg_iOS
//
//  Created by sen on 2018/12/15.
//  Copyright © 2018年 sen. All rights reserved.
//

#import "ViewController.h"
#import "ffmpeg.h"
#import "ReadAudio.h"

@interface ViewController ()

@end

@implementation ViewController
- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docsdir = [paths objectAtIndex:0];
    NSString *dataFilePath = [docsdir stringByAppendingPathComponent:@"av_base"];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL isDir = NO;
    
    // fileExistsAtPath 判断一个文件或目录是否有效，isDirectory判断是否一个目录
    BOOL existed = [fileManager fileExistsAtPath:dataFilePath isDirectory:&isDir];
    
    if (!(isDir && existed)) {
        // 在Document目录下创建一个archiver目录
        [fileManager createDirectoryAtPath:dataFilePath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    // 在archiver下写入文件
    NSString *path = [dataFilePath stringByAppendingPathComponent:@"audio.pcm"];
    [fileManager removeItemAtPath:path error:nil];
    NSLog(@"path == %@", path);
    char *cPath = (char *)[path cStringUsingEncoding:NSUTF8StringEncoding];
    ffmpegTestReadAudio(cPath);


}


/**
 第一种调用方式
 */
- (void)normalRun{
    NSString *fromFile = [[NSBundle mainBundle]pathForResource:@"video.mov" ofType:nil];
    NSString *toFile = @"/Users/sen/Desktop/Output/video.gif";
    
    int argc = 4;
    char **arguments = calloc(argc, sizeof(char*));
    if(arguments != NULL)
    {
        arguments[0] = "ffmpeg";
        arguments[1] = "-i";
        arguments[2] = (char *)[fromFile UTF8String];
        arguments[3] = (char *)[toFile UTF8String];
        
        if (!ffmpeg_main(argc, arguments)) {
            NSLog(@"生成成功");
        }
    }
}

- (void)normalRun2{
    
    NSString *fromFile = [[NSBundle mainBundle]pathForResource:@"video.mov" ofType:nil];
    NSString *toFile   = @"/Users/sen/Desktop/Output/video.gif";
    
    NSString *command_str = [NSString stringWithFormat:@"ffmpeg -i %@ %@",fromFile,toFile];
    
    // 分割字符串
    NSMutableArray  *argv_array  = [command_str componentsSeparatedByString:(@" ")].mutableCopy;
    
    // 获取参数个数
    int argc = (int)argv_array.count;
    
    // 遍历拼接参数
    char **argv = calloc(argc, sizeof(char*));
    
    for(int i=0; i<argc; i++)
    {
        NSString *codeStr = argv_array[i];
        argv_array[i]     = codeStr;
        argv[i]      = (char *)[codeStr UTF8String];
    }
    
    ffmpeg_main(argc, argv);
}
@end
