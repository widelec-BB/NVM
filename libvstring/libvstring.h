/* libvstring.h
** libvstring Copyright (c) Grzegorz Kraszewski, All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** Redistributions of source code must retain the above copyright notice, this list 
** of conditions and the following disclaimer. Redistributions in binary form must 
** reproduce the above copyright notice, this list of conditions and the following 
** disclaimer in the documentation and/or other materials provided with the distribution. 
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
** OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
** IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef __LIBVSTRING_H__
#define __LIBVSTRING_H__

#ifndef __NOLIBBASE__
#define __NOLIBBASE__
#endif

#include <exec/types.h>
#include <exec/memory.h>
#include <stdarg.h>

extern struct Library *SysBase;

ULONG FmtLen(STRPTR fmt, ...);
void FmtPut(STRPTR dest, STRPTR fmt, ...);
LONG FmtNPut(STRPTR dest, STRPTR fmt, LONG maxlen, ...);
STRPTR VFmtNew(STRPTR fmt, va_list args);
STRPTR FmtNew(STRPTR fmt, ...);
ULONG StrLen(STRPTR str);
STRPTR StrCopy(STRPTR src, STRPTR dest);
STRPTR StrNCopy(STRPTR src, STRPTR dest, LONG maxlen);
STRPTR StrNew(STRPTR str);
STRPTR* CloneStrArray(STRPTR *array);
VOID FreeStrArray(STRPTR *array);
ULONG StrArrayLen(STRPTR *array);
void StrFree(STRPTR str);
BOOL StrEqu(STRPTR s1, STRPTR s2);
STRPTR StrCat(STRPTR s, STRPTR d);

/* memory alloc and free functions must be a matched pair */
#define StrFree(s) FreeVec(s)
#define FmtFree(s) FreeVec(s)
#define internal_alloc(l) AllocVec(l, MEMF_PUBLIC)

#endif /* __LIBVSTRING_H__ */

