//
//  AJEG_Coders.cpp
//  AJEG
//
//  Created by Sebastian Mach on 06.02.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#include "AJEG_Coders.h"
#include "AJEG_Tables.h"
#include <math.h>






#pragma mark -
#pragma mark RGB <--> Block

int convRGBToBlockImage(aj_image *sourceImage, aj_image *destinationImage)
{
    //*********************************************************** validate input
    if ((!sourceImage      ||
         !destinationImage ||
         sourceImage->imageType != aj_imageType_rgbImage))
        return 1;
    
    
    
    //************************************************ prepare destination image
    int width  = sourceImage->width;
    int height = sourceImage->height;
    
    destinationImage->width     = width;
    destinationImage->height    = height;
    destinationImage->imageType = aj_imageType_blockImage;
    destinationImage->blocks    = (aj_block*)malloc(sizeof(aj_block) * (ceil(destinationImage->width/8) * ceil(destinationImage->height/8)));
    
    
    
    //************************************************** generate block contents
    
    for(int y = 0, blockOffset = 0; y < destinationImage->height; y += 8)  {
        for(int x = 0; x < destinationImage->width; x += 8, blockOffset++) {
            
            aj_block *block = (destinationImage->blocks + blockOffset);
            
            for(int row = y, pixelOffset = 0; row < y+8; row++)            {
                for(int col = x; col < x+8; col++, pixelOffset++)          {
                    
                    int p = (row * width) + col;
                    if(row >= height)
                        p -= width * (row+1 - height);
                    if(col >= width)
                        p -= (col+1 - width);
                    
                    aj_rgb_Pixel *pixel = (sourceImage->rgbPixels + p);
                    
                    float red   = (float)pixel->red;
                    float green = (float)pixel->green;
                    float blue  = (float)pixel->blue;
                    
                    block->luminance[pixelOffset] = (+0.29900f * red) + (0.58700f * green) + (0.11400f * blue) -128;
                    block->colorBlue[pixelOffset] = (-0.16874f * red) - (0.33126f * green) + (0.50000f * blue)     ;
                    block->colorRed [pixelOffset] = (+0.50000f * red) - (0.41869f * green) - (0.08131f * blue)     ;
                }
            }
        }
    }
    
    return 0;
}


int convBlockToRGBImage(aj_image *sourceImage, aj_image *destinationImage)
{
    return -1;
}






/******************************************************************************/

#pragma mark -
#pragma mark Block <--> DCT

#pragma mark FDCT

void performFDCTOnBlock(aj_block *sourceBlock, aj_block *destinationBlock);
void performFDCTOnChannel(float *chanel);
void fdct(float &d0, float &d1, float &d2, float &d3, float &d4, float &d5, float &d6, float &d7);


int convBlockToDCTImage(aj_image *sourceImage, aj_image *destinationImage)
{
    //*********************************************************** validate input
    if (!sourceImage      ||
        !destinationImage ||
        (sourceImage->imageType != aj_imageType_blockImage))
        return 1;
    
    
    
    //************************************************ prepare destination image
    int width  = sourceImage->width;
    int height = sourceImage->height;
    
    destinationImage->width     = width;
    destinationImage->height    = height;
    destinationImage->imageType = aj_imageType_dctImage;
    size_t numberOfBlocks       = (ceil(width / 8) * ceil(height / 8));
    destinationImage->blocks    = (aj_block*)malloc(sizeof(aj_block) * numberOfBlocks);
    
    
    
    //*********************************************** perform FDCT on each block
    for (int i = 0; i < numberOfBlocks; i++)
        performFDCTOnBlock((sourceImage->blocks + i), (destinationImage->blocks + i));
    
    
    
    
    return 0;
}


void performFDCTOnBlock(aj_block *sourceBlock, aj_block *destinationBlock)
{
    // Copy source to destination block because fdtc performs calculation
    // directly on the block
    memcpy(destinationBlock, sourceBlock, sizeof(aj_block));
    
    performFDCTOnChannel(destinationBlock->luminance);
    performFDCTOnChannel(destinationBlock->colorBlue);
    performFDCTOnChannel(destinationBlock->colorRed );
}


void performFDCTOnChannel(float *channel)
{
    // DCT rows
    for(int dataOff=0; dataOff<64; dataOff+=8)
    {
        fdct(channel[dataOff+0],
             channel[dataOff+1],
             channel[dataOff+2],
             channel[dataOff+3],
             channel[dataOff+4],
             channel[dataOff+5],
             channel[dataOff+6],
             channel[dataOff+7]);
    }
    
    // DCT columns
    for(int dataOff=0; dataOff<8; ++dataOff)
    {
        fdct(channel[dataOff+0],
             channel[dataOff+8],
             channel[dataOff+16],
             channel[dataOff+24],
             channel[dataOff+32],
             channel[dataOff+40],
             channel[dataOff+48],
             channel[dataOff+56]);
    }
}


void fdct(float &d0, float &d1, float &d2, float &d3, float &d4, float &d5, float &d6, float &d7)
{
    float tmp0 = d0 + d7;
    float tmp7 = d0 - d7;
    float tmp1 = d1 + d6;
    float tmp6 = d1 - d6;
    float tmp2 = d2 + d5;
    float tmp5 = d2 - d5;
    float tmp3 = d3 + d4;
    float tmp4 = d3 - d4;
    
    // Even part
    float tmp10 = tmp0 + tmp3;	// phase 2
    float tmp13 = tmp0 - tmp3;
    float tmp11 = tmp1 + tmp2;
    float tmp12 = tmp1 - tmp2;
    
    d0 = tmp10 + tmp11; 		// phase 3
    d4 = tmp10 - tmp11;
    
    float z1 = (tmp12 + tmp13) * 0.707106781f; // c4
    d2 = tmp13 + z1; 		    // phase 5
    d6 = tmp13 - z1;
    
    // Odd part
    tmp10 = tmp4 + tmp5; 		// phase 2
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;
    
    // The rotator is modified from fig 4-8 to avoid extra negations.
    float z5 = (tmp10 - tmp12) * 0.382683433f; // c6
    float z2 = tmp10 * 0.541196100f + z5;      // c2-c6
    float z4 = tmp12 * 1.306562965f + z5;      // c2+c6
    float z3 = tmp11 * 0.707106781f;           // c4
    
    float z11 = tmp7 + z3;		// phase 5
    float z13 = tmp7 - z3;
    
    d5 = z13 + z2;			    // phase 6
    d3 = z13 - z2;
    d1 = z11 + z4;
    d7 = z11 - z4;
}


void fdct2(float &d0, float &d1, float &d2, float &d3, float &d4, float &d5, float &d6, float &d7)
{
    static const float a = (  41.0 /  128.0);
    static const float b = (  99.0 /  128.0);
    static const float d = ( 113.0 /  128.0);
    static const float e = ( 719.0 / 4096.0);
    static const float n = (1533.0 / 2048.0);
    static const float t = (   1.0 /    2.0);
    
    
    
    //*********************************************************************** P1
    
    float temp_p1[8];
    
    temp_p1[0] = d0 + d7;
    temp_p1[1] = d1 + d6;
    temp_p1[2] = d2 + d5;
    temp_p1[3] = d3 + d4;
    temp_p1[4] = d3 - d4;
    temp_p1[5] = d2 - d5;
    temp_p1[6] = d1 - d6;
    temp_p1[7] = d0 - d7;
    
    
    
    //*********************************************************************** P2
    
    float temp_p2[4];
    
    temp_p2[0] = temp_p1[0] + temp_p1[3];
    temp_p2[1] = temp_p1[1] + temp_p1[2];
    temp_p2[2] = temp_p1[1] - temp_p1[2];
    temp_p2[3] = temp_p1[0] - temp_p1[3];
    
    
    
    //*********************************************************************** P3
    
    d0 = temp_p2[0] + temp_p2[1];
    d4 = temp_p2[0] - temp_p2[1];
    
    
    
    //*********************************************************************** P4
    
    //d2 = (temp_p2[2] * a) + (temp_p2[3] * b);
    //d6 = (temp_p2[3] * a) - (temp_p2[2] * b);
    
    float z1 = (temp_p2[2] + temp_p2[3]) * (0.707106781f);
    d2 = temp_p2[3] + z1;
    d6 = temp_p2[3] - z1;
    
    
    //*********************************************************************** P5
    
    float temp_p5[4];
    
    temp_p5[0] = (temp_p1[4] * n) + (temp_p1[7] * t);
    temp_p5[1] = (temp_p1[5] * d) + (temp_p1[6] * e);
    temp_p5[2] = (temp_p1[6] * d) - (temp_p1[5] * e);
    temp_p5[3] = (temp_p1[7] * n) - (temp_p1[4] * t);
    
    
    
    //*********************************************************************** P6
    
    float temp_p6[4];
    
    temp_p6[0] = temp_p5[0] + temp_p5[2];
    temp_p6[1] = temp_p5[3] - temp_p5[1];
    temp_p6[2] = temp_p5[0] - temp_p5[2];
    temp_p6[3] = temp_p5[1] + temp_p5[3];
    
    
    
    //*********************************************************************** P7
    
    d7 = temp_p6[3] - temp_p6[0];
    d3 = temp_p6[1];
    d5 = temp_p6[2];
    d1 = temp_p6[0] + temp_p6[3];
}






#pragma mark IDCT

void performIDCTOnBlock(aj_block *sourceBlock, aj_block *destinationBlock);
void performIDCTOnChannel(float *chanel);
void idct(float &d0, float &d1, float &d2, float &d3, float &d4, float &d5, float &d6, float &d7);


int convDCTToBlockImage(aj_image *sourceImage, aj_image *destinationImage)
{
    //*********************************************************** validate input
    if ((!sourceImage      ||
         !destinationImage ||
         sourceImage->imageType != aj_imageType_dctImage))
        return 1;
    
    
    
    //************************************************ prepare destination image
    int width  = sourceImage->width;
    int height = sourceImage->height;
    
    destinationImage->width     = width;
    destinationImage->height    = height;
    destinationImage->imageType = aj_imageType_blockImage;
    size_t numberOfBlocks       = (ceil(width / 8) * ceil(height / 8));
    destinationImage->blocks    = (aj_block*)malloc(sizeof(aj_block) * numberOfBlocks);
    
    
    
    //*********************************************** perform IDCT on each block
    for (int i = 0; i < numberOfBlocks; i++)
        performIDCTOnBlock((sourceImage->blocks + i), (destinationImage->blocks + i));
    
    
    
    
    return 0;
}


void performIDCTOnBlock(aj_block *sourceBlock, aj_block *destinationBlock)
{
    // Copy source to destination block because fdtc performs calculation
    // directly on the block
    memcpy(sourceBlock, destinationBlock, sizeof(aj_block));
    
    performIDCTOnChannel(destinationBlock->luminance);
    performIDCTOnChannel(destinationBlock->colorBlue);
    performIDCTOnChannel(destinationBlock->colorRed );
}


void performIDCTOnChannel(float *chanel)
{
    // DCT rows
    for(int dataOff=0; dataOff<64; dataOff+=8)
    {
        idct(chanel[dataOff+0],
             chanel[dataOff+1],
             chanel[dataOff+2],
             chanel[dataOff+3],
             chanel[dataOff+4],
             chanel[dataOff+5],
             chanel[dataOff+6],
             chanel[dataOff+7]);
    }
    
    // DCT columns
    for(int dataOff=0; dataOff<8; ++dataOff)
    {
        idct(chanel[dataOff+0],
             chanel[dataOff+8],
             chanel[dataOff+16],
             chanel[dataOff+24],
             chanel[dataOff+32],
             chanel[dataOff+40],
             chanel[dataOff+48],
             chanel[dataOff+56]);
    }
}


void idct(float &d0, float &d1, float &d2, float &d3, float &d4, float &d5, float &d6, float &d7)
{
    //TODO: please implement me
}






/******************************************************************************/

#pragma mark -
#pragma mark DTC <--> File

#pragma mark DTC > File

void writeJPEGHeaderForImage(FILE *fp, aj_image *image,
                             uint8_t *luminanceQuantizationTable  , size_t size_luminanceQuantizationTable,
                             uint8_t *chrominanceQuantizationTable, size_t size_chrominanceQuantizationTable);
void writeJPEGLuminanceChrominanceTables(FILE *fp,
                                         uint8_t *dc_luminance_codes   , size_t size_dc_luminance_codes,
                                         uint8_t *dc_luminance_values  , size_t size_dc_luminance_values,
                                         uint8_t *ac_luminance_codes   , size_t size_ac_luminance_codes,
                                         uint8_t *ac_luminance_values  , size_t size_ac_luminance_values,
                                         uint8_t *dc_chrominance_codes , size_t size_dc_chrominance_codes,
                                         uint8_t *dc_chrominance_values, size_t size_dc_chrominance_values,
                                         uint8_t *ac_chrominance_codes , size_t size_ac_chrominance_codes,
                                         uint8_t *ac_chrominance_values, size_t size_ac_chrominance_values);
void quantizeAndZigZagChannel(float *sourceChannel, int *destinationChannel, float quantizationTable[64]);
void writeJPEG_dcEncode(FILE *fp, int &bitBuf, int &bitCnt, int *channel, int dc, uint16_t huffmanTable[256][2]);
int  writeJPEG_acEncode(FILE *fp, int &bitBuf, int &bitCnt, int *channel,         uint16_t huffmanTable[256][2]);
void calcBits(int val, uint16_t bits[2]);
void writeBits(FILE *fp, int &bitBuf, int &bitCnt, uint16_t *bs);


int writeDCTImageToFile(aj_image *image, const char *destinationPath, int quality)
{
    //*********************************************************** validate input
    if (!image || (image->imageType != aj_imageType_dctImage) || !destinationPath)
        return 1;
    
    
    
    //**************************************************************** open file
    FILE *fp = fopen(destinationPath, "wb");
    if(!fp)
        return -1;
    
    
    
    //******************************************************** normalize quality
    quality =  quality       ?  quality         :  90;
    quality = (quality <  1) ?  1               : (quality > 100) ? 100 : quality;
    quality = (quality < 50) ? (5000 / quality) : (200 - quality * 2);
    
    
    
    //********************************************** prepare quantization tables
    
    uint8_t   luminanceQuantizationTable[64];
    uint8_t chrominanceQuantizationTable[64];
    
    for(int i = 0; i < 64; ++i)
    {
        int yti = (luminance_quantization[i]*quality+50)/100;
        luminanceQuantizationTable[zigZag[i]] = yti < 1 ? 1 : yti > 255 ? 255 : yti;
        int uvti  = (chrominance_quantization[i]*quality+50)/100;
        chrominanceQuantizationTable[zigZag[i]] = uvti < 1 ? 1 : uvti > 255 ? 255 : uvti;
    }
    
    
    float   fdLuminanceQuantizationTable[64];
    float fdChrominanceQuantizationTable[64];
    
    for(int row = 0, offset = 0; row < 8; row++)
    {
        for(int col = 0; col < 8; col++, offset++)
        {
            fdLuminanceQuantizationTable  [offset] = 1 / (  luminanceQuantizationTable[zigZag[offset]] * aasf[row] * aasf[col]);
            fdChrominanceQuantizationTable[offset] = 1 / (chrominanceQuantizationTable[zigZag[offset]] * aasf[row] * aasf[col]);
        }
    }
    
    
    
    //******************************************************** write file header
    writeJPEGHeaderForImage(fp, image,
                              luminanceQuantizationTable, sizeof(luminanceQuantizationTable),
                            chrominanceQuantizationTable, sizeof(chrominanceQuantizationTable));
    
    
    
    //*************************************** write luminance chrominance tables
    writeJPEGLuminanceChrominanceTables(fp,
                                        (uint8_t*)dc_luminance_codes   , sizeof(dc_luminance_codes),
                                        (uint8_t*)dc_luminance_values  , sizeof(dc_luminance_values),
                                        (uint8_t*)ac_luminance_codes   , sizeof(ac_luminance_codes),
                                        (uint8_t*)ac_luminance_values  , sizeof(ac_luminance_values),
                                        (uint8_t*)dc_chrominance_codes , sizeof(dc_chrominance_codes),
                                        (uint8_t*)dc_chrominance_values, sizeof(dc_chrominance_values),
                                        (uint8_t*)ac_chrominance_codes , sizeof(ac_chrominance_codes),
                                        (uint8_t*)ac_chrominance_values, sizeof(ac_chrominance_values));
    
    
    
    //******************************** quantize, ZigZag, AC and DC Encode blocks
    
    size_t numberOfBlocks = (ceil(image->width / 8) * ceil(image->height / 8));
    
    int dc_luminance = 0;
    int dc_colorBlue = 0;
    int dc_colorRed  = 0;
    
    int bitBuf = 0;
    int bitCnt = 0;
    
    for (int blockOffset = 0; blockOffset < numberOfBlocks; blockOffset ++)
    {
        aj_block *block = (image->blocks + blockOffset);
        
        int luminance[64];
        int colorBlue[64];
        int  colorRed[64];
        
        //************************************************************ luminance
        quantizeAndZigZagChannel(block->luminance,            luminance,               fdLuminanceQuantizationTable);
                       writeJPEG_dcEncode(fp, bitBuf, bitCnt, luminance, dc_luminance, luminance_dc_huffmanTable);
        dc_luminance = writeJPEG_acEncode(fp, bitBuf, bitCnt, luminance,               luminance_ac_huffmanTable);
        
        
        
        //*********************************************************** color blue
        quantizeAndZigZagChannel(block->colorBlue,            colorBlue,               fdChrominanceQuantizationTable);
                       writeJPEG_dcEncode(fp, bitBuf, bitCnt, colorBlue, dc_colorBlue, chrominance_dc_huffmanTable);
        dc_colorBlue = writeJPEG_acEncode(fp, bitBuf, bitCnt, colorBlue,               chrominance_ac_huffmanTable);
        
        
        
        //************************************************************ color red
        quantizeAndZigZagChannel(block->colorRed,            colorRed,              fdChrominanceQuantizationTable);
                      writeJPEG_dcEncode(fp, bitBuf, bitCnt, colorRed, dc_colorRed, chrominance_dc_huffmanTable);
        dc_colorRed = writeJPEG_acEncode(fp, bitBuf, bitCnt, colorRed,              chrominance_ac_huffmanTable);
    }
    
    
    
    
    
    
    
    
    
    
    //************************************************************* write footer
    
    uint16_t fillBits[] = {0x7F, 7};
    writeBits(fp, bitBuf, bitCnt, fillBits);
    
    // EOI
    putc(0xFF, fp);
    putc(0xD9, fp);
    
    fclose(fp);
    
    return 0;
}


void writeJPEGHeaderForImage(FILE *fp, aj_image *image,
                             uint8_t   *luminanceQuantizationTable, size_t   size_luminanceQuantizationTable,
                             uint8_t *chrominanceQuantizationTable, size_t size_chrominanceQuantizationTable)
{
    //****************************************************** define file headers
    static const uint8_t head0[] = {
        0xFF,
        0xD8,
        0xFF,
        0xE0,
        0x00,
        0x10,
        'J',
        'F',
        'I',
        'F',
        0x00,
        0x01,
        0x01,
        0x00,
        0x00,
        0x01,
        0x00,
        0x01,
        0x00,
        0x00,
        0xFF,
        0xDB,
        0x00,
        0x84,
        0x00
    };
    
    const uint8_t head1[] = {
        0xFF,
        0xC0,
        0x00,
        0x11,
        0x08,
        static_cast<uint8_t>((image->height >> 0x08)),
        static_cast<uint8_t>(image->height  &  0xFF),
        static_cast<uint8_t>(image->width   >> 0x08),
        static_cast<uint8_t>(image->width   &  0xFF),
        0x03,
        0x01,
        0x11,
        0x00,
        0x02,
        0x11,
        0x01,
        0x03,
        0x11,
        0x01,
        0xFF,
        0xC4,
        0x01,
        0xA2,
        0x00
    };
    
    
    
    //***************************************************** write header to file
    fwrite(head0, sizeof(head0), 1, fp);
    
    fwrite(  luminanceQuantizationTable, sizeof(uint8_t), size_luminanceQuantizationTable, fp);
    putc(1, fp);
    fwrite(chrominanceQuantizationTable, sizeof(uint8_t), size_chrominanceQuantizationTable, fp);
    
    fwrite(head1, sizeof(head1), 1, fp);
}


void writeJPEGLuminanceChrominanceTables(FILE *fp,
                                         uint8_t *dc_luminance_codes   , size_t size_dc_luminance_codes,
                                         uint8_t *dc_luminance_values  , size_t size_dc_luminance_values,
                                         uint8_t *ac_luminance_codes   , size_t size_ac_luminance_codes,
                                         uint8_t *ac_luminance_values  , size_t size_ac_luminance_values,
                                         uint8_t *dc_chrominance_codes , size_t size_dc_chrominance_codes,
                                         uint8_t *dc_chrominance_values, size_t size_dc_chrominance_values,
                                         uint8_t *ac_chrominance_codes , size_t size_ac_chrominance_codes,
                                         uint8_t *ac_chrominance_values, size_t size_ac_chrominance_values)
{
    //************************************************* define image data header
    static const unsigned char head2[] = {
        0xFF,
        0xDA,
        0x00,
        0x0C,
        0x03,
        0x01,
        0x00,
        0x02,
        0x11,
        0x03,
        0x11,
        0x00,
        0x3F,
        0x00
    };
    
    
    
    //************************************************************* dc luminance
    fwrite(dc_luminance_codes+1  , size_dc_luminance_codes-1  , 1, fp);
    fwrite(dc_luminance_values   , size_dc_luminance_values   , 1, fp);
    
    
    
    //************************************************************* ac luminance
    putc(0x10, fp);
    fwrite(ac_luminance_codes+1  , size_ac_luminance_codes-1  , 1, fp);
    fwrite(ac_luminance_values   , size_ac_luminance_values   , 1, fp);
    
    
    
    //*********************************************************** dc chrominance
    putc(0x01, fp);
    fwrite(dc_chrominance_codes+1, size_dc_chrominance_codes-1, 1, fp);
    fwrite(dc_chrominance_values , size_dc_chrominance_values , 1, fp);
    
    
    
    //************************************************************* ac luminance
    putc(0x11, fp);
    fwrite(ac_chrominance_codes+1, size_ac_chrominance_codes-1, 1, fp);
    fwrite(ac_chrominance_values , size_ac_chrominance_values , 1, fp);
    
    
    
    //*********************************************************** start of image
    fwrite(head2, sizeof(head2), 1, fp);
}


void quantizeAndZigZagChannel(float *sourceChannel, int *destinationChannel, float quantizationTable[64])
{
    for(int i = 0; i < 64; i++)
    {
        float v = sourceChannel[i] * quantizationTable[i];
        destinationChannel[zigZag[i]] = (int)((v < 0) ? ceilf(v - 0.5f) : floorf(v + 0.5f));
    }
}


void writeJPEG_dcEncode(FILE *fp, int &bitBuf, int &bitCnt, int *channel, int dc, uint16_t huffmanTable[256][2])
{
    int diff = channel[0] - dc;
    if (diff == 0)
        writeBits(fp, bitBuf, bitCnt, huffmanTable[0]);
    else
    {
        unsigned short bits[2];
        calcBits(diff, bits);
        writeBits(fp, bitBuf, bitCnt, huffmanTable[bits[1]]);
        writeBits(fp, bitBuf, bitCnt, bits);
    }
}


int writeJPEG_acEncode(FILE *fp, int &bitBuf, int &bitCnt, int *channel, uint16_t huffmanTable[256][2])
{
    uint16_t EOB[2]       = { huffmanTable[0x00][0], huffmanTable[0x00][1] };
    uint16_t M16zeroes[2] = { huffmanTable[0xF0][0], huffmanTable[0xF0][1] };
    
    int end0pos = 63;
    
    while ((end0pos > 0) && (channel[end0pos] == 0)) end0pos--;
    
    // end0pos = first element in reverse order !=0
    if(end0pos == 0)
    {
        writeBits(fp, bitBuf, bitCnt, EOB);
        return channel[0];
    }
    
    
    for(int i = 1; i <= end0pos; i++)
    {
        int startpos = i;
        while ((channel[i] == 0) && (i <= end0pos)) i++;
        
        int nrzeroes = i - startpos;
        if ( nrzeroes >= 16 )
        {
            int lng = nrzeroes >> 4;
            
            for (int nrmarker=1; nrmarker <= lng; ++nrmarker)
                writeBits(fp, bitBuf, bitCnt, M16zeroes);
                
            nrzeroes &= 15;
        }
        
        unsigned short bits[2];
        calcBits(channel[i], bits);
        writeBits(fp, bitBuf, bitCnt, huffmanTable[(nrzeroes<<4)+bits[1]]);
        writeBits(fp, bitBuf, bitCnt, bits);
    }
    
    
    if(end0pos != 63)
        writeBits(fp, bitBuf, bitCnt, EOB);
    
    return channel[0];
}


void calcBits(int val, uint16_t bits[2])
{
    int tmp1 = val < 0 ? -val : val;
    val      = val < 0 ? val-1 : val;
    bits[1]  = 1;
    
    while(tmp1 >>= 1) ++bits[1];
    
    bits[0] = val & ((1 << bits[1]) - 1);
}


void writeBits(FILE *fp, int &bitBuf, int &bitCnt, uint16_t *bs)
{
    bitCnt += bs[1];
    bitBuf |= bs[0] << (24 - bitCnt);
    
    while(bitCnt >= 8)
    {
        unsigned char c = (bitBuf >> 16) & 255;
        putc(c, fp);
        
        if(c == 255)
            putc(0, fp);
        
        bitBuf <<= 8;
        bitCnt  -= 8;
    }
}



