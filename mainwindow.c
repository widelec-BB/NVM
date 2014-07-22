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

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/ezxml.h>
#include <libvstring.h>

#include <stdlib.h>
#include <math.h>

#include "globaldefines.h"
#include "support.h"
#include "locale.h"
#include "list.h"
#include "application.h"
#include "mainwindow.h"
#include "charsetcycle.h"

extern struct Library *SysBase, *DOSBase, *EzxmlBase, *IntuitionBase, *MUIMasterBase, *IconBase;

struct MUI_CustomClass *MainWindowClass;
static IPTR MainWindowDispatcher(VOID);
const struct EmulLibEntry MainWindowGate = {TRAP_LIB, 0, (VOID(*)(VOID))MainWindowDispatcher};

struct MWP_SaveSubs
{
	ULONG MethodID;
	STRPTR location;
	STRPTR xml_subs;
};

struct MWP_AskASL
{
	ULONG MethodID;
	STRPTR xml_subs;
};

struct MainWindowData
{
	Object *url_string, *fetch_list_button, *subs_list, *download_button, *charset_cycle;
	STRPTR movie_id;
};

struct MUI_CustomClass *CreateMainWindowClass(VOID)
{
	struct MUI_CustomClass *cl;

	cl = MUI_CreateCustomClass(NULL, MUIC_Window, NULL, sizeof(struct MainWindowData), (APTR)&MainWindowGate);
	MainWindowClass = cl;
	return cl;
}

VOID DeleteMainWindowClass(VOID)
{
	if (MainWindowClass) MUI_DeleteCustomClass(MainWindowClass);
}

static IPTR MainWindowNew(Class *cl, Object *obj, struct opSet *msg)
{
	Object *url_string, *fetch_list_button, *subs_list, *download_button, *charset_cycle;

	obj = (Object*)DoSuperNew(cl, obj,
		MUIA_Window_ID, USD_MAIN_WINDOW,
		MUIA_Window_Title, (IPTR)APP_NAME,
		MUIA_Window_ScreenTitle, (IPTR)APP_NAME,
		MUIA_Window_RootObject, (IPTR)MUI_NewObject(MUIC_Group,
			MUIA_Group_Child, (IPTR)MUI_NewObject(MUIC_Group,
				MUIA_Group_Horiz, TRUE,
				MUIA_Group_Child, (IPTR)StringLabel(GetString(MSG_LINK_STRING_LABEL), "\33l"),
				MUIA_Group_Child, (IPTR)(url_string = StringGadget(0)),
				MUIA_Group_Child, (IPTR)(fetch_list_button = NormalButton(GetString(MSG_DOWNLOAD_LIST_BUTTON), *GetString(MSG_DOWNLOAD_LIST_BUTTON_HOTKEY), 0, 10)),
			TAG_END),

			MUIA_Group_Child, (IPTR)(subs_list = NewObject(ListClass->mcc_Class, NULL, TAG_END)),

			MUIA_Group_Child, (IPTR)MUI_NewObject(MUIC_Group,
				MUIA_Group_Horiz, TRUE,
				MUIA_Group_Child, (IPTR)StringLabel(GetString(MSG_CHARSET_SELECTION), "\33r"),
				MUIA_Group_Child, (IPTR)(charset_cycle = NewObject(CharsetCycleClass->mcc_Class, NULL, TAG_END)),
			TAG_END),

			MUIA_Group_Child, (IPTR)MUI_NewObject(MUIC_Group,
				MUIA_Group_Horiz, TRUE,
				MUIA_Group_Child, (IPTR)EmptyRectangle(50),
				MUIA_Group_Child, (IPTR)(download_button = NormalButton(GetString(MSG_DOWNLOAD_SUBTITLES_BUTTON), *GetString(MSG_DOWNLOAD_SUBTITLES_BUTTON_HOTKEY), 0, 50)),
				MUIA_Group_Child, (IPTR)EmptyRectangle(50),
			TAG_END),
		TAG_END),
	TAG_MORE, (IPTR)msg->ops_AttrList);

	if(obj)
	{
		struct MainWindowData *d = INST_DATA(cl, obj);

		d->url_string = url_string;
		d->fetch_list_button = fetch_list_button;
		d->subs_list = subs_list;
		d->download_button = download_button;
		d->charset_cycle = charset_cycle;

		set(d->download_button, MUIA_Disabled, TRUE);

		DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 3,
		 MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		DoMethod(d->fetch_list_button, MUIM_Notify, MUIA_Pressed, FALSE, (IPTR)obj, 1,
		 MWM_DownloadList);

		DoMethod(d->subs_list, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, (IPTR)d->download_button, 3,
		 MUIM_Set, MUIA_Disabled, FALSE);

		DoMethod(d->download_button, MUIM_Notify, MUIA_Pressed, FALSE, (IPTR)obj, 1,
		 MWM_DownloadSubs);

		return (IPTR)obj;
	}
	return (IPTR)NULL;
}


static IPTR MainWindowDispose(Class *cl, Object *obj, Msg msg)
{
	return DoSuperMethodA(cl, obj, msg);
}

static IPTR MainWindowParseLink(Class *cl, Object *obj)
{
	struct MainWindowData *d = INST_DATA(cl, obj);
	STRPTR url = (STRPTR)xget(d->url_string, MUIA_String_Contents);
	STRPTR id_start = NULL, id_end = NULL;

	if(url && StrIStr(url, "youtube.com"))
	{
		while(*url != 0x00)
		{
			if(*url == 'v' && *(url+1) == '=')
			{
				id_start = (url+2);
				break;
			}
			url++;
		}

		if(id_start)
		{
			while(*url != 0x00 && *url != '&')
				url++;

			id_end = url;

			if(!(d->movie_id = StrNewLen(id_start, id_end - id_start)))
				MUI_RequestA(_app(obj), obj, 0x00, NULL, GetString(MSG_WRONG_URL_BUTTONS), GetString(MSG_WRONG_URL), NULL);


			tprintf("MOVIE ID: %s\n", d->movie_id);

		}
		else
			MUI_RequestA(_app(obj), obj, 0x00, NULL, GetString(MSG_WRONG_URL_BUTTONS), GetString(MSG_WRONG_URL), NULL);
	}
	else
		MUI_RequestA(_app(obj), obj, 0x00, NULL, GetString(MSG_WRONG_URL_BUTTONS), GetString(MSG_WRONG_URL), NULL);


	return (IPTR)d->movie_id;
}

static IPTR MainWindowDownloadList(Class *cl, Object *obj)
{
	struct MainWindowData *d = INST_DATA(cl, obj);
	STRPTR xml_url;

	DoMethod(d->subs_list, MUIM_List_Clear);

	DoMethod(obj, MWM_ParseLink);

	if(d->movie_id && (xml_url = FmtNew(YT_LIST_URL, d->movie_id)))
	{
		UBYTE *xml_list;

		if((xml_list = (UBYTE*)DoMethod(_app(obj), APPM_FetchXML, (IPTR)xml_url)))
		{
			tprintf("%s\n", xml_list);

			DoMethod(d->subs_list, YLLM_ParseXML, (IPTR)xml_list);

			FreeVec(xml_list);
		}
		FmtFree(xml_url);
	}
	else
		tprintf("out of memory\n");

	return (IPTR)0;
}

static IPTR MainWindowDownloadSubs(Class *cl, Object *obj)
{
	struct MainWindowData *d = INST_DATA(cl, obj);
	struct SubtitlesListEntry *entry;
	STRPTR xml_url;

	DoMethod(d->subs_list, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, (IPTR)&entry);

	if(d->movie_id && entry)
	{
		if((xml_url = FmtNew(YT_SUBS_URL, entry->lang_code, d->movie_id)))
		{
			UBYTE *xml_subs;

			if((xml_subs = (UBYTE*)DoMethod(_app(obj), APPM_FetchXML, (IPTR)xml_url)))
			{
				tprintf("%s\n", xml_subs);

				DoMethod(obj, MWM_AskASL, (IPTR)xml_subs);
				
				FreeVec(xml_subs);
			}
			FmtFree(xml_url);
		}
	}

	return (IPTR)0;
}

static IPTR MainWindowAskASL(Class *cl, Object *obj, struct MWP_AskASL *msg)
{
	struct FileRequester *freq;

	set(_app(obj), MUIA_Application_Sleep, TRUE);

	if((freq = MUI_AllocAslRequestTags(ASL_FileRequest, TAG_END)))
	{
		if(MUI_AslRequestTags(freq,
			ASLFR_Window, (IPTR)_window(obj),
			ASLFR_TitleText, (IPTR)GetString(MSG_SUBSSAVE_ASL_TITLE),
			ASLFR_PositiveText, (IPTR)GetString(MSG_SUBSSAVE_ASL_POSITIVE),
			ASLFR_InitialPattern, (IPTR)"#?.srt",
			ASLFR_DoPatterns, TRUE,
			ASLFR_RejectIcons, TRUE,
			ASLFR_DoSaveMode, TRUE,
			TAG_END))
		{
			UBYTE location[500];

			StrNCopy(freq->fr_Drawer, (STRPTR)location, 500);
			AddPart((STRPTR)location, freq->fr_File, 500);

			tprintf("FILENAME: %s\n", freq->fr_File);
			if(freq->fr_File && !StrIStr(freq->fr_File, ".srt"))
			{
				tprintf("ADDING\n");
				StrCat(".srt", location);
			}
			tprintf("save subs to file: %s\n", location);

			DoMethod(obj, MWM_SaveSubs, (IPTR)location, (IPTR)msg->xml_subs);
		}
	}

	set(_app(obj), MUIA_Application_Sleep, FALSE);

	return (IPTR)0;

}

static IPTR MainWindowSaveSubs(Class *cl, Object *obj, struct MWP_SaveSubs *msg)
{
	struct MainWindowData *d = INST_DATA(cl, obj);

	if(msg->location && msg->xml_subs)
	{
		ezxml_t subs;

		if((subs = ezxml_parse_str(msg->xml_subs, StrLen(msg->xml_subs))))
		{
			ezxml_t text = ezxml_child(subs, "text");

			if(text)
			{
				BPTR fh;
				ULONG count = 1;
				UBYTE buffer[30] = {0};

				if((fh = Open(msg->location, MODE_NEWFILE)))
				{
					ULONG dst_mib = xget(d->charset_cycle, CHCA_ActiveMib);

					tprintf("Selected Mibenum: %ld\n", dst_mib);

					if(dst_mib == MIBENUM_INVALID)
						dst_mib = MIBENUM_SYSTEM;

					do
					{
						DOUBLE time_start = 0.0, time_end = 0.0;
						ULONG h_s, m_s, s_s, h_e, m_e, s_e;
						ULONG e_s, e_e;
						STRPTR start, dur, txt;
						
						FPrintf(fh, "%ld\n", count++);

						if((start = (STRPTR) ezxml_attr(text, "start")))
						{
							LONG a, b;
							
							if(StrToLong(start, &a) == -1)
							{
								tprintf("Failed to read start a!\n");
								continue;
							}
							
							while(*start && *start != '.' && *start != '"')
								start++;
						
							if(*start == '.')
							{
								LONG len, i;
								
								if((len = StrToLong(start + 1, &b)) == -1)
								{
									tprintf("Failed to read start b!\n");
									continue;
								}
								
								time_start = (DOUBLE)b; 
								
								for(i = 0; i < len; i++)
									time_start /= 10.0;
							}
							time_start += (DOUBLE)a;
						}
						if((dur = (STRPTR) ezxml_attr(text, "dur")))
						{
							LONG a, b;
							
							if(StrToLong(dur, &a) == -1)
							{
								tprintf("Failed to read duration a\n");
								continue;
							}
							
							while(*dur && *dur != '.' && *dur != '"')
								dur++;
						
							if(*dur == '.')
							{
								LONG len, i;
								
								if((len = StrToLong(dur + 1, &b)) == -1)
								{
									tprintf("failed to read duration b\n");
									continue;
								}

								time_end = (DOUBLE)b;
								
								for(i = 0; i < len; i++)
									time_end /= 10.0;
							}
							time_end += (DOUBLE)a;
							time_end += time_start;
						}
						
						txt = StrNewUTF8Recode(ezxml_txt(text), dst_mib);

						h_s = ((ULONG)time_start)/3600;
						m_s = (((ULONG)time_start)%3600)/60;
						s_s = ((ULONG)time_start)%60;
						e_s = (ULONG)((time_start - ((ULONG)time_start))*1000);

						h_e = ((ULONG)time_end)/3600;
						m_e = (((ULONG)time_end)%3600)/60;
						s_e = ((ULONG)time_end)%60;
						e_e = (ULONG)((time_end - ((ULONG)time_end))*1000);

						FmtNPut(buffer, "%.2ld:%.2ld:%.2ld,%.3ld --> %.2ld:%.2ld:%.2ld,%.3ld", sizeof(buffer),
						 h_s, m_s, s_s, e_s, h_e, m_e, s_e, e_e);

						FPrintf(fh, "%s\n", (IPTR)buffer);
						FPrintf(fh, "%s\n\n\n", (IPTR)txt);

						FreeVec(txt);
					}while((text = ezxml_next(text)));

					Close(fh);
				}
			}
		}
	}
	return (IPTR)0;
}

static IPTR MainWindowDispatcher(VOID)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;

	switch (msg->MethodID)
	{
		case OM_NEW:  return (MainWindowNew(cl, obj, (struct opSet*)msg));
		case OM_DISPOSE:  return (MainWindowDispose(cl, obj, msg));
		case MWM_ParseLink: return (MainWindowParseLink(cl, obj));
		case MWM_DownloadList: return (MainWindowDownloadList(cl, obj));
		case MWM_DownloadSubs: return (MainWindowDownloadSubs(cl, obj));
		case MWM_AskASL: return (MainWindowAskASL(cl, obj, (struct MWP_AskASL*)msg));
		case MWM_SaveSubs: return (MainWindowSaveSubs(cl, obj, (struct MWP_SaveSubs*)msg));
		default:  return (DoSuperMethodA(cl, obj, msg));
	}
}
