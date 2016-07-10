

/****************************************************************************************
    Implementation of Pixel Extraction Algorithms of Monochrome Bitmap Images (1bpp)
    Author:             Ashis Kumar Das
    Email:              akd.bracu@gmail.com
    GitHub:             https://github.com/AKD92
*****************************************************************************************/






#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Bitmap_Read_Algorithms.h"








// Function prototypes of auxiliary helper operations

uint32_t bitmap_LittleEndToINT32(unsigned char *pByteArray);

uint16_t bitmap_LittleEndToINT16(unsigned char *pByteArray);

unsigned char bitmap_readPixelValue(const unsigned char *pImageData, int scanLineSize, int row, int col);








void bitmap_readDibHeader(FILE *fpBitmap, struct DibHeader *pDibHeader)
{
    unsigned int dibDataLen;
    unsigned char *pDibData, arrHeaderSize[4];
    
    if (fpBitmap == 0 || pDibHeader == 0) {
        return;
    }
    
    pDibData = 0;
    dibDataLen = 0;
    fread((void *) arrHeaderSize, 1, 4, fpBitmap);
    dibDataLen = bitmap_LittleEndToINT32(arrHeaderSize) - 4;        // Already 4 byte is read
    
    pDibData = (unsigned char *) malloc(dibDataLen);
    fread((void *) pDibData, 1, dibDataLen, fpBitmap);              // This is the dib header without the size
    
    memset((void *) pDibHeader, 0,  sizeof(struct DibHeader));
    pDibHeader->imgWidth =          (int) bitmap_LittleEndToINT32(pDibData + 0);
    pDibHeader->imgHeight =         (int) bitmap_LittleEndToINT32(pDibData + 4);
    pDibHeader->bitsPerPixel =      (int) bitmap_LittleEndToINT16(pDibData + 10);
    pDibHeader->compression =       (int) bitmap_LittleEndToINT32(pDibData + 12);
    pDibHeader->imageDataLen =      (int) bitmap_LittleEndToINT32(pDibData + 16);
    pDibHeader->colorCount =        (int) bitmap_LittleEndToINT32(pDibData + 28);
    
    if (pDibHeader->imgHeight < 0) {
        pDibHeader->imgHeight = (-1) * pDibHeader->imgHeight;
    }
    
    free((void *) pDibData);
    return;
}




void bitmap_readPixelArray(unsigned char *pPixelArray,
                                const struct DibHeader *pDibHeader,
                                            const unsigned char *pImageData)
{
    
    int width, height;
    int scanLineSize;
    unsigned char cPixelValue;
    register int row, col;
    int rowIndex;
    
    if (pPixelArray == 0 || pImageData == 0 || pDibHeader == 0) {
        return;
    }
    
    width = pDibHeader->imgWidth;
    height = pDibHeader->imgHeight;
    scanLineSize = pDibHeader->imageDataLen / height;
    
    for(row = 0; row < height; row++) {
        rowIndex = width * row;
        for(col = 0; col < width; col++) {
            cPixelValue = bitmap_readPixelValue(pImageData, scanLineSize, row, col);
            *(pPixelArray + rowIndex + col) = !cPixelValue;
        }
    }
    return;
}




uint32_t bitmap_LittleEndToINT32(unsigned char *pByteArray)
{
    uint32_t intValue;
    
    intValue = 0;
    intValue = (uint32_t) pByteArray[0];
    intValue = intValue | ((uint32_t) pByteArray[1] << 8);
    intValue = intValue | ((uint32_t) pByteArray[2] << 16);
    intValue = intValue | ((uint32_t) pByteArray[3] << 24);
    return intValue;
}




uint16_t bitmap_LittleEndToINT16(unsigned char *pByteArray)
{
    uint16_t intValue;
    
    intValue = 0;
    intValue = (uint16_t) pByteArray[0];
    intValue = intValue | ((uint16_t) pByteArray[1] << 8);
    return intValue;
}




unsigned char bitmap_readPixelValue(const unsigned char *pImageData, int scanLineSize, int row, int col)
{
    // The bits representing the bitmap pixels are packed in rows
    // The size of each row is rounded up to a multiple of 4 bytes (a 32-bit DWORD) by padding
    
    int arrayIndex;
    unsigned char cByteValue;
    unsigned char cPixelMask;
    unsigned char cPixelValue;
    
    arrayIndex = (scanLineSize * row) + (col / 8);
    cByteValue = *(pImageData + arrayIndex);
    cPixelMask = 0x01 << (7 - (col % 8));
    cPixelValue = !!(cByteValue & cPixelMask);
    
    return cPixelValue;
}



