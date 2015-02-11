//
//  ImageDisplayViewController.h
//  AJEG
//
//  Created by Sebastian Mach on 05.02.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AJEG_Core.h"



typedef NS_OPTIONS(NSUInteger, AJ_Channel)
{
    AJ_Channel_Red       = 1 << 0,
    AJ_Channel_Green     = 1 << 1,
    AJ_Channel_Blue      = 1 << 2,
    
    AJ_Channel_Luminance = 1 << 3,
    AJ_Channel_ColorBlue = 1 << 4,
    AJ_Channel_ColorRed  = 1 << 5
};


typedef NS_ENUM(NSUInteger, AJ_DrawType)
{
    AJ_DrawType_RGBImage   = 1 << 0,
    AJ_DrawType_BlockImage = 1 << 1,
    AJ_DrawType_ErrorImage = 1 << 2
};



@interface ImageDisplayViewController : NSViewController

- (void)setImage:(aj_image *)image;
- (void)setChannel:(AJ_Channel)channel;
- (void)setDrawType:(AJ_DrawType)drawType;

@end
