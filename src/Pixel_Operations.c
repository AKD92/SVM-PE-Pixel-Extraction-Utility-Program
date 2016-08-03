

/****************************************************************************************
    Implementation of various GUI Callback Functions for working with Pixel Arrays
    Author:             Ashis Kumar Das
    Email:              akd.bracu@gmail.com
    GitHub:             https://github.com/AKD92
*****************************************************************************************/






#include <iup.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "Bitmap_Read_Algorithms.h"





#define ID_DLGMAIN              "MainDialog"





// Data structure for storing Bitmap Info and Pixel Data
extern struct DibHeader *pHeader;
extern unsigned char *pPixelArray;


// Pointers to existing GUI objects
extern Ihandle *canvas;
extern Ihandle *txtOpen;
extern Ihandle *lblWidth, *lblHeight;
extern Ihandle *lblBpp, *lblColor, *lblCompression;
extern Ihandle *lblPadByteCount;
extern Ihandle *btnStoreLSBFirst;
extern Ihandle *btnStoreMSBFirst;


// Utility function prototypes
void util_tryIncludeExtension(char *strFile, const char *strExtension);
unsigned char util_pack8ByteMSBFirst(unsigned char *pByteArray, unsigned int startPosition);
unsigned char util_pack8ByteLSBFirst(unsigned char *pByteArray, unsigned int startPosition);



// GUI Callback function prototypes
int cb_btnOpen(Ihandle *ih);
int cb_btnExtractPixels(Ihandle *ih);
int cb_btnInvertPixels(Ihandle *ih);
int cb_btnStoreBitRaster(Ihandle *btn);
int cb_btnEraseData(Ihandle *ih);




// String constants used in GUI Label objects
static const char *frmt1 = "%d pixels";
static const char *frmt2 = "%d bpp";
static const char *frmt3 = "%" PRIu64;
static const char *frmt4 = "%d (RLE)";
static const char *frmt5 = "%u bits";
static const char *strHeaderError
            = "This is not a bitmap image file at all\n\n%s";
static const char *strOpenFilter
            = "1 bitPerPixel Monochrome Bitmaps (*.BMP)|*.bmp|All Files (*.*)|*.*|";
static const char *strInvalidBpp
            = "Wrong value : %d bpp\nThis is not a valid 1 bpp Monochrome Bitmap Image";
static const char *strInvalidWidth
            = "Wrong value : %d pixels\nESC\\POS requires that width must be a multiple of 8";
static const char *strSaveDlgTitleLSB
            = "LSB First Raster";
static const char *strSaveDlgTitleMSB
            = "MSB First Raster";
static const char *strSaveFilterInfoLSB
            = "ESC\\POS Single-Bit Raster LSB First (*.R)";
static const char *strSaveFilterInfoMSB
            = "ESC\\POS Single-Bit Raster MSB First (*.R)";
static const char *strFileWriteError
            = "An error occured while attempting to write to the file:\n\n%s";








// Implementation of GUI Callback functions


int cb_btnOpen(Ihandle *ih) {
    
    Ihandle *dlgFile;
    char *strFile;
    int dlgStatus;
    
    dlgFile = IupFileDlg();
    IupSetAttribute(dlgFile, "ALLOWNEW", "NO");
    IupSetAttribute(dlgFile, "DIALOGTYPE", "OPEN");
    IupSetAttribute(dlgFile, "MULTIPLEFILES", "NO");
    IupSetAttribute(dlgFile, "PARENTDIALOG", ID_DLGMAIN);
    IupSetAttribute(dlgFile, "TITLE", "Open a Monochrome Bitmap");
    IupSetAttribute(dlgFile, "EXTFILTER", strOpenFilter);
    IupPopup(dlgFile, IUP_CENTER, IUP_CENTER);
    
    dlgStatus = IupGetInt(dlgFile, "STATUS");
    if (dlgStatus == 0) {
        strFile = IupGetAttribute(dlgFile, "VALUE");
        IupSetStrAttribute(txtOpen, "VALUE", strFile);
    }
    IupDestroy(dlgFile);
    return IUP_DEFAULT;
}



int cb_btnExtractPixels(Ihandle *btn) {
    
    char *atrTitle;
    char *strFile;
    FILE *fpBitmap;
    Ihandle *dlgMsg;
    struct DibHeader dibHeader;                             // we store image information in DibHeader
    unsigned char bmpSignature[2];                          // 2 byte file signature
    unsigned char bmpHeader[BMP_HEADER_SIZE];               // 14 byte file header
    unsigned char colPalette[BMP_PALETTE_SIZE];       // 8 byte color pallette
    unsigned char *pImageData;                              // arbitrary length raw image data with padding
    
    fpBitmap = 0;
    pImageData = 0;
    atrTitle = "TITLE";
    strFile = IupGetAttribute(txtOpen, "VALUE");
    
    if (strFile == 0) {
        goto END;
    }
    if (strlen(strFile) == 0) {
        goto END;
    }
    
    
    // Open selected file for reading a 1bpp Monochrome Bitmap Image
    
    fpBitmap = fopen(strFile, "rb");                    // File mode must be 'READ IN BINARY'
    if (fpBitmap == 0) {
        goto END;
    }
    
    
    // Read first 2 bytes from opened file to verify file signature
    
    fread((void *) bmpSignature, 1, 2, fpBitmap);
    if (bmpSignature[0] != BMX1 || bmpSignature[1] != BMX2) {
        dlgMsg = IupMessageDlg();
        IupSetAttribute(dlgMsg, "DIALOGTYPE", "ERROR");
        IupSetAttribute(dlgMsg, "PARENTDIALOG", ID_DLGMAIN);
        IupSetAttribute(dlgMsg, atrTitle, "Header Error");
        IupSetfAttribute(dlgMsg, "VALUE", strHeaderError, strFile);
        IupPopup(dlgMsg, IUP_CENTER, IUP_CENTER);
        IupDestroy(dlgMsg);
        goto END;
    }
    
    
    // Rewind current opened file and read from the start again
    
    rewind(fpBitmap);
    fread((void *) bmpHeader, 1, BMP_HEADER_SIZE, fpBitmap);
    
    
    // Read BITMAPINFOHEADER structure (See MSDN documentation for this C structure)
    
    bitmap_readDibHeader(fpBitmap, &dibHeader);
    
    free((void *) pHeader);                                         // Erase old header memory
    free((void *) pPixelArray);                                     // Erase old pixel data memory
    
    
    // Allocate new memory space for storing Image header info
    
    pHeader = 0;
    pPixelArray = 0;
    pHeader = (struct DibHeader *) malloc(sizeof(struct DibHeader));
    
    
    // Data copy from temporary header memory to newly allocated header memory
    
    memcpy((void *) pHeader, (const void *) &dibHeader, sizeof(struct DibHeader));
    
    
    // Update GUI objects to reflect new header information
    
    IupSetfAttribute(lblWidth,              atrTitle,   frmt1, pHeader->imgWidth);
    IupSetfAttribute(lblHeight,             atrTitle,   frmt1, pHeader->imgHeight);
    IupSetfAttribute(lblBpp,                atrTitle,   frmt2, pHeader->bitsPerPixel);
    IupSetfAttribute(lblColor,              atrTitle,   frmt3, pHeader->colorCount);
    IupSetfAttribute(lblPadByteCount,       atrTitle,   frmt5, pHeader->padBitPerRow);
    
    if (pHeader->compression == 0) {
        IupSetAttribute(lblCompression,     atrTitle, "0 (None)");
    }
    else {
        IupSetfAttribute(lblCompression,    atrTitle, frmt4, pHeader->compression);
    }
    
    
    // If this image is not a Monochrome Bitmap (not 1bpp), show an error message
    
    if (pHeader->bitsPerPixel != 1) {
        dlgMsg = IupMessageDlg();
        IupSetAttribute(dlgMsg, "DIALOGTYPE", "WARNING");
        IupSetAttribute(dlgMsg, "PARENTDIALOG", ID_DLGMAIN);
        IupSetAttribute(dlgMsg, atrTitle, "Format Error");
        IupSetfAttribute(dlgMsg, "VALUE", strInvalidBpp, pHeader->bitsPerPixel);
        
        // Refresh GUI object & clear the Image Canvas
        IupRefresh(btn);
        IupUpdate(canvas);
        
        IupPopup(dlgMsg, IUP_CENTER, IUP_CENTER);
        IupDestroy(dlgMsg);
        goto END;
    }
    
    
    // Allocate memory for storing raw pixel data & refined pixel array
    
    pImageData = (unsigned char *) malloc(pHeader->imageDataLen);
    pPixelArray = (unsigned char *) malloc(pHeader->imgWidth * pHeader->imgHeight);
    
    if (pImageData == 0 || pPixelArray == 0) {
        free((void *) pPixelArray);
        goto END;
    }
    
    
    // Read color pallette data & pixel data from bitmap file
    
    fread((void *) colPalette, 1, BMP_PALETTE_SIZE, fpBitmap);
    fread((void *) pImageData, 1, pHeader->imageDataLen, fpBitmap);
    
    
    // Algorithm for extracting binary pixel array from raw pixel data
    // This is the MOST IMPORTANT OPERATION
    // This operation processes ACTUAL PIXEL ARRAY that we need
    // Data Flow: pImageData (source) ----> pPixelArray (destination)
    
    bitmap_readPixelArray(pPixelArray,
                            (const struct DibHeader *) pHeader,
                                    (const unsigned char *) pImageData);
    
    
    // Update GUI objects to reflect the changes
    // UPDATE:
    
    IupRefresh(btn);
    IupRedraw(lblWidth, 0);
    IupRedraw(lblHeight, 0);
    IupRedraw(lblBpp, 0);
    IupRedraw(lblColor, 0);
    IupRedraw(lblCompression, 0);
    IupRedraw(lblPadByteCount, 0);
    IupUpdate(canvas);
    
    END:
    fclose(fpBitmap);
    free((void *) pImageData);
    return IUP_DEFAULT;
}



int cb_btnInvertPixels(Ihandle *ih) {
    
    int width;
    int row_a, row_b;
    unsigned char *pCopy;
    unsigned char *pTopRow, *pBottomRow;
    
    if (pHeader == 0 || pPixelArray == 0) {
        goto END;
    }
    
    pCopy = 0;
    row_a = 0;
    row_b = pHeader->imgHeight - 1;
    width = pHeader->imgWidth;
    pCopy = (unsigned char *) malloc(width);                // Temporary memory used in swap operation
    
    if (pCopy == 0) {
        goto END;
    }
    
    // Run a row-swapping algorithm
    
    while (row_a < row_b) {
        pTopRow = pPixelArray + (row_a * width);
        pBottomRow = pPixelArray + (row_b * width);
        memcpy((void *) pCopy,          (const void *) pTopRow,         width);
        memcpy((void *) pTopRow,        (const void *) pBottomRow,      width);
        memcpy((void *) pBottomRow,     (const void *) pCopy,           width);
        row_a = row_a + 1;
        row_b = row_b - 1;
    }
    
    // Update GUI object
    // Draw the inverted pixel array on the graphic canvas, pixel by pixel
    
    IupUpdate(canvas);
    free((void *) pCopy);                               // Erase temporary memory
    
    END:
    return IUP_DEFAULT;
}



int cb_btnEraseData(Ihandle *btn) {
    
    const char *atrTitle;
    
    free((void *) pHeader);
    free((void *) pPixelArray);
    
    pHeader = 0;
    pPixelArray = 0;
    atrTitle = "TITLE";
    
    IupSetAttribute(lblWidth,           atrTitle, 0);
    IupSetAttribute(lblHeight,          atrTitle, 0);
    IupSetAttribute(lblBpp,             atrTitle, 0);
    IupSetAttribute(lblColor,           atrTitle, 0);
    IupSetAttribute(lblCompression,     atrTitle, 0);
    IupSetAttribute(lblPadByteCount,    atrTitle, 0);
    
    IupRefresh(btn);
    IupUpdate(canvas);
    return IUP_DEFAULT;
}



int cb_btnStoreBitRaster(Ihandle *btn) {
    
    FILE *fpRaster;
    Ihandle *dlgFile, *dlgMsg;
    char *strFile;
    const char *strSelection;
    unsigned int index;
    unsigned int byteCount;
    unsigned char packByte;
    int dlgStatus;
    const char *strTitle, *strFilterInfo;
    unsigned char (*packAlgorithm)(unsigned char *pByteArray, unsigned int iPos);
    
    dlgFile = 0;
    fpRaster = 0;
    strFile = 0;
    packByte = 0x00;
    
    if (pHeader == 0 || pPixelArray == 0) {
        goto END;
    }
    if (pHeader->imgWidth % 8 != 0) {
        dlgMsg = IupMessageDlg();
        IupSetAttribute(dlgMsg, "DIALOGTYPE", "ERROR");
        IupSetAttribute(dlgMsg, "PARENTDIALOG", ID_DLGMAIN);
        IupSetAttribute(dlgMsg, "TITLE", "Error");
        IupSetfAttribute(dlgMsg, "VALUE", strInvalidWidth, pHeader->imgWidth);
        IupPopup(dlgMsg, IUP_CENTER, IUP_CENTER);
        IupDestroy(dlgMsg);
        goto END;
    }
    
    if (btn == btnStoreLSBFirst) {
        strTitle = strSaveDlgTitleLSB;
        strFilterInfo = strSaveFilterInfoLSB;
        packAlgorithm = util_pack8ByteLSBFirst;
    }
    else if (btn == btnStoreMSBFirst) {
        strTitle = strSaveDlgTitleMSB;
        strFilterInfo = strSaveFilterInfoMSB;
        packAlgorithm = util_pack8ByteMSBFirst;
    }
    else {
        goto END;
    }
    
    dlgFile = IupFileDlg();
    IupSetAttribute(dlgFile, "ALLOWNEW", "YES");
    IupSetAttribute(dlgFile, "DIALOGTYPE", "SAVE");
    IupSetAttribute(dlgFile, "MULTIPLEFILES", "NO");
    IupSetAttribute(dlgFile, "PARENTDIALOG", ID_DLGMAIN);
    IupSetAttribute(dlgFile, "TITLE", strTitle);
    IupSetAttribute(dlgFile, "FILTER", "*.R");
    IupSetAttribute(dlgFile, "FILTERINFO", strFilterInfo);
    IupPopup(dlgFile, IUP_CENTER, IUP_CENTER);
    
    dlgStatus = IupGetInt(dlgFile, "STATUS");
    if (dlgStatus == 0 || dlgStatus == 1) {
        strSelection = IupGetAttribute(dlgFile, "VALUE");
        strFile = (char *) malloc(strlen(strSelection) + 1 + 3);
        strcpy(strFile, strSelection);
        util_tryIncludeExtension(strFile, "R");
    }
    else {
        goto END;
    }
    
    
    fpRaster = fopen(strFile, "wb");                    // File mode must be BINARY WRITE
    if (fpRaster == 0) {
        dlgMsg = IupMessageDlg();
        IupSetAttribute(dlgMsg, "DIALOGTYPE", "ERROR");
        IupSetAttribute(dlgMsg, "PARENTDIALOG", ID_DLGMAIN);
        IupSetAttribute(dlgMsg, "TITLE", "File Error");
        IupSetfAttribute(dlgMsg, "VALUE", strFileWriteError, strFile);
        IupPopup(dlgMsg, IUP_CENTER, IUP_CENTER);
        IupDestroy(dlgMsg);
        goto END;
    }
    
    
    // Here we are actualy converting our extracted pixel array
    // To ESC\POS Single-Bit Raster Format
    // Algorithm :
    // Pack LSBs of each consecutive 8 bytes into a single 'packed byte'
    // Write each packed byte to the output file ONE BY ONE
    // 8 Pixels = 8 bit value (1 byte)
    
    index = 0;
    byteCount = (unsigned int) ((pHeader->imgWidth / 8) * pHeader->imgHeight);
    while (index < byteCount) {
        packByte = packAlgorithm(pPixelArray, index * 8);               // Pack 8 bits into a single byte
        fwrite((const void *) &packByte, 1, 1, fpRaster);               // Write packed byte to file
        index = index + 1;
    }
    fflush(fpRaster);
    fclose(fpRaster);
    
    // Conversion complete
    
    END:
    IupDestroy(dlgFile);                            // Destroy file selection dialog
    free((void *) strFile);                         // Release occupied memory
    return IUP_DEFAULT;
}



