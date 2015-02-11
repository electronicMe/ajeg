//
//  ImageDisplayViewController.m
//  AJEG
//
//  Created by Sebastian Mach on 05.02.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#import "ImageDisplayViewController.h"
#import "ImageDisplayView.h"

@interface ImageDisplayViewController ()

@end



@implementation ImageDisplayViewController



- (void)viewDidLoad
{
    [super viewDidLoad];
    NSLog(@"InputDataViewController did load");
}


- (void)setImage:(aj_image *)image
{
    ImageDisplayView *inputDataView = (ImageDisplayView*)[self view];
    [inputDataView setImage:image];
}


- (void)setChannel:(AJ_Channel)channel;
{
    ImageDisplayView *inputDataView = (ImageDisplayView*)[self view];
    [inputDataView setChannel:channel];
}


- (void)setDrawType:(AJ_DrawType)drawType
{
    ImageDisplayView *inputDataView = (ImageDisplayView*)[self view];
    [inputDataView setDrawType:drawType];
}

@end
