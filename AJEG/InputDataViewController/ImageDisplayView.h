//
//  ImageDisplayView.h
//  AJEG
//
//  Created by Sebastian Mach on 05.02.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AJEG_Core.h"
#import "ImageDisplayViewController.h"

@interface ImageDisplayView : NSView

@property aj_image *image;
@property AJ_Channel channel;
@property AJ_DrawType drawType;

@end
