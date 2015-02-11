//
//  AJEG_Core.h
//  AJEG
//
//  Created by Sebastian Mach on 14.01.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#ifndef __AJEG__AJEG_Core__
#define __AJEG__AJEG_Core__

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>


static std::string AC_DEBUG   = "[ DEBUG ]";
static std::string AC_WARNING = "[WARNING]";
static std::string AC_ERROR   = "[ ERROR ]";



typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} aj_rgb_Pixel;


typedef struct {
    float luminance[64]; // = Y
    float colorBlue[64]; // = Cb
    float  colorRed[64]; // = Cr
} aj_block;


typedef enum {
    aj_imageType_unknown,
    aj_imageType_rgbImage,
    aj_imageType_blockImage,
    aj_imageType_dctImage
} aj_imageType;


typedef struct {
    
    // Dimension of the image
    int width;
    int height;
    
    // The type of this image
    aj_imageType imageType;
    
    // RGB Pixels
    // Number of rgb pixels = (width * height)
    // Is allocated if imageType = aj_imageType_rgbImage
    aj_rgb_Pixel *rgbPixels;
    
    // YCbCr Blocks
    // Number of blocks = (ceil(width/8) * ceil(height/8))
    // Is allocated if imageType = aj_imageType_blockImage or
    // aj_imageType_dctImage
    aj_block *blocks;
    
} aj_image;


typedef enum {
    P0, // = unknown value
    P1,
    P2,
    P3,
    P4,
    P5,
    P6
} portablePixmap_magicValue;


typedef struct {
    portablePixmap_magicValue magicalValue;
    int width;
    int height;
    int maxValue;
} portablePixmapHeader;


/*!
 *    Gets the pixel at the x and y position of the specified image.
 *    Returns NULL if the pixel is outside of the image.
 */
aj_rgb_Pixel *getPixelInImage(aj_image *image, int x, int y);



class AJEG_Core {
    
public:
    
    AJEG_Core(std::string inputFile, std::string outputFile);
    ~AJEG_Core();
    
    /*!
     *    Returns the rgb image in aj_image representation or NULL if something
     *    failed. Only Portable Pixmap (pbm) are supported yet.
     */
    aj_image *loadRGBImage();
    
    int decode();
    int encode(aj_image *rgbImage);
    int jo_encode(aj_image *rgbImage);
    
private:
    
    std::string _inputFile;
    std::string _outputFile;
    
    std::ifstream _inputFileStream;
    
    int  openInputFileStream();
    void readHeader();
    
    /*!
     *    Reads the header of a Portable Pixmap file and sets the file pointer
     *    to the beginning of the image binary data.
     */
    portablePixmapHeader readRGBHeader(std::ifstream& fs);
    std::string readToWhitespace(std::ifstream& fs);
    
    /*!
     *    Loads the binary data from fs into image.
     *    The width and the height of image must be set before calling this
     *    method. The position of fs must be set to the beginning of the image
     *    data stream.
     *    First, the method checks if the size of the data in fs equals to the
     *    image size.
     *    Returns 0 on success, 1 if the data size does not correspond to the
     *    image size and any other value for other errors.
     */
    int loadImageData(std::ifstream& fs, aj_image *image);
    
    
    /*!
     *    Sets the specified pixel in the pixel array on position x, y.
     *    The method also checks for a buffer overflow. If the specified pixel
     *    is outside of the buffer, it returns -1.
     *    Returns 0 on success.
     */
    int setPixel(aj_rgb_Pixel *pixels, aj_image *image, int x, int y, aj_rgb_Pixel pixel);
    
    
    
};


#endif /* defined(__AJEG__AJEG_Core__) */





