//
//  ViewController.m
//  AJEG
//
//  Created by Sebastian Mach on 14.01.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#import "ViewController.h"
#import "ImageDisplayViewController.h"
#import "AJEG_Core.h"



@interface ViewController ()
{
    IBOutlet NSTextField *inputFileTextField;
    IBOutlet NSTextField *outputTextField;
    
    IBOutlet NSButton *rgbImage_red;
    IBOutlet NSButton *rgbImage_green;
    IBOutlet NSButton *rgbImage_blue;
    
    IBOutlet NSButton *yCbCrImage_y;
    IBOutlet NSButton *yCbCrImage_Cb;
    IBOutlet NSButton *yCbCrImage_Cr;
    
    IBOutlet NSButton *errorImage_red;
    IBOutlet NSButton *errorImage_green;
    IBOutlet NSButton *errorImage_blue;
    
    IBOutlet NSSlider *qualitySlider;
    
    AJEG_Core *_ajegCore;
    aj_image  *_image;
}

- (IBAction)loadInputImage:(id)sender;
- (IBAction)convertImage:(id)sender;

@end






@implementation ViewController


- (IBAction)loadInputImage:(id)sender
{
    if (_ajegCore)
        free(_ajegCore);
    if (_image)
        free(_image);
    
    std::string inputFilePath ([[inputFileTextField stringValue] UTF8String]);
    std::string outputFilePath([[outputTextField    stringValue] UTF8String]);
    
    _ajegCore = new AJEG_Core(inputFilePath, outputFilePath);
    
    NSLog(@"loadRGBImage start");
    NSDate *startDate = [NSDate date];
    _image    = _ajegCore->loadRGBImage();
    NSDate *endDate = [NSDate date];
    NSLog(@"loadRGBImage stop");
    
    NSTimeInterval duration = [endDate timeIntervalSinceDate:startDate];
    
    NSLog(@"Load Duration: %f seconds, %f milliseconds, %f microseconds", duration, duration*100, duration*100);
}


- (IBAction)convertImage:(id)sender
{
    if (!_image || !_ajegCore)
    {
        NSLog(@"Load image before converting it into jpg");
        return;
    }
    
    NSLog(@"encode start");
    NSDate *startDate = [NSDate date];
    _ajegCore->encode(_image, [qualitySlider intValue]);
    NSDate *endDate = [NSDate date];
    NSLog(@"encode stop");
    
    NSTimeInterval duration = [endDate timeIntervalSinceDate:startDate];
    
    NSLog(@"Encode Duration: %f seconds, %f milliseconds, %f microseconds", duration, duration*100, duration*100);
}






- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender
{
    if ([[segue identifier] isEqualToString:@"drawRGBImage"])
    {
        NSWindowController *windowController = [segue destinationController];
        ImageDisplayViewController *inputDataViewController = [[windowController window] contentView];
        
        [inputDataViewController setImage:_image];
        [inputDataViewController setDrawType:AJ_DrawType_RGBImage];
        
        AJ_Channel channel = 0;
        
        if ([rgbImage_red   state])
            channel |= AJ_Channel_Red;
        if ([rgbImage_green state])
            channel |= AJ_Channel_Green;
        if ([rgbImage_blue  state])
            channel |= AJ_Channel_Blue;
        
        
        [inputDataViewController setChannel:channel];
    }
    else if ([[segue identifier] isEqualToString:@"drawYCbCrImage"])
    {
        NSWindowController *windowController = [segue destinationController];
        ImageDisplayViewController *inputDataViewController = [[windowController window] contentView];
        
        [inputDataViewController setImage:_image];
        [inputDataViewController setDrawType:AJ_DrawType_BlockImage];
        
        AJ_Channel channel = ( ([yCbCrImage_y  state] << AJ_Channel_Luminance) |
                               ([yCbCrImage_Cb state] << AJ_Channel_ColorBlue) |
                               ([yCbCrImage_Cr state] << AJ_Channel_ColorRed)    );
        
        [inputDataViewController setChannel:channel];
    }
    else if ([[segue identifier] isEqualToString:@"drawErrorImage"])
    {
        NSWindowController *windowController = [segue destinationController];
        ImageDisplayViewController *inputDataViewController = [[windowController window] contentView];
        
        [inputDataViewController setImage:_image];
        [inputDataViewController setDrawType:AJ_DrawType_ErrorImage];
        
        AJ_Channel channel = ( ([errorImage_red   state] << AJ_Channel_Red)   |
                               ([errorImage_green state] << AJ_Channel_Green) |
                               ([errorImage_blue  state] << AJ_Channel_Blue)    );
        
        [inputDataViewController setChannel:channel];
    }
}

@end
