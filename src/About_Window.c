


/****************************************************************************************
    Implementation of About Window GUI Interface of SVM Pixel Extraction Utility
    Author:             Ashis Kumar Das
    Email:              akd.bracu@gmail.com
    GitHub:             https://github.com/AKD92
*****************************************************************************************/






#include <iup.h>
#include <stdlib.h>






#define BTN_CLOSE                   "btnClose"
#define ID_DLGMAIN                  "MainDialog"                  // Primary Dialog Window






static const char *strProjName
                    = "Pixel Extraction Utility Program (SVM PE)";
                    
static const char *strProjDesc
                    = "Convert pixels from 1bpp Monochrome Bitmaps\n"
                        "To ESC\\POS compatible Single-Bit Rasters\n"
                        "Utilizing MSB/LSB First Bit-Packing Algorithm";
                        
static const char *strProjSource
                    = "Produced Rasters are directly printable by\n"
                        "Thermal printers with SVM Thermal Device Driver";
                    
static const char *strProjCompiler
                    = "Compiled using TDM-GCC TDragon Compiler\n"
                        "Version 5.1.0, Revision 3";
                    
static const char *strProjProgrammer
                    = "Programmed by : Ashis Kumar Das";
                    
static const char *strProjDept
                    = "Department of Engineering && Computer Science\n"
                        "BRAC University";
                        
static const char *strProjEmail
                    = "mailto:akd.bracu@gmail.com";
                    
static const char *strProjSrcLink
                    = "https://github.com/AKD92/SVM-PE-Pixel-Extraction-Utility-Program";





int cb_ShowAboutWindow(void);
static int cb_btnClose(Ihandle *btn);







Ihandle *createAboutDialog(void) {
	
	Ihandle *lblProjName, *lblDesc, *lblArduino, *lblCompiler;
	Ihandle *lblAuthor, *lblDepartment, *lblEmail, *lblProjSource;
	Ihandle *lblSeparator;
	Ihandle *vbox1, *vbox2;
	Ihandle *vboxContainer, *bottomHbox;
	Ihandle *vboxWhole;
	Ihandle *btnClose, *dlgAbout;
	
	lblProjName = IupLabel(strProjName);
	lblDesc = IupLabel(strProjDesc);
	lblArduino = IupLabel(strProjSource);
	lblCompiler = IupLabel(strProjCompiler);
	lblAuthor = IupLabel(strProjProgrammer);
	lblDepartment = IupLabel(strProjDept);
	lblEmail = IupLink(strProjEmail, "akd.bracu@gmail.com");
	lblProjSource = IupLink(strProjSrcLink, "Source code on my GitHub account");
	lblSeparator = IupLabel(0);
	IupSetAttribute(lblSeparator, "SEPARATOR", "HORIZONTAL");
	IupSetAttribute(lblProjName, "ALIGNMENT", "ACENTER");
	IupSetAttribute(lblDesc, "ALIGNMENT", "ACENTER");
	IupSetAttribute(lblArduino, "ALIGNMENT", "ACENTER");
	IupSetAttribute(lblCompiler, "ALIGNMENT", "ACENTER");
	IupSetAttribute(lblAuthor, "ALIGNMENT", "ACENTER");
	IupSetAttribute(lblDepartment, "ALIGNMENT", "ACENTER");
	IupSetAttribute(lblEmail, "ALIGNMENT", "ACENTER");
	IupSetAttribute(lblProjSource, "ALIGNMENT", "ACENTER");
	
	vbox1 = IupVbox(lblProjName, lblDesc, lblArduino, lblCompiler, 0);
	IupSetAttribute(vbox1, "EXPANDCHILDREN", "YES");
	IupSetAttribute(vbox1, "NGAP", "12");
	IupSetAttribute(vbox1, "NMARGIN", "0x0");
	
	vbox2 = IupVbox(lblAuthor, lblDepartment, lblEmail, lblProjSource, 0);
	IupSetAttribute(vbox2, "EXPANDCHILDREN", "YES");
	IupSetAttribute(vbox2, "NGAP", "5");
	IupSetAttribute(vbox2, "NMARGIN", "0x0");
	
	vboxContainer = IupVbox(IupFill(), vbox1, lblSeparator, vbox2, IupFill(), 0);
	IupSetAttribute(vboxContainer, "NGAP", "10");
	IupSetAttribute(vboxContainer, "NMARGIN", "20x20");
	
	btnClose = IupButton("Close", 0);
	IupSetAttribute(btnClose, "RASTERSIZE", "100x35");
	IupSetCallback(btnClose, "ACTION", (Icallback) cb_btnClose);
	
	bottomHbox = IupHbox(IupFill(), btnClose, 0);
	vboxWhole = IupVbox(vboxContainer, bottomHbox, 0);
	IupSetAttribute(vboxWhole, "NGAP", "10");
	IupSetAttribute(vboxWhole, "NMARGIN", "10x10");
	
	IupSetHandle(BTN_CLOSE, btnClose);
	
	dlgAbout = IupDialog(vboxWhole);
	IupSetAttribute(dlgAbout, "SHRINK", "YES");
	IupSetAttribute(dlgAbout, "PARENTDIALOG", ID_DLGMAIN);
	IupSetAttribute(dlgAbout, "TITLE", "About SVM PE");
	IupSetAttribute(dlgAbout, "DEFAULTESC", BTN_CLOSE);
	IupSetAttribute(dlgAbout, "ICON", "A");
	
	return dlgAbout;
}





int cb_ShowAboutWindow(void) {
    
    Ihandle *dlgAbout;
    
    dlgAbout = createAboutDialog();
    IupSetAttribute(dlgAbout, "RASTERSIZE", "430x460");
    IupPopup(dlgAbout, IUP_CENTER, IUP_CENTER);
    IupDestroy(dlgAbout);
    
    return IUP_DEFAULT;
}




static int cb_btnClose(Ihandle *btn) {
	
	return IUP_CLOSE;
}




