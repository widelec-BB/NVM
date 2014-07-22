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

#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#include <exec/types.h>
#include <intuition/classusr.h>
#include <utility/date.h>
#include <proto/charsets.h>
#include <proto/intuition.h>

#define findobj(id, parent) (Object*)DoMethod(parent, MUIM_FindUData, id)
#define EmptyRectangle(weight) MUI_NewObject(MUIC_Rectangle, MUIA_Weight, weight, TAG_END)
#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))

Object* StringGadget(ULONG id);
Object* StringLabel(STRPTR label, STRPTR preparse);
Object* GfxButton(ULONG id, STRPTR pic, UBYTE control);
Object* NormalButton(STRPTR label, UBYTE control, LONG objid, ULONG weight);
STRPTR StrIStr(STRPTR txt, STRPTR tmp); /* brute force case insensitive replacement for strstr() */
STRPTR StrNewLen(STRPTR s, LONG len);
STRPTR StrNewUTF8Recode(STRPTR s, ULONG mib);
#define StrNewUTF8ToSystem(s) StrNewUTF8Recode(s, MIBENUM_SYSTEM)

extern struct Library *IntuitionBase;

static inline ULONG xget(Object *obj, ULONG att)
{
  ULONG result;

  GetAttr(att, obj, &result);
  return result;
}

#endif /* __SUPPORT_H__ */
