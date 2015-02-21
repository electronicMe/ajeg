//
//  AJEG_Core.mm
//  AJEG
//
//  Created by Sebastian Mach on 14.01.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#include "AJEG_Core.h"
#include <sstream>

#define JO_JPEG_HEADER_FILE_ONLY
#include "jo_jpeg.cpp"
#include "AJEG_Coders.h"


aj_rgb_Pixel *getPixelInImage(aj_image *image, int x, int y)
{
    if (!image)
    {
        std::cout << AC_ERROR << "getPixelInImage image is NULL." << std::endl;
        return NULL;
    }
    
    if (image->imageType != aj_imageType_rgbImage)
    {
        std::cout << AC_ERROR << "getPixelInImage only aj_imageType_rgbImage supported yet." << std::endl;
        return NULL;
    }
    
    if (x > image->width)
    {
        std::cout << AC_ERROR << "getPixelInImage x coordinate is outside of the image." << std::endl;
        return NULL;
    }
    
    if (y > image->height)
    {
        std::cout << AC_ERROR << "getPixelInImage y coordinate is outside of the image." << std::endl;
        return NULL;
    }
    
    
    size_t offset = ((y * image->width) +  x);
    aj_rgb_Pixel *thePixel = (image->rgbPixels + offset);
    
    return thePixel;
}


AJEG_Core::AJEG_Core(std::string inputFile, std::string outputFile)
{
    _inputFile  = inputFile;
    _outputFile = outputFile;
    
    std::cout << AC_DEBUG << "Input File: "  << inputFile  << std::endl;
    std::cout << AC_DEBUG << "Output File: " << outputFile << std::endl;
    
    openInputFileStream();
}


AJEG_Core::~AJEG_Core()
{
    
}


int AJEG_Core::decode()
{
    return 0;
}


int AJEG_Core::encode(aj_image *rgbImage)
{
    //return jo_write_jpg(_outputFile.c_str(), rgbImage->rgbPixels, rgbImage->width, rgbImage->height, 3, 90);
    
    aj_image blockImage;
    aj_image dctImage;
    aj_image quantizedImage;
    
    
    
    int result = 0;
    
    if ((result = convRGBToBlockImage(rgbImage, &blockImage)) != 0)
    {
        std::cout << AC_ERROR << "Error converting RGB to Block Image" << std::endl;
        return result;
    }
    
    
    
    if ((result = convBlockToDCTImage(&blockImage, &dctImage)) != 0)
    {
        std::cout << AC_ERROR << "Error converting Block to DCT Image" << std::endl;
        return result;
    }
    
    
    
    if ((result = convDCTTToQuantizedImage(&dctImage, &quantizedImage, 100)) != 0)
    {
        std::cout << AC_ERROR << "Error converting DCT to Quantized Image" << std::endl;
        return result;
    }
    
    
    
    if ((result = writeQuantizedImageToFile(&quantizedImage, _outputFile.c_str())) != 0)
    {
        std::cout << AC_ERROR << "Error writing DCT Image to file" << std::endl;
        return result;
    }
    
    
    //return jo_write_jpg(_outputFile.c_str(), image->rgbPixels, image->width, image->height, 3, 90);
    return 0;
}



int AJEG_Core::jo_encode(aj_image *rgbImage)
{
    return jo_write_jpg(_outputFile.c_str(), rgbImage->rgbPixels, rgbImage->width, rgbImage->height, 3, 90);
}


int AJEG_Core::openInputFileStream()
{
    _inputFileStream = std::ifstream(_inputFile, std::ifstream::in | std::ifstream::binary);
    return _inputFileStream.is_open();
}


aj_image *AJEG_Core::loadRGBImage()
{
    //*************************************************************** loadHeader
    
    portablePixmapHeader header = readRGBHeader(_inputFileStream);
    
    if (header.magicalValue != P6)
    {
        std::cout << AC_ERROR << "Magical value of image not supported!" << std::endl;
        return NULL;
    }
    else if (header.maxValue != 255)
    {
        std::cout << AC_ERROR << "Maximum value of image not supported!" << std::endl;
        return NULL;
    }
    
    
    
    //********************************************************* initialize image
    
    aj_image *image = (aj_image*)malloc(sizeof(image));
    
    image->imageType = aj_imageType_rgbImage;
    image->width     = header.width;
    image->height    = header.height;
    
    
    
    //********************************************************** load image data
    
    int returnValue = loadImageData(_inputFileStream, image);
    
    if (returnValue == 1)
    {
        std::cout << AC_ERROR << "Image size of header does not correspond to image data size. File might be corrupted." << std::endl;
        free(image);
        return NULL;
    }
    else if (returnValue != 0)
    {
        std::cout << AC_ERROR << "Unknown error when processing the input file: " << returnValue << std::endl;
        free(image);
        return NULL;
    }
    
    
    
    return image;
}


portablePixmapHeader AJEG_Core::readRGBHeader(std::ifstream& fs)
{
    portablePixmapHeader header;
    fs.seekg(_inputFileStream.beg);
    
    
    // Read Header
    std::string magicValue = readToWhitespace(fs);
    std::string width      = readToWhitespace(fs);
    std::string height     = readToWhitespace(fs);
    std::string maxValue   = readToWhitespace(fs);
    
    
    // Determinate magical value
    if (magicValue == "P1")
        header.magicalValue = P1;
    else if (magicValue == "P2")
        header.magicalValue = P2;
    else if (magicValue == "P3")
        header.magicalValue = P3;
    else if (magicValue == "P4")
        header.magicalValue = P4;
    else if (magicValue == "P5")
        header.magicalValue = P5;
    else if (magicValue == "P6")
        header.magicalValue = P6;
    else
        header.magicalValue = P0;
    
    
    // Determinate image width
    header.width = atoi(width.c_str());
    
    
    // Determinate image height
    header.height = atoi(height.c_str());
    
    
    // Determinate max value
    header.maxValue = atoi(maxValue.c_str());
        
    
    return header;
}


std::string AJEG_Core::readToWhitespace(std::ifstream& fs)
{
    std::stringstream text;
    
    char letter;
    
    while (true)
    {
        fs.read(&letter, 1);
        
        if ((letter == ' ') || (letter == '\t') || (letter == '\r') || (letter == '\n'))
            break;
        
        text << letter;
    }
    
    return text.str();
}


int AJEG_Core::loadImageData(std::ifstream& fs, aj_image *image)
{
    //****************************************************** get image data size
    
    long long start  = fs.tellg();
    fs.seekg(0, fs.end);
    long long end = fs.tellg();
    fs.seekg(start, fs.beg);
    long long length = end - start;
    
    
    
    //**************************************************** check image data size
    
    if ((length / 3) != (image->width * image->height)) // (length * 3) for red, green, blue (each 1 byte)
        return 1;
    
    
    
    //********************************************************** load image data
    
    size_t arraySize = (sizeof(aj_rgb_Pixel) * (image->width * image->height));
    aj_rgb_Pixel *pixels = (aj_rgb_Pixel*)malloc(arraySize);
    image->rgbPixels    = pixels;
    
    
    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            aj_rgb_Pixel pixel;
            
            char red;
            char green;
            char blue;
            
            fs.read(&red  , 1);
            fs.read(&green, 1);
            fs.read(&blue , 1);
            
            pixel.red   = red;
            pixel.green = green;
            pixel.blue  = blue;
            
            if (setPixel(pixels, image, x, y, pixel) != 0)
            {
                std::cout << AC_ERROR << "Buffer overflow in setPixel" << std::endl;
                return 2;
            }
        }
    }
    
    
    
    
    
    return 0;
}


int AJEG_Core::setPixel(aj_rgb_Pixel *pixels, aj_image *image, int x, int y, aj_rgb_Pixel pixel)
{
    if (image->width < x)
        return -1;
    else if (image->height < y)
        return -1;
    else
    {
        //std::cout << "setPixels x: " << x << " y: " << y << " r: " << pixel.red << " g: " << pixel.green << " b: " << pixel.blue << std::endl;
        size_t offset = ((y * image->width) +  x);
        aj_rgb_Pixel *pixelInArray = (pixels + offset);
        
        pixelInArray->red   = pixel.red;
        pixelInArray->green = pixel.green;
        pixelInArray->blue  = pixel.blue;
        
        return 0;
    }
}





