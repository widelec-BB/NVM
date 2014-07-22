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
#include <proto/ezxml.h>
#include <libvstring.h>

#include "globaldefines.h"
#include "locale.h"
#include "support.h"
#include "list.h"

extern struct Library *SysBase, *EzxmlBase, *MUIMasterBase;

struct MUI_CustomClass *ListClass;

static IPTR ListDispatcher(VOID);
const struct EmulLibEntry ListGate = {TRAP_LIB, 0, (VOID(*)(VOID))ListDispatcher};

struct YLLP_ParseXML
{
	ULONG MethodID;
	STRPTR xml_data;
};

struct MUI_CustomClass *CreateListClass(VOID)
{
	struct MUI_CustomClass *cl;

	cl = MUI_CreateCustomClass(NULL, MUIC_List, NULL, 0, (APTR)&ListGate);
	ListClass = cl;
	return cl;
}

VOID DeleteListClass(VOID)
{
	if (ListClass) MUI_DeleteCustomClass(ListClass);
}


static IPTR ListNew(Class *cl, Object *obj, struct opSet *msg)
{
	obj = (Object*)DoSuperNew(cl, obj,
							MUIA_Frame, MUIV_Frame_ReadList,
							MUIA_Background, MUII_ReadListBack,
							MUIA_List_Title, TRUE,
							MUIA_List_Format, (IPTR)",,",
	               TAG_MORE, (IPTR)msg->ops_AttrList);

	if(obj)
		return (IPTR)obj;

	return (IPTR)NULL;
}


static IPTR ListDispose(Class *cl, Object *obj, Msg msg)
{
	return DoSuperMethodA(cl, obj, msg);
}

static IPTR ListConstruct(Class *cl, Object *obj, struct MUIP_List_Construct *msg)
{
	struct SubtitlesListEntry *src = msg->entry;
	struct SubtitlesListEntry *new = NULL;

	if(src)
	{
		if((new = AllocPooled(msg->pool, sizeof(struct SubtitlesListEntry))))
		{
			StrCopy(src->lang_code, new->lang_code);
			new->lang_original = StrNew(src->lang_original);
			new->lang_translated = StrNew(src->lang_translated);
		}
	}

	return (IPTR)new;
}

static IPTR ListDestruct(Class *cl, Object *obj, struct MUIP_List_Destruct *msg)
{
	if(msg->entry)
	{
		StrFree(((struct SubtitlesListEntry*)msg->entry)->lang_original);
		StrFree(((struct SubtitlesListEntry*)msg->entry)->lang_translated);
		FreePooled(msg->pool, msg->entry, sizeof(struct SubtitlesListEntry));
	}
	return 0;
}

static IPTR ListDisplay(Class *cl, Object *obj, struct MUIP_List_Display *msg)
{
	struct SubtitlesListEntry *src = msg->entry;

	if(src != NULL)
	{
		msg->array[0] = src->lang_code;
		msg->array[1] = src->lang_translated;
		msg->array[2] = src->lang_original;
	}
	else
	{
		msg->array[0] = GetString(MSG_LIST_TITLE_LANGCODE);
		msg->array[1] = GetString(MSG_LIST_TITLE_LANGTRANS);
		msg->array[2] = GetString(MSG_LIST_TITLE_LANGORIG);
	}

	return (IPTR)0;
}

static IPTR ListParseXML(Class *cl, Object *obj, struct YLLP_ParseXML *msg)
{
	if(msg->xml_data)
	{
		ezxml_t xml;

		if((xml = ezxml_parse_str(msg->xml_data, StrLen(msg->xml_data))))
		{
			ezxml_t track = ezxml_child(xml, "track");

			if(track)
			{
				do
				{
					struct SubtitlesListEntry en;

					StrCopy((STRPTR)ezxml_attr(track, "lang_code"), en.lang_code);
					en.lang_original = StrNewUTF8ToSystem((STRPTR)ezxml_attr(track, "lang_original"));
					en.lang_translated = StrNewUTF8ToSystem((STRPTR)ezxml_attr(track, "lang_translated"));

					DoMethod(obj, MUIM_List_InsertSingle, (IPTR)&en, MUIV_List_Insert_Bottom);

					FreeVec(en.lang_original);
					FreeVec(en.lang_translated);

				}while((track = ezxml_next(track)));
			}

			if(xget(obj, MUIA_List_Entries) == 0)
				MUI_RequestA(_app(obj), obj, 0x00, NULL, GetString(MSG_NO_SUBTITLES_BUTTONS), GetString(MSG_NO_SUBTITLES), NULL);

			ezxml_free(xml);
		}
	}

	return (IPTR)0;
}

static IPTR ListDispatcher(VOID)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;

	switch (msg->MethodID)
	{
		case OM_NEW:  return (ListNew(cl, obj, (struct opSet*)msg));
		case OM_DISPOSE:  return (ListDispose(cl, obj, msg));
		case MUIM_List_Construct: return (ListConstruct(cl, obj, (struct MUIP_List_Construct*)msg));
		case MUIM_List_Destruct: return (ListDestruct(cl, obj, (struct MUIP_List_Destruct*)msg));
		case MUIM_List_Display: return (ListDisplay(cl, obj, (struct MUIP_List_Display*)msg));
		case YLLM_ParseXML: return(ListParseXML(cl, obj, (struct YLLP_ParseXML*)msg));

		default:  return (DoSuperMethodA(cl, obj, msg));
	}
}
