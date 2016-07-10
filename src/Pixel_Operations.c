

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
#include "Bitmap_Read_Algorithms.h"





#define ID_DLGMAIN          "dlgMAIN"




// Data structure for storing Bitmap Image Info
extern struct DibHeader *pHeader;
extern unsigned char *pPixelArray;


// Pointers to the GUI objects
extern Ihandle *txtOpen;
extern Ihandle *canvas;
extern Ihandle *lblWidth, *lblHeight;
extern Ihandle *lblBpp, *lblColor, *lblCompression;




// Utility function prototypes
void util_addExtensionIfNeeded(char *strFile, char *strExtension);
unsigned char util_pack8ByteMSBfirst(unsigned char *pByteArray, unsigned int startPosition);
unsigned char util_pack8ByteLSBfirst(unsigned char *pByteArray, unsigned int startPosition);


// GUI Callback function prototypes
int cb_btnOpen(Ihandle *ih);
int cb_btnExtract(Ihandle *ih);
int cb_btnFlip(Ihandle *ih);
int cb_btnStoreBinaryPixelArray(Ihandle *ih);
int cb_btnStoreRasterBitImage(Ihandle *ih);
int cb_btnEraseData(Ihandle *ih);




// String constants used in GUI Label objects
static const char *frmt1 = "%d pixels";
static const char *frmt2 = "%d bpp";
static const char *frmt3 = "%d";
static const char *frmt4 = "%d (None)";
static const char *frmt5 = "%d (RLE)";
static const char *strHeaderError
            = "This is not an image file at all";
static const char *strOpenFilter
            = "1 bitPerPixel Monochrome Bitmaps (*.BMP)|*.bmp|All Files (*.*)|*.*|";
static const char *strInvalidBpp
            = "Wrong value : %d bpp\nThis is not a valid 1 bpp Monochrome Bitmap Image";
static const char *strInvalidWidth
            = "Wrong value : %d pixels\nESC\\POS requires that width must be a multiple of 8";








// Implementation of GUI Callback functions


int cb_btnOpen(Ihandle *ih) {
    
    Ihandle *dlgFile;
    char *pStrFile;
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
        pStrFile = IupGetAttribute(dlgFile, "VALUE");
        IupSetStrAttribute(txtOpen, "VALUE", pStrFile);
    }
    IupDestroy(dlgFile);
    return IUP_DEFAULT;
}



int cb_btnExtract(Ihandle *ih) {
    
    char *atrTitle;
    char *pStrFile;
    FILE *fpBitmap;
    Ihandle *dlgMsg;
    struct DibHeader dibHeader;                             // we store image information here
    unsigned char bmpSignature[2];                          // 2 byte file signature
    unsigned char bmpHeader[BMP_HEADER_SIZE];               // 14 byte file header
    unsigned char colPallette[COLOR_PALLETTE_SIZE];         // 8 byte color pallette
    unsigned char *pImageData;                              // arbitrary length raw image data with padding
    
    fpBitmap = 0;
    pImageData = 0;
    atrTitle = "TITLE";
    pStrFile = IupGetAttribute(txtOpen, "VALUE");
    
    if (strlen(pStrFile) == 0) {
        goto END;
    }
    
    // Open selected file for reading a 1bpp Monochrome Bitmap Image
    // File mode must be 'READ IN BINARY'
    fpBitmap = fopen(pStrFile, "rb");
    if (fpBitmap == 0) {
        goto END;
    }
    
    // Read first 2 bytes from opened file to verify file signature
    fread((void *) bmpSignature, 1, 2, fpBitmap);
    if (bmpSignature[0] != BMX1 || bmpSignature[1] != BMX2) {
        dlgMsg = IupMessageDlg();
        IupSetAttribute(dlgMsg, "DIALOGTYPE", "ERROR");
        IupSetAttribute(dlgMsg, "PARENTDIALOG", ID_DLGMAIN);
        IupSetAttribute(dlgMsg, "TITLE", "Header Error");
        IupSetAttribute(dlgMsg, "VALUE", strHeaderError);
        IupPopup(dlgMsg, IUP_CENTER, IUP_CENTER);
        IupDestroy(dlgMsg);
        goto END;
    }
    
    // Rewind opened file to read from the start again
    rewind(fpBitmap);
    fread((void *) bmpHeader, 1, BMP_HEADER_SIZE, fpBitmap);
    
    // Read BITMAPINFOHEADER structure (See MSDN documentation for this C structure)
    bitmap_readDibHeader(fpBitmap, &dibHeader);
    
    free((void *) pHeader);                                         // Erase previous header memory
    free((void *) pPixelArray);                                     // Erase previous pixel memory
    
    // Allocate new memory space for storing Image header info
    pHeader = 0;
    pPixelArray = 0;
    pHeader = (struct DibHeader *) malloc(sizeof(struct DibHeader));
    
    // Data copy from temporary header memory to newly allocated header memory
    memcpy((void *) pHeader, (const void *) &dibHeader, sizeof(struct DibHeader));
    
    // Update GUI objects to reflect new header information
    IupSetfAttribute(lblWidth,  atrTitle, frmt1, pHeader->imgWidth);
    IupSetfAttribute(lblHeight, atrTitle, frmt1, pHeader->imgHeight);
    IupSetfAttribute(lblBpp,    atrTitle, frmt2, pHeader->bitsPerPixel);
    
    if (pHeader->colorCount != 0) {
        IupSetfAttribute(lblColor, atrTitle, frmt3, pHeader->colorCount);
    }
    else {
        IupSetfAttribute(lblColor, atrTitle, frmt4, pHeader->colorCount);
    }
    
    if (pHeader->compression == 0) {
        IupSetfAttribute(lblCompression, atrTitle, frmt4, pHeader->compression);
    }
    else {
        IupSetfAttribute(lblCompression, atrTitle, frmt5, pHeader->compression);
    }
    
    // If this image is not a Monochrome Bitmap (not 1bpp), show an error message
    if (dibHeader.bitsPerPixel != 1) {
        dlgMsg = IupMessageDlg();
        IupSetAttribute(dlgMsg, "DIALOGTYPE", "WARNING");
        IupSetAttribute(dlgMsg, "PARENTDIALOG", ID_DLGMAIN);
        IupSetAttribute(dlgMsg, "TITLE", "Format Error");
        IupSetfAttribute(dlgMsg, "VALUE", strInvalidBpp, dibHeader.bitsPerPixel);
        
        // Refresh GUI object & clear the Image Canvas
        IupRefresh(ih);
        IupUpdate(canvas);
        
        IupPopup(dlgMsg, IUP_CENTER, IUP_CENTER);
        IupDestroy(dlgMsg);
        goto END;
    }
    
    // Allocate memory for storing raw pixel data & refined binary pixel array
    pImageData = (unsigned char *) malloc(pHeader->imageDataLen);
    pPixelArray = (unsigned char *) malloc(pHeader->imgWidth * pHeader->imgHeight);
    
    // Read color pallette data & pixel data from bitmap file
    fread((void *) colPallette, 1, COLOR_PALLETTE_SIZE, fpBitmap);
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
    IupRefresh(ih);
    IupRedraw(lblWidth, 0);
    IupRedraw(lblHeight, 0);
    IupRedraw(lblBpp, 0);
    IupRedraw(lblColor, 0);
    IupRedraw(lblCompression, 0);
    IupUpdate(canvas);
    
    END:
    fclose(fpBitmap);
    free((void *) pImageData);
    return IUP_DEFAULT;
}



int cb_btnFlip(Ihandle *ih) {
    
    int width;
    int row_a, row_b;
    unsigned char *pCopy;
    unsigned char *pRow_a, *pRow_b;
    
    if (pHeader == 0 || pPixelArray == 0) {
        goto END;
    }
    
    pCopy = 0;
    row_a = 0;
    row_b = pHeader->imgHeight - 1;
    width = pHeader->imgWidth;
    pCopy = (unsigned char *) malloc(width);                // Temporary memory used in swap operation
    
    // Reverse Ordering of Rows
    // Simple algorithm implemented by swapping data between row memories
    while (row_a < row_b) {
        pRow_a = pPixelArray + (row_a * width);
        pRow_b = pPixelArray + (row_b * width);
        memcpy((void *) pCopy,      (const void *) pRow_a,      width);
        memcpy((void *) pRow_a,     (const void *) pRow_b,      width);
        memcpy((void *) pRow_b,     (const void *) pCopy,       width);
        row_a = row_a + 1;
        row_b = row_b - 1;
    }
    
    // Update GUI object
    // Draw the inverted pixel array as an image in the grsphic canvas
    IupUpdate(canvas);
    free((void *) pCopy);                               // Erase temporary memory
    
    END:
    return IUP_DEFAULT;
}



int cb_btnEraseData(Ihandle *ih) {
    
    free((void *) pHeader);
    free((void *) pPixelArray);
    pHeader = 0;
    pPixelArray = 0;
    
    IupSetAttribute(lblWidth, "TITLE", 0);
    IupSetAttribute(lblHeight, "TITLE", 0);
    IupSetAttribute(lblBpp, "TITLE", 0);
    IupSetAttribute(lblColor, "TITLE", 0);
    IupSetAttribute(lblCompression, "TITLE", 0);
    
    IupRefresh(ih);
    IupUpdate(canvas);
    return IUP_DEFAULT;
}



int cb_btnStoreBinaryPixelArray(Ihandle *ih) {
    
    FILE *fpBPA;
    Ihandle *dlgFile, *dlgMsg;
    char *strFile, *dlgVal;
    int row, col;
    unsigned char *pPixel;
    int dlgStatus;
    
    dlgFile = 0;
    fpBPA = 0;
    strFile = 0;
    
    if (pHeader == 0 || pPixelArray == 0) {
        goto END;
    }
    
    dlgFile = IupFileDlg();
    IupSetAttribute(dlgFile, "ALLOWNEW", "YES");
    IupSetAttribute(dlgFile, "DIALOGTYPE", "SAVE");
    IupSetAttribute(dlgFile, "MULTIPLEFILES", "NO");
    IupSetAttribute(dlgFile, "PARENTDIALOG", ID_DLGMAIN);
    IupSetAttribute(dlgFile, "TITLE", "Store Pixel Array");
    IupSetAttribute(dlgFile, "FILTER", "*.BPA");
    IupSetAttribute(dlgFile, "FILTERINFO", "Binary Pixel Array (*.BPA)");
    IupPopup(dlgFile, IUP_CENTER, IUP_CENTER);
    
    dlgStatus = IupGetInt(dlgFile, "STATUS");
    if (dlgStatus == 0 || dlgStatus == 1) {
        dlgVal = IupGetAttribute(dlgFile, "VALUE");
        strFile = (char *) malloc(strlen(dlgVal) + 1 + 4);
        strcpy(strFile, dlgVal);
        util_addExtensionIfNeeded(strFile, "BPA");
    }
    else {
        goto END;
    }
    
    fpBPA = fopen(strFile, "wb");
    if (fpBPA == 0) {
        dlgMsg = IupMessageDlg();
        IupSetAttribute(dlgMsg, "DIALOGTYPE", "ERROR");
        IupSetAttribute(dlgMsg, "PARENTDIALOG", ID_DLGMAIN);
        IupSetAttribute(dlgMsg, "TITLE", "File Error");
        IupSetfAttribute(dlgMsg, "VALUE",
            "Error occured while opening the file for writing\n%s", strFile);
        IupPopup(dlgMsg, IUP_CENTER, IUP_CENTER);
        IupDestroy(dlgMsg);
        goto END;
    }
    
    // Write each pixel value as a byte into output file one by one
    // Each Pixel = 8 bit value (1 byte)
    
    for (row = 0; row < pHeader->imgHeight; row++) {
        for (col = 0; col < pHeader->imgWidth; col++) {
            pPixel = pPixelArray + (row * pHeader->imgWidth) + col;
            fwrite((const void *) pPixel, 1, 1, fpBPA);
        }
    }
    fflush(fpBPA);
    fclose(fpBPA);
    
    END:
    IupDestroy(dlgFile);                            // Destroy file selection dialog
    free((void *) strFile);                         // Free occupied memory
    return IUP_DEFAULT;
}



int cb_btnStoreRasterBitImage(Ihandle *ih) {
    
    FILE *fpRBI;
    Ihandle *dlgFile, *dlgMsg;
    char *strFile, *dlgVal;
    unsigned int index;
    unsigned int byteCount;
    unsigned char packByte;
    int dlgStatus;
    
    dlgFile = 0;
    fpRBI = 0;
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
    
    dlgFile = IupFileDlg();
    IupSetAttribute(dlgFile, "ALLOWNEW", "YES");
    IupSetAttribute(dlgFile, "DIALOGTYPE", "SAVE");
    IupSetAttribute(dlgFile, "MULTIPLEFILES", "NO");
    IupSetAttribute(dlgFile, "PARENTDIALOG", ID_DLGMAIN);
    IupSetAttribute(dlgFile, "TITLE", "Store Bit Image");
    IupSetAttribute(dlgFile, "FILTER", "*.RBI");
    IupSetAttribute(dlgFile, "FILTERINFO", "ESC\\POS Raster Bit Image (*.RBI)");
    IupPopup(dlgFile, IUP_CENTER, IUP_CENTER);
    
    dlgStatus = IupGetInt(dlgFile, "STATUS");
    if (dlgStatus == 0 || dlgStatus == 1) {
        dlgVal = IupGetAttribute(dlgFile, "VALUE");
        strFile = (char *) malloc(strlen(dlgVal) + 1 + 5);
        strcpy(strFile, dlgVal);
        util_addExtensionIfNeeded(strFile, "RBI");
    }
    else {
        goto END;
    }
    
    fpRBI = fopen(strFile, "wb");                   // File mode must be binary write
    if (fpRBI == 0) {
        dlgMsg = IupMessageDlg();
        IupSetAttribute(dlgMsg, "DIALOGTYPE", "ERROR");
        IupSetAttribute(dlgMsg, "PARENTDIALOG", ID_DLGMAIN);
        IupSetAttribute(dlgMsg, "TITLE", "File Error");
        IupSetfAttribute(dlgMsg, "VALUE",
            "Error occured while opening the file for writing\n%s", strFile);
        IupPopup(dlgMsg, IUP_CENTER, IUP_CENTER);
        IupDestroy(dlgMsg);
        goto END;
    }
    
    // Pack LSBs from each consecutive 8 bytes into a single packed byte
    // Write each packed byte to the output file ONE BY ONE
    // Order of packing bits into byte is MSB FIRST
    // 8 Pixels = 8 bit value (1 byte)
    
    index = 0;
    byteCount = (unsigned int) ((pHeader->imgWidth / 8) * pHeader->imgHeight);
    while (index < byteCount) {
        packByte = util_pack8ByteMSBfirst(pPixelArray, index * 8);      // Pack 8 bits into a single byte
        fwrite((const void *) &packByte, 1, 1, fpRBI);                  // Write packed byte to file
        index = index + 1;
    }
    fflush(fpRBI);
    fclose(fpRBI);
    
    END:
    IupDestroy(dlgFile);                            // Destroy file selection dialog
    free((void *) strFile);                         // Free occupied memory
    return IUP_DEFAULT;
}



