

/****************************************************************************************
    Implementation of Driver Function (main) : The Entrypoint of the Program
    Author:             Ashis Kumar Das
    Email:              akd.bracu@gmail.com
    GitHub:             https://github.com/AKD92
*****************************************************************************************/






#include <iup.h>
#include <stdio.h>
#include <stdlib.h>
#include "Bitmap_Read_Algorithms.h"








// Just execute the program

int main(void);

Ihandle * createMainDialog(void);






int main(void) {
    
    auto Ihandle *dlgMain;
    extern struct DibHeader *pHeader;
    extern unsigned char *pPixelArray;
    
    pHeader = 0;
    pPixelArray = 0;
    
    IupOpen(0, 0);
        IupSetGlobal("DEFAULTFONT", "System, Bold -10");
        dlgMain = createMainDialog();
        IupSetAttribute(dlgMain, "RASTERSIZE", "680x515");
        IupShowXY(dlgMain, IUP_CENTER, IUP_CENTER);
        IupMainLoop();
        IupDestroy(dlgMain);
    IupClose();
    
    free((void *) pHeader);                                         // Destroy data structure memories
    free((void *) pPixelArray);
    
    return 0;                                                       // Return control back to host OS
}



