/*
** Copyright (c) 2012-2014, Filip "widelec" Maryjanski
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

#include <proto/exec.h>
#include <proto/ezxml.h>
#include <proto/charsets.h>
#include <proto/multimedia.h>
#include <proto/intuition.h>
#include <libvstring.h>

#include "globaldefines.h"
#include "locale.h"
#include "application.h"
#include "list.h"
#include "mainwindow.h"
#include "charsetcycle.h"

extern struct Library *SysBase, *DOSBase;

struct Library *MUIMasterBase, *CharsetsBase, *EzxmlBase, *MultimediaBase, *HttpStreamBase, *IntuitionBase, *IconBase;

BOOL startApp(VOID)
{
	BYTE error[128];
	if(!(MUIMasterBase = OpenLibrary("muimaster.library", 20)))
	{
		PutStr("Cannot open muimaster.library (V20)\n");
		FmtNPut((STRPTR)error, GetString(MSG_OPEN_LIBRARY_FAILED), 128, "muimaster.library (V20)");
		MUI_Request(NULL, NULL, NULL, APP_NAME, "*_OK", (STRPTR)error, NULL);
		return FALSE;
	}
	if(!(CharsetsBase = OpenLibrary("charsets.library", 53)))
	{
		PutStr("Cannot open charsets.library (V53)!\n");
		FmtNPut((STRPTR)error, GetString(MSG_OPEN_LIBRARY_FAILED), 128, "charsets.library (V53)");
		MUI_Request(NULL, NULL, NULL, APP_NAME, "*_OK", (STRPTR)error, NULL);
		return FALSE;
	}
	if(!(EzxmlBase = OpenLibrary("ezxml.library", 8)))
	{
		PutStr("Cannot open ezxml.library (V8)!\n");
		FmtNPut((STRPTR)error, GetString(MSG_OPEN_LIBRARY_FAILED), 128, "ezxml.library (V8)");
		MUI_Request(NULL, NULL, NULL, APP_NAME, "*_OK", (STRPTR)error, NULL);
		return FALSE;
	}
	if(!(MultimediaBase = OpenLibrary("multimedia/multimedia.class", 53)))
	{
		PutStr("Cannot open multimedia.class (V53)!\n");
		FmtNPut((STRPTR)error, GetString(MSG_OPEN_LIBRARY_FAILED), 128, "multimedia.class (V53)");
		MUI_Request(NULL, NULL, NULL, APP_NAME, "*_OK", (STRPTR)error, NULL);
		return FALSE;
	}
	if(!(HttpStreamBase = OpenLibrary("multimedia/http.stream", 51)))
	{
		PutStr("Cannot open http.stream (V51)!\n");
		FmtNPut((STRPTR)error, GetString(MSG_OPEN_LIBRARY_FAILED), 128, "http.stream (V51)");
		MUI_Request(NULL, NULL, NULL, APP_NAME, "*_OK", (STRPTR)error, NULL);
		return FALSE;
	}
	if(!(IntuitionBase = OpenLibrary("intuition.library", 0)))
	{
		PutStr("Cannot open intuition.library (V51)!\n");
		FmtNPut((STRPTR)error, GetString(MSG_OPEN_LIBRARY_FAILED), 128, "intuition.library(V51)");
		MUI_Request(NULL, NULL, NULL, APP_NAME, "*_OK", (STRPTR)error, NULL);
		return FALSE;
	}
	if(!(IconBase = OpenLibrary("icon.library", 41)))
	{
		PutStr("Cannot open icon.library (V51)!\n");
		FmtNPut((STRPTR)error, GetString(MSG_OPEN_LIBRARY_FAILED), 128, "icon.library (V51)");
		MUI_Request(NULL, NULL, NULL, APP_NAME, "*_OK", (STRPTR)error, NULL);
		return FALSE;
	}

	Locale_Open("NVM.catalog", 1, 0);

	return TRUE;
}

VOID closeApp(VOID)
{
	if(MUIMasterBase) CloseLibrary(MUIMasterBase);
	if(CharsetsBase) CloseLibrary(CharsetsBase);
	if(EzxmlBase) CloseLibrary(EzxmlBase);
	if(MultimediaBase) CloseLibrary(MultimediaBase);
	if(HttpStreamBase) CloseLibrary(HttpStreamBase);
	if(IntuitionBase) CloseLibrary(IntuitionBase);
	if(IconBase) CloseLibrary(IconBase);

	Locale_Close();
	return;
}


ULONG Main(VOID)
{
	if(startApp())
	{
		if(CreateApplicationClass())
		{
			tprintf("ApplicationClass created\n");
			if(CreateListClass())
			{
				if(CreateMainWindowClass())
				{
					tprintf("MainWindowClass created\n");
					if(CreateCharsetCycleClass())
					{
						Object *app;

						tprintf("CharsetCycleClass created\n");

						if((app = NewObject(ApplicationClass->mcc_Class, NULL, TAG_END)))
						{
							tprintf("App obj == %p\n", app);

							DoMethod(app, MUIM_Application_Load, (IPTR)MUIV_Application_Load_ENV);

							DoMethod(app, APPM_MainLoop);

							DoMethod(app, MUIM_Application_Save, (IPTR)MUIV_Application_Save_ENV);
							DoMethod(app, MUIM_Application_Save, (IPTR)MUIV_Application_Save_ENVARC);

							MUI_DisposeObject(app);
						}
						DeleteCharsetCycleClass();
					}
					DeleteMainWindowClass();
				}
				DeleteListClass();
			}
			DeleteApplicationClass();
		}
		closeApp();
	}

	return 0;
}