/***************************************************************************
                          cfg.c  -  description
                             -------------------
    begin                : Sun Oct 28 2001
    copyright            : (C) 2001 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

//*************************************************************************//
// History of changes:
//
// 2003/01/04 - Pete
// - added dithering/scale2x options and odd/even game fix
//
// 2002/11/06 - Pete
// - added 2xSai, Super2xSaI, SuperEagle cfg stuff
//
// 2002/09/27 - linuzappz
// - added based on cfg.c for linux gui
//
//*************************************************************************//

#include "stdafx.h"


#define _IN_CFG

#include <gtk/gtk.h>
#undef FALSE
#undef TRUE

#include "externals.h"
#include "cfg.h"
#include "gpu.h"


int            iResX;
int            iResY;
long           GlobalTextAddrX,GlobalTextAddrY,GlobalTextTP;
long           GlobalTextREST,GlobalTextABR,GlobalTextPAGE;
short          ly0,lx0,ly1,lx1,ly2,lx2,ly3,lx3;
long           lLowerpart;
BOOL           bIsFirstFrame;
int            iWinSize;
BOOL           bCheckMask;
unsigned short sSetMask;
unsigned long  lSetMask;
BOOL           bDeviceOK;
short          g_m1;
short          g_m2;
short          g_m3;
short          DrawSemiTrans;
int            iUseGammaVal;
int            iUseScanLines;
int            iUseDither;
int            iDesktopCol;
int            iUseNoStretchBlt;
int            iShowFPS;
int            iFastFwd;


BOOL           bUsingTWin;
TWin_t         TWin;
unsigned long  clutid;
void (*primTableJ[256])(unsigned char *);
void (*primTableSkip[256])(unsigned char *);
unsigned short  usMirror;
unsigned long  dwCfgFixes;
unsigned long  dwActFixes;
int            iUseFixes;
BOOL           bDoVSyncUpdate;
long           drawX;
long           drawY;
long           drawW;
long           drawH;

VRAMLoad_t     VRAMWrite;
VRAMLoad_t     VRAMRead;
DATAREGISTERMODES DataWriteMode;
DATAREGISTERMODES DataReadMode;
int            iColDepth;
int            iWindowMode;
//char           szDispBuf[];
//char           szMenuBuf[];
//char           szDebugText[];
//short          sDispWidths[];
BOOL           bDebugText;
//unsigned int   iMaxDMACommandCounter;
//unsigned long  dwDMAChainStop;
PSXDisplay_t   PSXDisplay;
PSXDisplay_t   PreviousPSXDisplay;
BOOL           bSkipNextFrame;
long           lGPUstatusRet;
long           drawingLines;
unsigned char  * psxVub;
signed char    * psxVsb;
unsigned short * psxVuw;
signed short   * psxVsw;
unsigned long  * psxVul;
signed long    * psxVsl;
BOOL           bChangeWinMode;
long           lSelectedSlot;
BOOL           bInitCap;


unsigned long dwCoreFlags;

unsigned long  ulKeybits;


int            UseFrameLimit;
int            UseFrameSkip;
int            iFrameRate;
int            iFrameLimit;
float          fFrameRateHz;
float          fps_skip;
float          fps_cur;


char * pConfigFile=NULL;

////////////////////////////////////////////////////////////////////////
// gtk linux stuff
////////////////////////////////////////////////////////////////////////

GtkWidget *ConfDlg;
GtkWidget *FullBtn,*WinBtn;
//GtkWidget *StretchBtn;
GtkWidget *ShowFpsBtn,*FpsLimitBtn,*FrameSkipBtn,*AutoFpsBtn;
GtkWidget *ScanLinesBtn,*FixesBtn;
GtkWidget *EditW,*EditH,*EditF;
GtkWidget *ComboModes, *StretchComboModes, *DitherComboModes;
GtkWidget *FixesDlg;
GtkWidget *FixesBtns[32];
GList *Modes;
GList *SModes;
GList *DModes;

void OnCfgFixes_Ok()
{
 int i;

 for (i=0; i<32; i++)
  if (FixesBtns[i] != NULL)
  {
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FixesBtns[i]))) dwCfgFixes |= 1<<i;
   else dwCfgFixes &= ~(1<<i);
  }
 gtk_widget_hide(FixesDlg);
}

void OnCfgFixes_Cancel()
{
 gtk_widget_hide(FixesDlg);
}

#define AddCfgFixBtn(name, desc) \
 BBox = gtk_hbox_new(0, 5); \
 gtk_box_pack_start(GTK_BOX(Box), BBox, FALSE, FALSE, 0); \
 \
 sprintf(Text, "0x%4.4x:", (1<<i)); \
 Label = gtk_label_new(Text); \
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 0); \
 \
 FixesBtns[i] = gtk_check_button_new_with_label(name); \
 gtk_widget_set_usize(FixesBtns[i], 180, 25); \
 gtk_box_pack_start(GTK_BOX(BBox), FixesBtns[i], FALSE, FALSE, 0); \
 \
 Label = gtk_label_new(desc); \
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 0); \
 \
 i++;

void OnCfgFixes()
{
 GtkWidget *DlgPacker;
 GtkWidget *Label;
 GtkWidget *Ok,*Cancel;
 GtkWidget *Box,*BBox;
 int i;
 char Text[32];

 FixesDlg = gtk_window_new(GTK_WINDOW_DIALOG);
// gtk_widget_set_usize(FixesDlg, 500, 400);
 gtk_window_set_title(GTK_WINDOW(FixesDlg), "Special game fixes...");
 gtk_window_set_position(GTK_WINDOW(FixesDlg), GTK_WIN_POS_CENTER);
 gtk_container_set_border_width(GTK_CONTAINER(FixesDlg), 5);

 DlgPacker = gtk_packer_new();
 gtk_container_add(GTK_CONTAINER(FixesDlg), DlgPacker);

 Box = gtk_vbox_new(0, 5);
 gtk_container_set_border_width(GTK_CONTAINER(Box), 5);
 gtk_packer_add(GTK_PACKER(DlgPacker), Box, GTK_SIDE_TOP, GTK_ANCHOR_CENTER,
                GTK_FILL_X, 0, 8, 8, 0, 0);

 Label = gtk_label_new("Activate the following options only if you want to play one of the listed games\n(or if your game is showing similar glitches)!");
 gtk_box_pack_start(GTK_BOX(Box), Label, FALSE, FALSE, 0);

 for (i=0; i<32; i++) FixesBtns[i] = NULL;

 i = 0;
 AddCfgFixBtn("Odd/even bit hack"        , "epsxe 1.5.2 - ChronoCross");
 AddCfgFixBtn("Expand screen width"      , "2D Capcom fighters");
 AddCfgFixBtn("Ignore brightness color"  , "Black screens in Lunar");
 AddCfgFixBtn("Disable coord check"      , "Old compatibility mode");
 AddCfgFixBtn("Unused"                   , "");
 AddCfgFixBtn("Use PC fps calculation"   , "Better fps limitation with some games");
 AddCfgFixBtn("Lazy screen updates"      , "Maybe faster, needed for Pandemonium2");
 AddCfgFixBtn("Use old frame skipping"   , "Skips only every second frame");

 for (i=0; i<32; i++)
  if (FixesBtns[i] != NULL)
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FixesBtns[i]), dwCfgFixes & (1<<i));

 BBox = gtk_hbutton_box_new();
 gtk_packer_add(GTK_PACKER(DlgPacker), BBox, GTK_SIDE_BOTTOM, GTK_ANCHOR_S,
                GTK_FILL_X | GTK_FILL_Y | GTK_EXPAND, 0, 8, 8, 0, 0);

 Ok = gtk_button_new_with_label("Ok");
 gtk_signal_connect(GTK_OBJECT(Ok), "clicked", GTK_SIGNAL_FUNC(OnCfgFixes_Ok), NULL);
 gtk_box_pack_start(GTK_BOX(BBox), Ok, FALSE, FALSE, 0);
 GTK_WIDGET_SET_FLAGS(Ok, GTK_CAN_DEFAULT);

 Cancel = gtk_button_new_with_label("Cancel");
 gtk_signal_connect(GTK_OBJECT(Cancel), "clicked", GTK_SIGNAL_FUNC(OnCfgFixes_Cancel), NULL);
 gtk_box_pack_start(GTK_BOX(BBox), Cancel, FALSE, FALSE, 0);
 GTK_WIDGET_SET_FLAGS(Cancel, GTK_CAN_DEFAULT);

 gtk_widget_show_all(FixesDlg);
}


char szStretchModes[10][32]={"0: Full size stretch","1: No stretching","2: 2xSaI stretching","3: 2xSaI unstretched","4: Super2xSaI stretching","5: Super2xSaI unstretched","6: SuperEagle stretching","7: SuperEagle unstretched","8: Scale2x stretching","9: Scale2x unstretched"};
char szDitherModes[3][32]={"0: No dithering (fastest)","1: Game dependend","2: Always dither"};

void OnDefFast()
{
 char Text[32];

 iResX = 320; iResY = 240;
 if (!iWindowMode)
 {
  sprintf (Text, "%dx%d", iResX, iResY);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ComboModes)->entry), Text);
 }
 sprintf(Text,"%d", iResX);
 gtk_entry_set_text(GTK_ENTRY(EditW), Text);
 sprintf(Text,"%d", iResY);
 gtk_entry_set_text(GTK_ENTRY(EditH), Text);
 sprintf(Text,"%d", 200);
 gtk_entry_set_text(GTK_ENTRY(EditF), Text);

 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FullBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WinBtn), 0);
// gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(StretchBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ShowFpsBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FpsLimitBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FrameSkipBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(AutoFpsBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ScanLinesBtn), 0);

 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(StretchComboModes)->entry),(char *)&szStretchModes[1][0]);
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(DitherComboModes)->entry),(char *)&szDitherModes[0][0]);
}

void OnDefNice()
{
 char Text[32];

 iResX = 640; iResY = 480;
 if (!iWindowMode)
 {
  sprintf (Text, "%dx%d", iResX, iResY);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ComboModes)->entry), Text);
 }
 sprintf(Text,"%d", iResX);
 gtk_entry_set_text(GTK_ENTRY(EditW), Text);
 sprintf(Text,"%d", iResY);
 gtk_entry_set_text(GTK_ENTRY(EditH), Text);
 sprintf(Text,"%d", 200);
 gtk_entry_set_text(GTK_ENTRY(EditF), Text);

 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FullBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WinBtn), 0);
 //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(StretchBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ShowFpsBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FpsLimitBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FrameSkipBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(AutoFpsBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ScanLinesBtn), 0);

 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(StretchComboModes)->entry),(char *)&szStretchModes);
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(DitherComboModes)->entry),(char *)&szDitherModes[1][0]);
}

void OnOk()
{
 char *tmp;

 iWindowMode     = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FullBtn));
// iUseNoStretchBlt = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(StretchBtn));
 iShowFPS        = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ShowFpsBtn));
 UseFrameLimit   = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FpsLimitBtn));
 UseFrameSkip    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FrameSkipBtn));
 iFrameLimit     = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(AutoFpsBtn))+1;
 iUseScanLines   = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ScanLinesBtn));
 iUseFixes       = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FixesBtn));

 iUseNoStretchBlt =atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(StretchComboModes)->entry)));

 iUseDither =atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(DitherComboModes)->entry)));

 tmp = gtk_entry_get_text(GTK_ENTRY(EditW));
 iResX = atoi(tmp);
 tmp = gtk_entry_get_text(GTK_ENTRY(EditH));
 iResY = atoi(tmp);
 if (!iWindowMode)
 {
  tmp = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(ComboModes)->entry));
  sscanf(tmp, "%dx%d", &iResX, &iResY);
 }
 tmp = gtk_entry_get_text(GTK_ENTRY(EditF));
 iFrameRate = atoi(tmp);
 gtk_widget_hide(ConfDlg);//really needed?
 gtk_object_destroy((gpointer)ConfDlg);
/* moved by lu_zero
 gtk_main_quit();
 */
 WriteConfig();
}

void OnCancel()
{
 gtk_widget_hide(ConfDlg);
 gtk_object_destroy((gpointer)ConfDlg);
/* moved by lu_zero
 gtk_main_quit();
 */
}

void SoftDlgProc(void)
{
 GtkWidget *DlgPacker;
 GtkWidget *Label;
 GtkWidget *Ok,*Cancel;
 GtkWidget *Box,*Box1,*Box2,*BBox;
 GtkWidget *Btn,*Frame;
 char Text[32];

 ReadConfig();

 ConfDlg = gtk_window_new(GTK_WINDOW_DIALOG);
 gtk_widget_set_usize(ConfDlg, 400, 405);//375);
#ifndef _SDL
 gtk_window_set_title(GTK_WINDOW(ConfDlg), "Configure the P.E.Op.S. PSX X11 Soft Renderer...");
#else
 gtk_window_set_title(GTK_WINDOW(ConfDlg), "Configure the P.E.Op.S. PSX SDL Soft Renderer...");
#endif
 gtk_window_set_position(GTK_WINDOW(ConfDlg), GTK_WIN_POS_CENTER);
 gtk_container_set_border_width(GTK_CONTAINER(ConfDlg), 5);

 DlgPacker = gtk_packer_new();
 gtk_container_add(GTK_CONTAINER(ConfDlg), DlgPacker);

 Frame = gtk_frame_new("Resolution");
 gtk_packer_add(GTK_PACKER(DlgPacker), Frame, GTK_SIDE_TOP, GTK_ANCHOR_CENTER,
                GTK_FILL_X, 0, 8, 8, 0, 0);

 Box = gtk_vbox_new(0, 2);
 gtk_container_set_border_width(GTK_CONTAINER(Box), 5);
 gtk_container_add(GTK_CONTAINER(Frame), Box);

 Box1 = gtk_hbox_new(0, 5);
 gtk_box_pack_start(GTK_BOX(Box), Box1, FALSE, FALSE, 0);

 FullBtn = gtk_radio_button_new_with_label(NULL, "Fullscreen mode");
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FullBtn), iWindowMode != 1);
 gtk_box_pack_start(GTK_BOX(Box1), FullBtn, FALSE, FALSE, 0);

 Modes = NULL;
 Modes = g_list_append(Modes, "320x240");
 Modes = g_list_append(Modes, "640x480");
 Modes = g_list_append(Modes, "800x600");
 Modes = g_list_append(Modes, "1024x768");

 ComboModes = gtk_combo_new();
 gtk_widget_set_usize(ComboModes, 193, 20);
 gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(ComboModes)->entry), FALSE);
 gtk_combo_set_popdown_strings(GTK_COMBO(ComboModes), Modes);
 if (!iWindowMode)
 {
  sprintf (Text, "%dx%d", iResX, iResY);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ComboModes)->entry), Text);
 }
 gtk_box_pack_start(GTK_BOX(Box1), ComboModes, FALSE, FALSE, 0);

 Box1 = gtk_hbox_new(0, 0);
 gtk_box_pack_start(GTK_BOX(Box), Box1, FALSE, FALSE, 0);

 WinBtn = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(FullBtn)), "Windowed mode");
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WinBtn), iWindowMode != 0);
 gtk_box_pack_start(GTK_BOX(Box1), WinBtn, FALSE, FALSE, 0);

 Label = gtk_label_new("Size:");
 gtk_box_pack_start(GTK_BOX(Box1), Label, FALSE, FALSE, 5);

 EditW = gtk_entry_new();
 sprintf(Text,"%d", iResX);
 gtk_entry_set_text(GTK_ENTRY(EditW), Text);
 gtk_widget_set_usize(EditW, 40, 22);
 gtk_box_pack_start(GTK_BOX(Box1), EditW, FALSE, FALSE, 5);

 Label = gtk_label_new("x");
 gtk_box_pack_start(GTK_BOX(Box1), Label, FALSE, FALSE, 5);

 EditH = gtk_entry_new();
 sprintf(Text,"%d", iResY);
 gtk_entry_set_text(GTK_ENTRY(EditH), Text);
 gtk_widget_set_usize(EditH, 40, 22);
 gtk_box_pack_start(GTK_BOX(Box1), EditH, FALSE, FALSE, 5);

 /////////
 Box1 = gtk_hbox_new(0, 5);
 gtk_box_pack_start(GTK_BOX(Box), Box1, FALSE, FALSE, 0);

 Label = gtk_label_new("Stretching mode:");
 gtk_box_pack_start(GTK_BOX(Box1), Label, FALSE, FALSE, 12);

 SModes = NULL;
 SModes = g_list_append(SModes, (char *)&szStretchModes[0][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[1][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[2][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[3][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[4][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[5][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[6][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[7][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[8][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[9][0]);

 StretchComboModes = gtk_combo_new();
 gtk_widget_set_usize(StretchComboModes, 193, 24);
 gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(StretchComboModes)->entry), FALSE);
 gtk_combo_set_popdown_strings(GTK_COMBO(StretchComboModes), SModes);
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(StretchComboModes)->entry), (char *)&szStretchModes[iUseNoStretchBlt][0]);
 gtk_box_pack_start(GTK_BOX(Box1), StretchComboModes, FALSE, FALSE, 0);

 Box1 = gtk_hbox_new(0, 5);
 gtk_box_pack_start(GTK_BOX(Box), Box1, FALSE, FALSE, 0);

 Label = gtk_label_new("Dithering:");
 gtk_box_pack_start(GTK_BOX(Box1), Label, FALSE, FALSE, 12);

 DModes = NULL;
 DModes = g_list_append(DModes, (char *)&szDitherModes[0][0]);
 DModes = g_list_append(DModes, (char *)&szDitherModes[1][0]);
 DModes = g_list_append(DModes, (char *)&szDitherModes[2][0]);

 DitherComboModes = gtk_combo_new();
 gtk_widget_set_usize(DitherComboModes, 193, 24);
 gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(DitherComboModes)->entry), FALSE);
 gtk_combo_set_popdown_strings(GTK_COMBO(DitherComboModes), DModes);
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(DitherComboModes)->entry), (char *)&szDitherModes[iUseDither][0]);
 gtk_box_pack_start(GTK_BOX(Box1), DitherComboModes, FALSE, FALSE, 40);

// Box1 = gtk_vbox_new(0, 0);
// gtk_box_pack_start(GTK_BOX(Box), Box1, FALSE, FALSE, 0);
// StretchBtn = gtk_check_button_new_with_label("No Stretching");
// gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(StretchBtn), iUseNoStretchBlt);
// gtk_box_pack_start(GTK_BOX(Box1), StretchBtn, FALSE, FALSE, 0);

 Frame = gtk_frame_new("Framerate");
 gtk_packer_add(GTK_PACKER(DlgPacker), Frame, GTK_SIDE_TOP, GTK_ANCHOR_CENTER,
                GTK_FILL_X | GTK_FILL_Y | GTK_EXPAND, 0, 8, 8, 0, 0);

 Box = gtk_vbox_new(0, 2);
 gtk_container_set_border_width(GTK_CONTAINER(Box), 5);
 gtk_container_add(GTK_CONTAINER(Frame), Box);

 Box1 = gtk_vbox_new(0, 0);
 gtk_box_pack_start(GTK_BOX(Box), Box1, FALSE, FALSE, 0);

 ShowFpsBtn = gtk_check_button_new_with_label("Show FPS display on startup");
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(ShowFpsBtn), iShowFPS);
 gtk_box_pack_start(GTK_BOX(Box1), ShowFpsBtn, FALSE, FALSE, 0);

 Box1 = gtk_hbox_new(0, 5);
 gtk_box_pack_start(GTK_BOX(Box), Box1, FALSE, FALSE, 0);

 Box2 = gtk_vbox_new(0, 2);
 gtk_box_pack_start(GTK_BOX(Box1), Box2, FALSE, FALSE, 0);

 FpsLimitBtn = gtk_check_button_new_with_label("Use FPS limit");
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(FpsLimitBtn), UseFrameLimit);
 gtk_box_pack_start(GTK_BOX(Box2), FpsLimitBtn, FALSE, FALSE, 0);

 FrameSkipBtn = gtk_check_button_new_with_label("Use Frame skipping");
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(FrameSkipBtn), UseFrameSkip);
 gtk_box_pack_start(GTK_BOX(Box2), FrameSkipBtn, FALSE, FALSE, 0);

 Box2 = gtk_vbox_new(0, 2);
 gtk_box_pack_start(GTK_BOX(Box1), Box2, FALSE, FALSE, 0);

 AutoFpsBtn = gtk_check_button_new_with_label("Auto-detect FPS/Frame skipping limit");
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(AutoFpsBtn), iFrameLimit-1);
 gtk_box_pack_start(GTK_BOX(Box2), AutoFpsBtn, FALSE, FALSE, 0);

 BBox = gtk_hbox_new(0, 0);
 gtk_box_pack_start(GTK_BOX(Box2), BBox, FALSE, FALSE, 0);

 Label = gtk_label_new("Framerate:");
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 5);

 EditF = gtk_entry_new();
 sprintf(Text,"%d", iFrameRate);
 gtk_entry_set_text(GTK_ENTRY(EditF), Text);
 gtk_box_pack_start(GTK_BOX(BBox), EditF, FALSE, FALSE, 5);
 gtk_widget_set_usize(EditF, 40, 22);

 Label = gtk_label_new("FPS");
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 5);

 Frame = gtk_frame_new("Options");
 gtk_packer_add(GTK_PACKER(DlgPacker), Frame, GTK_SIDE_TOP, GTK_ANCHOR_CENTER,
                GTK_FILL_X, 0, 8, 8, 0, 0);

 Box = gtk_vbox_new(0, 2);
 gtk_container_set_border_width(GTK_CONTAINER(Box), 5);
 gtk_container_add(GTK_CONTAINER(Frame), Box);

 ScanLinesBtn = gtk_check_button_new_with_label("Scanlines enabled");
 gtk_box_pack_start(GTK_BOX(Box), ScanLinesBtn, FALSE, FALSE, 0);
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(ScanLinesBtn), iUseScanLines);

 Box1 = gtk_hbox_new(0, 5);
 gtk_box_pack_start(GTK_BOX(Box), Box1, FALSE, FALSE, 0);

 FixesBtn = gtk_check_button_new_with_label("Activate special games fixes");
 gtk_box_pack_start(GTK_BOX(Box1), FixesBtn, FALSE, FALSE, 0);
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(FixesBtn), iUseFixes);

 Btn = gtk_button_new_with_label("...");
 gtk_signal_connect(GTK_OBJECT(Btn), "clicked", GTK_SIGNAL_FUNC(OnCfgFixes), NULL);
 gtk_box_pack_start(GTK_BOX(Box1), Btn, FALSE, FALSE, 0);

 Frame = gtk_frame_new("Default settings");
 gtk_packer_add(GTK_PACKER(DlgPacker), Frame, GTK_SIDE_LEFT, GTK_ANCHOR_W,
                GTK_FILL_Y | GTK_EXPAND, 0, 8, 8, 0, 0);

 BBox = gtk_hbutton_box_new();
 gtk_container_add(GTK_CONTAINER(Frame), BBox);

 Btn = gtk_button_new_with_label("Fast");
 gtk_signal_connect(GTK_OBJECT(Btn), "clicked", GTK_SIGNAL_FUNC(OnDefFast), NULL);
 gtk_box_pack_start(GTK_BOX(BBox), Btn, FALSE, FALSE, 0);
 GTK_WIDGET_SET_FLAGS(Btn, GTK_CAN_DEFAULT);

 Btn = gtk_button_new_with_label("Nice");
 gtk_signal_connect(GTK_OBJECT(Btn), "clicked", GTK_SIGNAL_FUNC(OnDefNice), NULL);
 gtk_box_pack_start(GTK_BOX(BBox), Btn, FALSE, FALSE, 0);
 GTK_WIDGET_SET_FLAGS(Btn, GTK_CAN_DEFAULT);

 BBox = gtk_hbutton_box_new();
 gtk_packer_add(GTK_PACKER(DlgPacker), BBox, GTK_SIDE_RIGHT, GTK_ANCHOR_S,
                GTK_FILL_Y | GTK_EXPAND, 0, 8, 8, 0, 0);

 Ok = gtk_button_new_with_label("Ok");
 gtk_signal_connect(GTK_OBJECT(Ok), "clicked", GTK_SIGNAL_FUNC(OnOk), NULL);
 gtk_box_pack_start(GTK_BOX(BBox), Ok, FALSE, FALSE, 0);
 GTK_WIDGET_SET_FLAGS(Ok, GTK_CAN_DEFAULT);

 Cancel = gtk_button_new_with_label("Cancel");
 gtk_signal_connect(GTK_OBJECT(Cancel), "clicked", GTK_SIGNAL_FUNC(OnCancel), NULL);
 gtk_box_pack_start(GTK_BOX(BBox), Cancel, FALSE, FALSE, 0);
 GTK_WIDGET_SET_FLAGS(Cancel, GTK_CAN_DEFAULT);

 gtk_widget_show_all(ConfDlg);

//added by lu
 gtk_signal_connect (GTK_OBJECT (ConfDlg), "destroy",
		      GTK_SIGNAL_FUNC(gtk_main_quit),
		      NULL);
 // added by pete:
 gtk_main ();
}
#ifndef _FPSE

GtkWidget *AboutDlg;

void OnAboutOk(GtkMenuItem *menuitem, gpointer userdata)
{
 gtk_widget_destroy(AboutDlg);
}

unsigned char revision;
unsigned char build;
char RELEASE_DATE[256];

void AboutDlgProc(void)
{
 GtkWidget *Label;
 GtkWidget *Ok;
 GtkWidget *Box,*BBox;
 char AboutText[256];

 AboutDlg = gtk_window_new(GTK_WINDOW_DIALOG);
 gtk_widget_set_usize(AboutDlg, 420, 270);
 gtk_window_set_title(GTK_WINDOW(AboutDlg), "About the P.E.Op.S. PSX X11 Soft Renderer...");
 gtk_window_set_position(GTK_WINDOW(AboutDlg), GTK_WIN_POS_CENTER);
 gtk_container_set_border_width(GTK_CONTAINER(AboutDlg), 10);

 Box = gtk_vbox_new(0, 0);
 gtk_container_add(GTK_CONTAINER(AboutDlg), Box);

 BBox = gtk_hbox_new(0, 10);
 gtk_box_pack_start(GTK_BOX(Box), BBox, FALSE, FALSE, 0);

 sprintf(AboutText, "Version:\n"
                    "Release date:\n"
                    "Coded by:\n"
					"Pete's EMail:\n");

 Label = gtk_label_new(AboutText);
 gtk_label_set_justify(GTK_LABEL(Label), GTK_JUSTIFY_LEFT);
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 0);

 sprintf(AboutText, "%d.%d\n"
                    "%s\n"
                    "Pete Bernert and the P.E.Op.S. team\n"
					"BlackDove@addcom.de\n"
                  , revision, build, RELEASE_DATE);

 Label = gtk_label_new(AboutText);
 gtk_label_set_justify(GTK_LABEL(Label), GTK_JUSTIFY_LEFT);
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 0);

 Label = gtk_label_new("The P.E.Op.S. SoftGPU team:\n");
 gtk_misc_set_alignment(GTK_MISC(Label), 0, 0);
 gtk_box_pack_start(GTK_BOX(Box), Label, FALSE, FALSE, 0);

 BBox = gtk_hbox_new(0, 10);
 gtk_box_pack_start(GTK_BOX(Box), BBox, FALSE, FALSE, 0);

 sprintf(AboutText, "P.E.Op.S. page:\n"
					"- Pete Bernert\n"
                    "- Lewpy\n"
                    "- lu_zero\n"
                    "- linuzappz\n"
                    "- syo\n"
                    "- Darko Matesic");

 Label = gtk_label_new(AboutText);
 gtk_label_set_justify(GTK_LABEL(Label), GTK_JUSTIFY_LEFT);
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 0);

 sprintf(AboutText, "https://sourceforge.net/projects/peops\n"
					"http://home.t-online.de/home/PeteBernert\n"
                    "http://lewpy.psxemu.com/\n"
                    "http://brsk.virtualave.net/lu_zero/\n"
                    "http://www.pcsx.net\n"
                    "http://www.geocities.co.jp/SiliconValley-Bay/2072/\n"
                    "http://mrdario.tripod.com");

 Label = gtk_label_new(AboutText);
 gtk_label_set_justify(GTK_LABEL(Label), GTK_JUSTIFY_LEFT);
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 0);

 BBox = gtk_hbutton_box_new();
 gtk_box_pack_start(GTK_BOX(Box), BBox, FALSE, FALSE, 0);

 Ok = gtk_button_new_with_label("Ok");
 gtk_signal_connect(GTK_OBJECT(Ok), "clicked", GTK_SIGNAL_FUNC(OnAboutOk), NULL);
 gtk_container_add(GTK_CONTAINER(BBox), Ok);
 GTK_WIDGET_SET_FLAGS(Ok, GTK_CAN_DEFAULT);

 gtk_widget_show_all(AboutDlg);

 gtk_signal_connect (GTK_OBJECT (AboutDlg), "destroy",
		      GTK_SIGNAL_FUNC(gtk_main_quit),
		      NULL);
 // added by pete:
 gtk_main ();
}


////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>

/* file helpers */

#define GetValue(name, var) \
 p = strstr(pB, name); \
 if (p != NULL) { \
  p+=strlen(name); \
  while ((*p == ' ') || (*p == '=')) p++; \
  if (*p != '\n') var = atoi(p); \
 }

#define SetValue(name, var) \
 p = strstr(pB, name); \
 if (p != NULL) { \
  p+=strlen(name); \
  while ((*p == ' ') || (*p == '=')) p++; \
  if (*p != '\n') { \
   len = sprintf(t1, "%d", var); \
   strncpy(p, t1, len); \
   if (p[len] != ' ' && p[len] != '\n' && p[len] != 0) p[len] = ' '; \
  } \
 } \
 else { \
  size+=sprintf(pB+size, "%s = %d\n", name, var); \
 }

void ReadConfigFile()
{

 struct stat buf;
 FILE *in;char t[256];int len, size;
 char * pB, * p;

 if(pConfigFile) 
      strcpy(t,pConfigFile);
 else 
  {
   strcpy(t,"cfg/gpuPeopsSoftX.cfg");
   in = fopen(t,"rb");
   if (!in)
    {
     strcpy(t,"gpuPeopsSoftX.cfg");
     in = fopen(t,"rb");
     if(!in) sprintf(t,"%s/gpuPeopsSoftX.cfg",getenv("HOME"));
     else    fclose(in);
    }
   else     fclose(in);
  }

 if (stat(t, &buf) == -1) return;
 size = buf.st_size;

 in = fopen(t,"rb");
 if (!in) return;

 pB=(char *)malloc(size);
 memset(pB,0,size);

 len = fread(pB, 1, size, in);
 fclose(in);

 GetValue("ResX", iResX);
 if(iResX<20) iResX=20;
 iResX=(iResX/4)*4;

 GetValue("ResY", iResY);
 if(iResY<20) iResY=20;
 iResY=(iResY/4)*4;

 GetValue("NoStretch", iUseNoStretchBlt);

 GetValue("FullScreen", iWindowMode);
 if(iWindowMode!=0) iWindowMode=0;
 else               iWindowMode=1;

 GetValue("ShowFPS", iShowFPS);
 if(iShowFPS<0) iShowFPS=0;
 if(iShowFPS>1) iShowFPS=1;

 GetValue("ScanLines", iUseScanLines);
 if(iUseScanLines<0) iUseScanLines=0;
 if(iUseScanLines>1) iUseScanLines=1;

 GetValue("UseDither", iUseDither);
 if(iUseDither<0) iUseDither=0;
 if(iUseDither>2) iUseDither=2;

 GetValue("UseFrameLimit", UseFrameLimit);
 if(UseFrameLimit<0) UseFrameLimit=0;
 if(UseFrameLimit>1) UseFrameLimit=1;

 GetValue("UseFrameSkip", UseFrameSkip);
 if(UseFrameSkip<0) UseFrameSkip=0;
 if(UseFrameSkip>1) UseFrameSkip=1;

 GetValue("FPSDetection", iFrameLimit);
 if(iFrameLimit<1) iFrameLimit=1;
 if(iFrameLimit>2) iFrameLimit=2;


 GetValue("FrameRate", iFrameRate);
 if(iFrameRate<10)   iFrameRate=10;
 if(iFrameRate>1000) iFrameRate=1000;

 GetValue("CfgFixes", dwCfgFixes);

 GetValue("UseFixes", iUseFixes);
 if(iUseFixes<0) iUseFixes=0;
 if(iUseFixes>1) iUseFixes=1;

 free(pB);

}

////////////////////////////////////////////////////////////////////////

void ReadConfig(void)
{

 // defaults
 iResX=640;iResY=480;
 iColDepth=16;
 iWindowMode=1;
 iUseScanLines=0;
 iUseDither=0;
 UseFrameLimit=0;
 UseFrameSkip=0;
 iFrameLimit=2;
 iFrameRate=200;
 dwCfgFixes=0;
 iUseFixes=0;
 iUseNoStretchBlt=1;
 iShowFPS=0;

 // read sets
 ReadConfigFile();

 // additional checks
 if(!iColDepth)       iColDepth=32;
 if(iUseFixes)        dwActFixes=dwCfgFixes;
}

void WriteConfig(void) {

 struct stat buf;
 FILE *out;char t[256];int len, size;
 char * pB, * p; char t1[8];

 if(pConfigFile) 
      strcpy(t,pConfigFile);
 else 
  {
   strcpy(t,"cfg/gpuPeopsSoftX.cfg");
   out = fopen(t,"rb");
   if (!out) 
    {
     strcpy(t,"gpuPeopsSoftX.cfg");
     out = fopen(t,"rb");
     if(!out) sprintf(t,"%s/gpuPeopsSoftX.cfg",getenv("HOME"));
     else     fclose(out);
    }
   else     fclose(out);
  }

 if (stat(t, &buf) != -1) size = buf.st_size;
 else size = 0;

 out = fopen(t,"rb");
 if (!out) {
  // defaults
  iResX=640;iResY=480;
  iColDepth=16;
  iWindowMode=1;
  iUseScanLines=0;
  iUseDither=0;
  UseFrameLimit=0;
  UseFrameSkip=0;
  iFrameLimit=2;
  iFrameRate=200;
  dwCfgFixes=0;
  iUseFixes=0;
  iUseNoStretchBlt=1;
  iShowFPS=0;

  size = 0;
  pB=(char *)malloc(4096);
  memset(pB,0,4096);
 }
 else {
  pB=(char *)malloc(size+4096);
  memset(pB,0,size+4096);

  len = fread(pB, 1, size, out);
  fclose(out);
 }

 SetValue("ResX", iResX);
 SetValue("ResY", iResY);
 SetValue("NoStretch", iUseNoStretchBlt);
 SetValue("UseDither", iUseDither);
 SetValue("FullScreen", !iWindowMode);
 SetValue("ShowFPS", iShowFPS);
 SetValue("ScanLines", iUseScanLines);
 SetValue("UseFrameLimit", UseFrameLimit);
 SetValue("UseFrameSkip", UseFrameSkip);
 SetValue("FPSDetection", iFrameLimit);
 SetValue("FrameRate", iFrameRate);
 SetValue("CfgFixes", (unsigned int)dwCfgFixes);
 SetValue("UseFixes", iUseFixes);

 out = fopen(t,"wb");
 if (!out) return;

 len = fwrite(pB, 1, size, out);
 fclose(out);

 free(pB);

}
#endif

int main(int argc, char *argv[]) {
	if (argc < 2) return 0;

	gtk_init(NULL, NULL);

	if (!strcmp(argv[1], "configure")) {
		SoftDlgProc();
	} else {
		revision = atol(argv[2]);
		build = atol(argv[3]);
		strcpy(RELEASE_DATE, argv[4]);
		AboutDlgProc();
	}

	return 0;
}




