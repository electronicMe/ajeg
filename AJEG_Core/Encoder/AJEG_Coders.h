//
//  AJEG_Coders.h
//  AJEG
//
//  Created by Sebastian Mach on 06.02.15.
//  Copyright (c) 2015 S-TEK. All rights reserved.
//

#ifndef __AJEG__AJEG_Coders__
#define __AJEG__AJEG_Coders__

#include <stdio.h>
#include "AJEG_Core.h"



/******************************************************************************/

#pragma mark -
#pragma mark RGB <--> Block

/*!
 *    Converts a RGB Image into a Block Image and converts the RGB Color Model
 *    into the YCbCr Color Model.
 *
 *    Returns:
 *    0 for success
 *    1 for invalid input
 *   -1 for unknown error
 */
int convRGBToBlockImage(aj_image *sourceImage, aj_image *destinationImage);


/*!
 *    Converts a Block Image into a RGB Image and converts the YCbCr Color Model
 *    into the RGB Color Model.
 *
 *    Returns:
 *    0 for success
 *    1 for invalid input
 *   -1 for unknown error
 */
int convBlockToRGBImage(aj_image *sourceImage, aj_image *destinationImage);






/******************************************************************************/

#pragma mark -
#pragma mark Block <--> DCT

/*!
 *    Converts a Block Image into a DCT (discrete cosinus transformation) Image.
 *
 *    Returns:
 *    0 for success
 *    1 for invalid input
 *   -1 for unknown error
 */
int convBlockToDCTImage(aj_image *sourceImage, aj_image *destinationImage);

/*!
 *    Converts a DCT (discrete cosinus transformation) Image into a Block Image.
 *
 *    Returns:
 *    0 for success
 *    1 for invalid input
 *   -1 for unknown error
 */
int convDCTToBlockImage(aj_image *sourceImage, aj_image *destinationImage);






/******************************************************************************/

#pragma mark -
#pragma mark Block <--> DCT

/*!
 *    Converts a DCT Image into a quantized image. The standard quantization
 *    table specified in the AJEG_Tables.h file is used for quantization.
 *
 *    Returns:
 *    0 for success
 *    1 for invalid input
 *   -1 for unknown error
 */
int convDCTTToQuantizedImage(aj_image *sourceImage, aj_image *destinationImage, int quality);






/******************************************************************************/

#pragma mark -
#pragma mark Quantized Image <--> File

/*!
 *    Writes the specified image to the specified path.
 *    Performs quantisation, ZigZag, AC and DC encode on the image and writes
 *    the calculated data to the JPEG File.
 *
 *    Returns:
 *    0 for success
 *    1 for invalid input
 *   -1 for unknown error
 */
int writeQuantizedImageToFile(aj_image *image, const char *destinationPath);









#endif /* defined(__AJEG__AJEG_Coders__) */
