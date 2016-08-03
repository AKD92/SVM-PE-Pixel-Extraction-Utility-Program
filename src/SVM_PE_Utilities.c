

/****************************************************************************************
    Implementation of Bit-Packing Algorithms and other Utility Functions
    Author:             Ashis Kumar Das
    Email:              akd.bracu@gmail.com
    GitHub:             https://github.com/AKD92
*****************************************************************************************/






#include <string.h>







// Definitions Bit-Packing Algorithms

void util_tryIncludeExtension(char *strFile, const char *strExtension);

unsigned char util_pack8ByteMSBFirst(unsigned char *pByteArray, unsigned int startPosition);

unsigned char util_pack8ByteLSBFirst(unsigned char *pByteArray, unsigned int startPosition);








void util_tryIncludeExtension(char *strFile, const char *strExtension) {
    
    unsigned int index;
    char cFile, cExten;
    unsigned int extenLen, fileLen;
    
    if (strFile == 0 || strExtension == 0) {
        goto END;
    }
    
    fileLen = strlen(strFile);
    extenLen = strlen(strExtension);
    
    if (extenLen == 0) {
        goto END;
    }
    if (fileLen < extenLen) {
        goto ADD_EXTENSION;
    }
    
    for (index = 1; index <= extenLen; index++) {
        cFile = *(strFile + fileLen - index);
        cExten = *(strExtension + extenLen - index);
        if (cFile != cExten) {
            goto ADD_EXTENSION;
        }
    }
    goto END;
    
    ADD_EXTENSION:
    strcat(strFile, ".");
    strcat(strFile, strExtension);
    
    END:
    return;
}



unsigned char util_pack8ByteMSBFirst(unsigned char *pByteArray, unsigned int startPosition) {
    
    unsigned int index;
    unsigned char dataByte;
    unsigned char packByte;
    unsigned char *pStartAddress;
    
    index = 0;
    packByte = 0x00;
    pStartAddress = pByteArray + startPosition;             // Calculate memory address of first byte
    
    while (index < 8) {
        dataByte = *(pStartAddress + index);
        dataByte = dataByte & 0x01;
        packByte = packByte << 1;                           // Make room for inserting new bit
        packByte = packByte | dataByte;                     // Bitwise OR for positioning the bit
        index = index + 1;
    }
    return packByte;
}



unsigned char util_pack8ByteLSBFirst(unsigned char *pByteArray, unsigned int startPosition) {
    
    unsigned int index;
    unsigned char dataByte;
    unsigned char packByte;
    unsigned char *pStartAddress;
    
    index = 0;
    packByte = 0x00;
    pStartAddress = pByteArray + startPosition;             // Calculate memory address of first byte
    
    while (index < 8) {
        dataByte = *(pStartAddress + index);
        dataByte = (dataByte << 7) & 0x80;                  // Shift up LSB to MSB by 7 bits
        packByte = packByte >> 1;                           // Make room for inserting new bit
        packByte = packByte | dataByte;                     // Bitwise OR for positioning the bit
        index = index + 1;
    }
    return packByte;
}



