

/****************************************************************************************
    Implementation of Primary Window GUI Interface of SVM Pixel Extraction Utility
    Author:             Ashis Kumar Das
    Email:              akd.bracu@gmail.com
    GitHub:             https://github.com/AKD92
*****************************************************************************************/






#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iup.h>
#include <cd.h>
#include <cdiup.h>
#include <cddbuf.h>
#include "Bitmap_Read_Algorithms.h"





#define ID_CD_SCREEN            "cd_canvas"
#define ID_CD_DBUF              "cd_doubleBufferedCanvas"
#define ID_DLGMAIN              "dlgMAIN"





Ihandle *txtOpen, *btnOpen;
Ihandle *canvas;
Ihandle *btnStoreRBI;
Ihandle *btnExtract, *btnFlip, *btnStore;
Ihandle *btnEraseData;
Ihandle *lblWidth, *lblHeight, *lblBpp, *lblColor;
Ihandle *lblCompression;


struct DibHeader *pHeader;
unsigned char *pPixelArray;


static const char *strTitle = "Pixel Extraction Utility (The SVM Project)";




Ihandle * createMainDialog(void);

int cb_btnOpen(Ihandle *ih);
int cb_btnExtract(Ihandle *ih);
int cb_btnFlip(Ihandle *ih);
int cb_btnStoreBinaryPixelArray(Ihandle *ih);
int cb_btnStoreRasterBitImage(Ihandle *ih);
int cb_btnEraseData(Ihandle *ih);

int cv_map(Ihandle *ih);
int cv_unmap(Ihandle *ih);
int cv_resize(Ihandle *ih);
int cv_draw(Ihandle *ih, float posx, float posy);





Ihandle *createMainDialog(void) {
    
    Ihandle *dlgMain;
    Ihandle *boxOpen, *boxLowerside, *boxWhole;
    Ihandle *frmPreview, *boxPreview;
    Ihandle *frmControls, *boxControls;
    Ihandle *lbl1, *lbl2, *lbl3, *lbl4, *lbl5;
    Ihandle *boxlbl1, *boxlbl2, *boxlbl3;
    
    txtOpen = IupText(0);
    IupSetAttribute(txtOpen, "EXPAND", "YES");
    IupSetAttribute(txtOpen, "MINSIZE", "x30");
    IupSetAttribute(txtOpen, "MULTILINE", "NO");
    IupSetAttribute(txtOpen, "READONLY", "YES");
    
    btnOpen = IupButton("Open...", 0);
    IupSetAttribute(btnOpen, "RASTERSIZE", "100x30");
    IupSetCallback(btnOpen, "ACTION", (Icallback) cb_btnOpen);
    
    boxOpen = IupVbox(txtOpen, btnOpen, 0);
    IupSetAttribute(boxOpen, "ALIGNMENT", "ARIGHT");
    IupSetAttribute(boxOpen, "EXPAND", "HORIZONTAL");
    IupSetAttribute(boxOpen, "NGAP", "5");
    IupSetAttribute(boxOpen, "NMARGIN", "0x0");
    
    btnExtract = IupButton("Extract Pixel Array", 0);
    IupSetAttribute(btnExtract, "RASTERSIZE", "180x35");
    IupSetCallback(btnExtract, "ACTION", (Icallback) cb_btnExtract);
    
    btnFlip = IupButton("Invert Pixel Array", 0);
    IupSetAttribute(btnFlip, "RASTERSIZE", "180x35");
    IupSetCallback(btnFlip, "ACTION", (Icallback) cb_btnFlip);
    
    btnStore = IupButton("Store As\nBinary Pixel Array", 0);
    IupSetAttribute(btnStore, "RASTERSIZE", "180x40");
    IupSetCallback(btnStore, "ACTION", (Icallback) cb_btnStoreBinaryPixelArray);
    
    btnStoreRBI = IupButton("Store As\nESC\\POS Raster Bit Image", 0);
    IupSetAttribute(btnStoreRBI, "RASTERSIZE", "180x40");
    IupSetCallback(btnStoreRBI, "ACTION", (Icallback) cb_btnStoreRasterBitImage);
    
    btnEraseData = IupButton("Erase Data Structures", 0);
    IupSetAttribute(btnEraseData, "RASTERSIZE", "180x35");
    IupSetCallback(btnEraseData, "ACTION", (Icallback) cb_btnEraseData);
    
    lbl1 = IupLabel("Width :");
    lbl2 = IupLabel("Height :");
    lbl3 = IupLabel("Bits Per Pixel :");
    lbl4 = IupLabel("Compression :");
    lbl5 = IupLabel("Pallette Count :");
    
    lblWidth = IupLabel(0);
//  IupSetAttribute(lblWidth, "ALIGNMENT", "ACENTER:ACENTER");
    lblHeight = IupLabel(0);
//  IupSetAttribute(lblHeight, "ALIGNMENT", "ACENTER:ACENTER");
    lblBpp = IupLabel(0);
//  IupSetAttribute(lblBpp, "ALIGNMENT", "ACENTER:ACENTER");
    lblColor = IupLabel(0);
//  IupSetAttribute(lblColor, "ALIGNMENT", "ACENTER:ACENTER");
    lblCompression = IupLabel(0);
    
    boxlbl1 = IupVbox(lbl1, lbl2, lbl3, lbl4, lbl5, 0);
    IupSetAttribute(boxlbl1, "NGAP", "5");
    IupSetAttribute(boxlbl1, "NMARGIN", "0x0");
    IupSetAttribute(boxlbl1, "EXPAND", "VERTICAL");
    
    boxlbl2 = IupVbox(lblWidth,
                        lblHeight,
                        lblBpp,
                        lblCompression,
                        lblColor, 0);
    IupSetAttribute(boxlbl2, "NGAP", "5");
    IupSetAttribute(boxlbl2, "NMARGIN", "0x0");
    
    boxlbl3 = IupHbox(boxlbl1, boxlbl2, 0);
    IupSetAttribute(boxlbl3, "NGAP", "5");
    IupSetAttribute(boxlbl3, "NMARGIN", "0x0");
    IupSetAttribute(boxlbl3, "EXPAND", "VERTICAL");
    
    boxControls = IupVbox(boxlbl3,
                            IupFill(),
                            btnExtract,
                            btnFlip,
                            btnStore,
                            btnStoreRBI,
                            btnEraseData, 0);
    IupSetAttribute(boxControls, "EXPAND", "VERTICAL");
    IupSetAttribute(boxControls, "NGAP", "5");
    IupSetAttribute(boxControls, "NMARGIN", "10x10");
    
    frmControls = IupFrame(boxControls);
    IupSetAttribute(frmControls, "TITLE", "Controls");
    IupSetAttribute(frmControls, "EXPAND", "VERTICAL");
    
    canvas = IupCanvas(0);
    IupSetAttribute(canvas, "BORDER", "NO");
    IupSetAttribute(canvas, "EXPAND", "YES");
    IupSetCallback(canvas, "MAP_CB", (Icallback) cv_map);
    IupSetCallback(canvas, "UNMAP_CB", (Icallback) cv_unmap);
    IupSetCallback(canvas, "RESIZE_CB", (Icallback) cv_resize);
    IupSetCallback(canvas, "ACTION", (Icallback) cv_draw);
    
    boxPreview = IupHbox(canvas, 0);
    IupSetAttribute(boxPreview, "NMARGIN", "10x10");
    IupSetAttribute(boxPreview, "EXPAND", "YES");
    
    frmPreview = IupFrame(boxPreview);
    IupSetAttribute(frmPreview, "TITLE", "Binary Image Preview");
    
    boxLowerside = IupHbox(frmPreview, frmControls, 0);
    IupSetAttribute(boxLowerside, "EXPAND", "YES");
    IupSetAttribute(boxLowerside, "NGAP", "10");
    IupSetAttribute(boxLowerside, "NMARGIN", "0x0");
    
    boxWhole = IupVbox(boxOpen, boxLowerside, 0);
    IupSetAttribute(boxWhole, "NMARGIN", "10x10");
    IupSetAttribute(boxWhole, "NGAP", "5");
    IupSetAttribute(boxWhole, "EXPAND", "YES");
    
    dlgMain = IupDialog(boxWhole);
    IupSetAttribute(dlgMain, "SHRINK", "YES");
    IupSetAttribute(dlgMain, "TITLE", strTitle);
    IupSetAttribute(dlgMain, "ICON", "A");
    IupSetHandle(ID_DLGMAIN, dlgMain);
    return dlgMain;
}



int cv_map(Ihandle *ih) {
    
    cdCanvas *cd;
    cdCanvas *db;
    
    cd = cdCreateCanvas(CD_IUP, canvas);
    db = cdCreateCanvas(CD_DBUFFER, cd);
    
    cdCanvasBackground(cd, CD_YELLOW);
    cdCanvasBackground(db, CD_YELLOW);
    cdCanvasForeground(cd, CD_WHITE);
    cdCanvasForeground(db, CD_WHITE);
    IupSetAttribute(canvas, ID_CD_SCREEN, (char *) cd);
    IupSetAttribute(canvas, ID_CD_DBUF, (char *) db);
    return IUP_DEFAULT;
}


int cv_unmap(Ihandle *ih) {
    
    cdCanvas *cd;
    cdCanvas *db;
    
    cd = (cdCanvas *) IupGetAttribute(canvas, ID_CD_SCREEN);
    db = (cdCanvas *) IupGetAttribute(canvas, ID_CD_DBUF);
    
    cdKillCanvas(db);
    cdKillCanvas(cd);
    return IUP_DEFAULT;
}


int cv_resize(Ihandle *ih) {
    
    cdCanvas *db;
    
    db = (cdCanvas *) IupGetAttribute(canvas, ID_CD_DBUF);
    cdCanvasActivate(db);
    return IUP_DEFAULT;
}



int cv_draw(Ihandle *ih, float posx, float posy) {
    
    cdCanvas *db;
    auto int xstart, ystart;
    auto int cv_width, cv_height;
    unsigned char cPixelValue;
    auto int rowIndex;
    
    int dy;
    register int dx;
    register int row, col;
    
    db = (cdCanvas *) IupGetAttribute(canvas, ID_CD_DBUF);
    cdCanvasActivate(db);
    cdCanvasGetSize(db, &cv_width, &cv_height, 0, 0);
    cdCanvasClear(db);
    cdCanvasFlush(db);
    
    if (pPixelArray == 0 || pHeader == 0) {
        goto END;
    }
    
    xstart = (cv_width - pHeader->imgWidth) / 2;
    ystart = (cv_height - pHeader->imgHeight) / 2;
    
    cdCanvasBox(db,
                xstart, xstart + pHeader->imgWidth - 1,
                ystart, ystart + pHeader->imgHeight - 1);
                
//  cdCanvasFlush(db);
    
    for (row = 0; row < pHeader->imgHeight; row++) {
        dy = row + ystart;
        cdCanvasUpdateYAxis(db, &dy);
        rowIndex = (row * pHeader->imgWidth);
        
        for (col = 0; col < pHeader->imgWidth; col++) {
            dx = col + xstart;
            cPixelValue = *(pPixelArray + rowIndex + col);
            if (cPixelValue == 0x01) {
                cdCanvasPixel(db, dx, dy, CD_BLACK);
            }
        }
    }
    cdCanvasFlush(db);
    
    END:
    return IUP_DEFAULT;
}



//int cv_draw(Ihandle *ih, float posx, float posy) {
//  
//  cdCanvas *db;
//  int xstart, ystart;
//  int cv_width, cv_height;
//  unsigned char cPixelValue;
//  long int pixelColor;
//  long int rowIndex;
//  
//  int dy;
//  register int dx;
//  register int row, col;
//  
//  db = (cdCanvas *) IupGetAttribute(canvas, ID_CD_DBUF);
//  cdCanvasActivate(db);
//  cdCanvasGetSize(db, &cv_width, &cv_height, 0, 0);
//  cdCanvasClear(db);
//  cdCanvasFlush(db);
//  
//  if (pPixelArray == 0 || pHeader == 0) {
//      goto END;
//  }
//  
//  xstart = (cv_width - pHeader->imgWidth) / 2;
//  ystart = (cv_height - pHeader->imgHeight) / 2;
//  
//  for (row = 0; row < pHeader->imgHeight; row++) {
//      rowIndex = (row * pHeader->imgWidth);
//      for (col = 0; col < pHeader->imgWidth; col++) {
//          dx = col + xstart;
//          dy = row + ystart;
//          cdCanvasUpdateYAxis(db, &dy);
//          cPixelValue = *(pPixelArray + rowIndex + col);
//          if (cPixelValue == 0x00) {
//              pixelColor = CD_WHITE;
//          }
//          else if (cPixelValue == 0x01) {
//              pixelColor = CD_BLACK;
//          }
//          else {
//              pixelColor = CD_GREEN;
//          }
//          cdCanvasPixel(db, dx, dy, pixelColor);
//      }
//  }
//  cdCanvasFlush(db);
//  
//  END:
//  return IUP_DEFAULT;
//}



