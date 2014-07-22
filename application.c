/*
** Copyright (c) 2012-2014, Filip "widelec" Maryja≈Ñski
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
#include <libraries/gadtools.h>
#include <classes/multimedia/streams.h>
#include <mui/Aboutbox_mcc.h>
#include <proto/icon.h>

#include "globaldefines.h"
#include "locale.h"
#include "mainwindow.h"
#include "application.h"

extern struct Library *SysBase, *IntuitionBase, *MUIMasterBase, *IconBase;

struct MUI_CustomClass *ApplicationClass;

static IPTR ApplicationDispatcher(VOID);
const struct EmulLibEntry ApplicationGate = {TRAP_LIB, 0, (VOID(*)(VOID))ApplicationDispatcher};

struct ApplicationData
{
	struct DiskObject *icon;

	Object *main_window;
};

struct APPP_FetchXML
{
	ULONG MethodID;
	STRPTR url;
};

struct MUI_CustomClass *CreateApplicationClass(VOID)
{
	struct MUI_CustomClass *cl;

	cl = MUI_CreateCustomClass(NULL, MUIC_Application, NULL, sizeof(struct ApplicationData), (APTR)&ApplicationGate);
	ApplicationClass = cl;
	return cl;
}

VOID DeleteApplicationClass(VOID)
{
	if (ApplicationClass) MUI_DeleteCustomClass(ApplicationClass);
}

static IPTR ApplicationNew(Class *cl, Object *obj, struct opSet *msg)
{
	Object *about_window = MUI_NewObject(MUIC_Aboutbox, MUIA_Aboutbox_Credits, (IPTR)APP_ABOUT, TAG_END);
	Object *main_window;
	Object *menu_about, *menu_quit, *menu_mui_prefs, *menu_mui_about;
	static STRPTR used_classes[] = {NULL};

	obj = (Object*)DoSuperNew(cl, obj,
	                 MUIA_Application_Author, (IPTR)APP_AUTHOR,
	                 MUIA_Application_Base, (IPTR)APP_BASE,
	                 MUIA_Application_Copyright, (IPTR)"©" APP_CYEARS " " "BlaBla group",
	                 MUIA_Application_Description, (IPTR)APP_DESC,
	                 MUIA_Application_Title, (IPTR)APP_NAME,
	                 MUIA_Application_Version, (IPTR)APP_VER,
						  MUIA_Application_UsedClasses, (IPTR)used_classes,
						  MUIA_Application_Menustrip, (IPTR)MUI_NewObject(MUIC_Menustrip,
							MUIA_Group_Child, (IPTR)MUI_NewObject(MUIC_Menu,
								MUIA_Menu_Title, (IPTR)GetString(MSG_MENUSTRIP_APP_NAME),
								MUIA_Group_Child, (IPTR)(menu_about = MUI_NewObject(MUIC_Menuitem,
									MUIA_Menuitem_Title, (IPTR)GetString(MSG_MENUSTRIP_ABOUT),
									MUIA_Menuitem_Shortcut, (IPTR)"?", 
								TAG_END)),
								MUIA_Group_Child, (IPTR)(menu_mui_about = MUI_NewObject(MUIC_Menuitem,
									MUIA_Menuitem_Title, (IPTR)GetString(MSG_MENUSTRIP_MUI_ABOUT),
								TAG_END)),
								
								MUIA_Group_Child, (IPTR)MUI_NewObject(MUIC_Menuitem,
									MUIA_Menuitem_Title, (IPTR)NM_BARLABEL,
								TAG_END),
			
								MUIA_Group_Child, (IPTR)(menu_quit = MUI_NewObject(MUIC_Menuitem,
									MUIA_Menuitem_Title, (IPTR)GetString(MSG_MENUSTRIP_QUIT),
									MUIA_Menuitem_Shortcut, (IPTR)"Q",
								TAG_END)),
							TAG_END),
							MUIA_Group_Child, (IPTR)MUI_NewObject(MUIC_Menu,
								MUIA_Menu_Title, (IPTR)GetString(MSG_MENUSTRIP_SETTINGS),
								MUIA_Group_Child, (IPTR)(menu_mui_prefs = MUI_NewObject(MUIC_Menuitem,
									MUIA_Menuitem_Title, (IPTR)GetString(MSG_MENUSTRIP_MUI_PREFS),
								TAG_END)),
							TAG_END),
						  TAG_END),
	                 MUIA_Application_Window, (IPTR)about_window,
	                 MUIA_Application_Window, (IPTR)(main_window = NewObject(MainWindowClass->mcc_Class, NULL, TAG_END)),
	               TAG_MORE, (IPTR)msg->ops_AttrList);

	if(obj)
	{
		struct ApplicationData *d = INST_DATA(cl, obj);

		d->main_window = main_window;

		if((d->icon = GetDiskObject("PROGDIR:NVM")))
			set(obj, MUIA_Application_DiskObject, (IPTR)d->icon);

		DoMethod(menu_about, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (IPTR)about_window, 3,
		 MUIM_Set, MUIA_Window_Open, TRUE);

		DoMethod(menu_mui_about, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (IPTR)obj, 2,
		 MUIM_Application_AboutMUI, (IPTR)d->main_window);

		DoMethod(menu_mui_prefs, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (IPTR)obj, 3,
		 MUIM_Application_OpenConfigWindow, 0, 0);

		DoMethod(menu_quit, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (IPTR)obj, 2,
		 MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		return (IPTR)obj;
	}

	tprintf("application create failed with window == %p\n", main_window);

	return (IPTR)NULL;
}


static IPTR ApplicationDispose(Class *cl, Object *obj, Msg msg)
{
	struct ApplicationData *d = INST_DATA(cl, obj);

	if(d->icon)
		FreeDiskObject(d->icon);

	return DoSuperMethodA(cl, obj, msg);
}

static IPTR ApplicationMainLoop(Class *cl, Object *obj)
{
	struct ApplicationData *d = INST_DATA(cl, obj);
	ULONG sigs = 0;

	set(d->main_window, MUIA_Window_Open, TRUE);

	while(DoMethod(obj, MUIM_Application_NewInput, (IPTR)&sigs) != (ULONG)MUIV_Application_ReturnID_Quit)
	{
		if(sigs)
		{
			sigs = Wait(sigs | SIGBREAKF_CTRL_C);
			if (sigs & SIGBREAKF_CTRL_C) break;
		}
	}

	set(d->main_window, MUIA_Window_Open, FALSE);

	return (IPTR)0;
}

static IPTR ApplicationFetchXML(Class *cl, Object *obj, struct APPP_FetchXML *msg)
{
	struct ApplicationData *d = INST_DATA(cl, obj);
	UBYTE *result = NULL;

	if(msg->url)
	{
		Object *http;

		tprintf("Download url: %s\n", msg->url);

		http = NewObject(NULL, "http.stream",
		 MMA_StreamName, (IPTR)msg->url,
		TAG_END);

		if(http)
		{
			QUAD buffer_size = MediaGetPort64(http, 0, MMA_StreamLength);

			tprintf("Http: buffer size == %Ld\n", (LONG)(buffer_size >> 32), (LONG)(buffer_size & 0xFFFFFFFF));

			if(buffer_size == 0)
				buffer_size = 1024*1024;

			if((buffer_size < 2147483648LL) && (result = (UBYTE*)AllocVec((LONG)buffer_size, MEMF_ANY | MEMF_CLEAR)))
			{
				LONG data_len;

				data_len = DoMethod(http, MMM_Pull, 0, (IPTR)result, (LONG)buffer_size);

				tprintf("data_len == %ld\n", data_len);
			}

			DisposeObject(http);
		}
		else
			MUI_RequestA(obj, d->main_window, 0x00, NULL, GetString(MSG_CANNOT_CONNECT_BUTTONS), GetString(MSG_CANNOT_CONNECT), NULL);
	}

	return (IPTR)result;
}

static IPTR ApplicationDispatcher(VOID)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;

	switch (msg->MethodID)
	{
		case OM_NEW:  return (ApplicationNew(cl, obj, (struct opSet*)msg));
		case OM_DISPOSE:  return (ApplicationDispose(cl, obj, msg));
		case APPM_MainLoop: return (ApplicationMainLoop(cl, obj));
		case APPM_FetchXML: return (ApplicationFetchXML(cl, obj, (struct APPP_FetchXML*)msg));
		default:  return (DoSuperMethodA(cl, obj, msg));
	}
}
