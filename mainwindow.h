/*
** Copyright (c) 2012-2014, Filip "widelec" Maryjański
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this
**   list of conditions and the following disclaimer.
**
** * Redistributions in binary form must reproduce the above copyright notice,
**   this list of conditions and the following disclaimer in the documentation
**   and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>

extern struct MUI_CustomClass *MainWindowClass;

struct MUI_CustomClass *CreateMainWindowClass(VOID);
VOID DeleteMainWindowClass(VOID);

#define YT_LIST_URL "video.google.com/timedtext?type=list&v=%s"
#define YT_SUBS_URL "video.google.com/timedtext?type=track&lang=%s&v=%s"


/* 0x7EDA XXXX MainWindowClass */
#define USD_MAIN_WINDOW	0x7EDA0000

/* methods */
#define MWM_ParseLink 		0x7EDA0000
#define MWM_DownloadList 	0x7EDA0001
#define MWM_DownloadSubs	0x7EDA0002
#define MWM_SaveSubs			0x7EDA0003
#define MWM_AskASL			0x7EDA0004

#endif /* __MAINWINDOW_H__ */
