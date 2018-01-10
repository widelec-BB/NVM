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
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <libvstring.h>
#include <proto/charsets.h>
#include "support.h"
#include "globaldefines.h"

extern struct Library *CharsetsBase, *MultimediaBase, *MUIMasterBase, *IntuitionBase, *SysBase;

Object* StringLabel(STRPTR label, STRPTR preparse)
{
	Object *obj = MUI_NewObject(MUIC_Text,
						MUIA_FramePhantomHoriz, TRUE,
						MUIA_Frame, MUIV_Frame_String,
						MUIA_Text_PreParse, (IPTR)preparse,
						MUIA_Text_Contents, (IPTR)label,
						MUIA_HorizWeight, 0,
					TAG_END);

	return obj;
}

Object* StringGadget(ULONG id)
{
	Object *obj = MUI_NewObject(MUIC_String,
					MUIA_UserData, id,
					MUIA_ObjectID, id,
					MUIA_Frame, MUIV_Frame_String,
					MUIA_Background, MUII_StringBack,
					MUIA_CycleChain, TRUE,
					MUIA_String_AdvanceOnCR, TRUE,
				TAG_END);

	return obj;
}

Object* GfxButton(ULONG id, STRPTR pic, UBYTE control)
{
	Object *obj = MUI_NewObject(MUIC_Group,
		MUIA_InputMode, MUIV_InputMode_RelVerify,
		MUIA_UserData, id,
		MUIA_ObjectID, id,
		MUIA_ControlChar, control,
		MUIA_Group_Child, (IPTR)MUI_NewObject(MUIC_Dtpic,
			MUIA_Dtpic_Name, (IPTR)pic,
		TAG_END),
	TAG_END);

	return obj;
}

Object* NormalButton(STRPTR label, UBYTE control, LONG objid, ULONG weight)
{
	Object *obj;

	obj = MUI_NewObject(MUIC_Text,
		MUIA_Text_Contents, (IPTR)label,
		MUIA_Text_PreParse, (IPTR)"\33c",
		MUIA_Frame, MUIV_Frame_Button,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Font, MUIV_Font_Button,
		MUIA_InputMode, MUIV_InputMode_RelVerify,
		MUIA_Text_HiChar, control,
		MUIA_ControlChar, control,
		MUIA_CycleChain, TRUE,
		MUIA_HorizWeight, weight,
		MUIA_UserData, objid,
	TAG_END);

	return obj;
}

STRPTR StrIStr(STRPTR txt, STRPTR tmp) /* brute force case insensitive replacement for strstr() */
{
	STRPTR temp = tmp;
	BYTE a, b;

	while(*txt != 0x00 && *temp != 0x00)
	{
		if(_between('A', *txt, 'Z'))
			a = *txt | 0x20;
		else
			a = *txt;
		if(_between('A', *temp, 'Z'))
			b = *temp | 0x20;
		else
			b = *temp;

		if(a != b)
		{
			txt -= temp - tmp;
			temp = tmp - 1 ;
		}
		txt++;
		temp++;
	}

	if(*temp == 0x00)
		return txt - (temp - tmp);
	else
		return NULL;
}

STRPTR StrNewLen(STRPTR s, LONG len)
{
	STRPTR result = NULL;

	if(s != NULL && len > 0)
	{
		if((result = AllocVec(len + 1, MEMF_ANY)))
		{
			LONG i;

			for(i=0; i < len; i++)
				result[i] = s[i];

			result[len] = 0x00;
		}
	}
	
	return result;
}

STRPTR StrNewUTF8Recode(STRPTR s, ULONG mib)
{
	STRPTR result = NULL;

	if(s)
	{
		ULONG buf_len = GetByteSize(s, -1, MIBENUM_UTF_8, mib);
		
		if((result = AllocVec(buf_len, MEMF_ANY)))
			ConvertTags(s, -1, result, buf_len, MIBENUM_UTF_8, mib, TAG_END);
	}	

	return result;
}
