

/****************************************************************************************
    Implementation of Bit-Packing Algorithms and other Utility Functions
    Author:             Ashis Kumar Das
    Email:              akd.bracu@gmail.com
    GitHub:             https://github.com/AKD92
*****************************************************************************************/






#include <string.h>







// Contains Bit-Packing Algorithms implemented

void util_addExtensionIfNeeded(char *strFile, char *strExtension);

unsigned char util_pack8ByteMSBfirst(unsigned char *pByteArray, unsigned int startPosition);

unsigned char util_pack8ByteLSBfirst(unsigned char *pByteArray, unsigned int startPosition);








void util_addExtensionIfNeeded(char *strFile, char *strExtension) {
    
    int index;
    char cFile, cExt;
    unsigned int extLen, fileLen;
    
    if (strFile == 0 || strExtension == 0)
        return;
    
    fileLen = strlen(strFile);
    extLen = strlen(strExtension);
    
    for (index = 1; index <= extLen; index++) {
        cFile = *(strFile + fileLen - index);
        cExt = *(strExtension + extLen - index);
        if (cFile != cExt) {
            goto ADDEXT;
        }
    }
    goto END;
    
    ADDEXT:
    strcat(strFile, ".");
    strcat(strFile, strExtension);
    
    END:
    return;
}



unsigned char util_pack8ByteMSBfirst(unsigned char *pByteArray, unsigned int startPosition) {
    
    unsigned int index;
    unsigned char xByte;
    unsigned char bPackByte;
    unsigned char *pStartAddress;
    
    index = 0;
    bPackByte = 0x00;
    pStartAddress = pByteArray + startPosition;         // Calculate memory address of first byte
    
    while (index < 8) {
        xByte = *(pStartAddress + index);
        xByte = xByte & 0x01;
        bPackByte = bPackByte << 1;                     // Make room for inserting new bit
        bPackByte = bPackByte | xByte;                  // Bitwise OR for positioning the bit
        index = index + 1;
    }
    return bPackByte;
}



unsigned char util_pack8ByteLSBfirst(unsigned char *pByteArray, unsigned int startPosition) {
    
    unsigned int index;
    unsigned char xByte;
    unsigned char bPackByte;
    unsigned char *pStartAddress;
    
    index = 0;
    bPackByte = 0x00;
    pStartAddress = pByteArray + startPosition;         // Calculate memory address of first byte
    
    while (index < 8) {
        xByte = *(pStartAddress + index);
        xByte = (xByte << 7) & 0x80;                    // Shift up LSB to MSB by 7 bits
        bPackByte = bPackByte >> 1;                     // Make room for inserting new bit
        bPackByte = bPackByte | xByte;                  // Bitwise OR for positioning the bit
        index = index + 1;
    }
    return bPackByte;
}



