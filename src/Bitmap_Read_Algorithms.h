

/****************************************************************************************
    Program Interface of Pixel Extraction Algorithms of Monochrome Bitmap Images (1 bpp)
    Author:             Ashis Kumar Das
    Email:              akd.bracu@gmail.com
    GitHub:             https://github.com/AKD92
*****************************************************************************************/






#ifndef BITMAP_READ_ALGORITHMS_H
#define BITMAP_READ_ALGORITHMS_H





#include <stdio.h>
#include <stdint.h>





#define BMX1                            'B'
#define BMX2                            'M'
#define BMP_HEADER_SIZE                  14
#define BMP_COLOR_PALETTE_SIZE           8





struct DibHeader {                              // Standard Data Structure for storing BITMAP Information
    
    int imgWidth;                               // Width of the bitmap image (in pixels)
    int imgHeight;                              // Height of the bitmap image (in pixels)
    int imageDataLen;                           // Length of the bitmap image WITH PADDING (in bytes)
    int bitsPerPixel;                           // Number of bits used to represent each pixel
    uint64_t colorCount;                        // Number of total colors in the color pallette
    int compression;                            // Is RLE compression applied or not (0 for none)
    unsigned char padBitPerRow;
};




void bitmap_readDibHeader(FILE *fpBitmap, struct DibHeader *pDibHeader);


void bitmap_readPixelArray(unsigned char *pPixelArray,
                                const struct DibHeader *pDibHeader,
                                            const unsigned char *pImageData);






#endif



