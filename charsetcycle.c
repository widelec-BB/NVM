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

#include <proto/charsets.h>
#include "globaldefines.h"
#include "support.h"
#include "charsetcycle.h"

extern struct Library *MUIMasterBase, *CharsetsBase, *IntuitionBase;

struct MUI_CustomClass *CharsetCycleClass;
static IPTR CharsetCycleDispatcher(VOID);
const struct EmulLibEntry CharsetCycleGate = {TRAP_LIB, 0, (VOID(*)(VOID))CharsetCycleDispatcher};

struct CharsetCycleData
{
	Object *charsets_list;
};

struct MUI_CustomClass *CreateCharsetCycleClass(VOID)
{
	struct MUI_CustomClass *cl;

	cl = MUI_CreateCustomClass(NULL, MUIC_Cycle, NULL, sizeof(struct CharsetCycleData), (APTR)&CharsetCycleGate);
	CharsetCycleClass = cl;
	return cl;
}

VOID DeleteCharsetCycleClass(VOID)
{
	if (CharsetCycleClass) MUI_DeleteCustomClass(CharsetCycleClass);
}

static IPTR CharsetCycleNew(Class *cl, Object *obj, struct opSet *msg)
{
	Object *ch_list = NewObject(NULL, "charsets.list", TAG_END);

	if(ch_list)
	{
		obj = (Object*)DoSuperNew(cl, obj,
			MUIA_ObjectID, USD_CHARSET_CYCLE,
			MUIA_Cycle_Entries, xget(ch_list, CLSA_NameTable),
		TAG_MORE, (IPTR)msg->ops_AttrList);

		if(obj)
		{
			struct CharsetCycleData *d = INST_DATA(cl, obj);
	
			d->charsets_list = ch_list;

			return (IPTR)obj;
		}
	}
	return (IPTR)NULL;
}

static IPTR CharsetCycleDispose(Class *cl, Object *obj, Msg msg)
{
	struct CharsetCycleData *d = INST_DATA(cl, obj);

	if(d->charsets_list)
		DisposeObject(d->charsets_list);

	return DoSuperMethodA(cl, obj, msg);
}

static IPTR CharsetCycleGet(Class *cl, Object *obj, struct opGet *msg)
{
	struct CharsetCycleData *d = INST_DATA(cl, obj);
	int rv = FALSE;

	switch (msg->opg_AttrID)
	{
		case CHCA_ActiveMib:
			*msg->opg_Storage = GetCharsetNumber(((STRPTR*)xget(d->charsets_list, CLSA_NameTable))[xget(obj, MUIA_Cycle_Active)], CSF_IANA_NAME | CSF_IANA_MIMENAME);
			return TRUE;

		case CHCA_ActiveName:
			*msg->opg_Storage = (IPTR)((STRPTR*)xget(d->charsets_list, CLSA_NameTable))[xget(obj, MUIA_Cycle_Active)];
			return TRUE;

		default: rv = (DoSuperMethodA(cl, obj, (Msg)msg));
	}

	return rv;
}


static IPTR CharsetCycleDispatcher(VOID)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;

	switch (msg->MethodID)
	{
		case OM_NEW:  return (CharsetCycleNew(cl, obj, (struct opSet*)msg));
		case OM_DISPOSE:  return (CharsetCycleDispose(cl, obj, msg));
		case OM_GET: return (CharsetCycleGet(cl, obj, (struct opGet*)msg));
		default:  return (DoSuperMethodA(cl, obj, msg));
	}
}
