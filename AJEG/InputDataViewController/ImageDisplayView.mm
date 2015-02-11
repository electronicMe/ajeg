//
//  ImageDisplayView.m
//  AJEG
//
//  Created by Sebastian Mach on 05.02.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#import "ImageDisplayView.h"

@interface ImageDisplayView ()
{
    
}

- (void)drawRGBImage:(aj_image*)image channel:(AJ_Channel)channel dirtyRect:(NSRect)dirtyRect;
- (void)drawYCbCrImage:(aj_image*)image channel:(AJ_Channel)channel dirtyRect:(NSRect)dirtyRect;
- (void)drawErrorImage:(aj_image*)rgbImage channel:(AJ_Channel)channel yCbCrImage:(aj_image*)yCbCrImage dirtyRect:(NSRect)dirtyRect;

- (void)drawRGBPixel:(NSRect)dirtyRect x:(int)x y:(int)y red:(CGFloat)red green:(CGFloat)green blue:(CGFloat)blue alpha:(CGFloat)alpha;

@end



@implementation ImageDisplayView

@synthesize image    = _image;
@synthesize channel  = _channel;
@synthesize drawType = _drawType;



- (void)drawRect:(NSRect)dirtyRect
{
    NSLog(@"ImageDisplayView drawRect start");
    
    
    
    [[NSColor blackColor] setFill];
    NSRectFill(dirtyRect);
    
    
    
    
    if (_image)
    {
        if (_drawType == AJ_DrawType_RGBImage)
            [self drawRGBImage:_image channel:_channel dirtyRect:dirtyRect];
        else if (_drawType == AJ_DrawType_BlockImage)
            [self drawYCbCrImage:_image channel:_channel dirtyRect:dirtyRect];
        else if (_drawType == AJ_DrawType_ErrorImage)
        {
            
        }
    }
    
    
    
    [super drawRect:dirtyRect];
    NSLog(@"ImageDisplayView drawRect finished");
}


- (void)drawRGBImage:(aj_image*)image channel:(AJ_Channel)channel dirtyRect:(NSRect)dirtyRect
{
    for (int y = 0; y < _image->height; y++)
    {
        for (int x = 0; x < _image->width; x++)
        {
            aj_rgb_Pixel *pixel = getPixelInImage(_image, x, y);
            
            CGFloat red   = ((channel & AJ_Channel_Red)   ? (pixel->red   / 255.0) : 0.0);
            CGFloat green = ((channel & AJ_Channel_Green) ? (pixel->green / 255.0) : 0.0);
            CGFloat blue  = ((channel & AJ_Channel_Blue)  ? (pixel->blue  / 255.0) : 0.0);
            
            
            [self drawRGBPixel:dirtyRect
                             x:x
                             y:y
                           red:red
                         green:green
                          blue:blue
                         alpha:1];
        }
    }
}


- (void)drawYCbCrImage:(aj_image*)image channel:(AJ_Channel)channel dirtyRect:(NSRect)dirtyRect
{
    
}


- (void)drawErrorImage:(aj_image*)rgbImage channel:(AJ_Channel)channel yCbCrImage:(aj_image*)yCbCrImage dirtyRect:(NSRect)dirtyRect;
{
    
}


- (void)drawRGBPixel:(NSRect)dirtyRect x:(int)x y:(int)y red:(CGFloat)red green:(CGFloat)green blue:(CGFloat)blue alpha:(CGFloat)alpha
{
    NSRect pixelRect = NSMakeRect(x, dirtyRect.size.height - y, 1, 1);
    [[NSColor colorWithRed:red green:green blue:blue alpha:alpha] set];
    NSRectFill(pixelRect);
}


@end

