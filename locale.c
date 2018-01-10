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

/*
** locale.h
**
** based on source created by Guido Mersmann's SimpleCat
**
*/
#include <exec/types.h>
#include <libraries/locale.h>
#include <proto/locale.h>
#include <proto/exec.h>

#define CATCOMP_BLOCK 1
#include "locale.h"

extern struct Library *SysBase;
static struct Library *LocaleBase;
static struct Locale  *locale_locale;
static struct Catalog *locale_catalog;

BOOL Locale_Open(STRPTR catname, ULONG version, ULONG revision)
{
	if((LocaleBase = (APTR) OpenLibrary("locale.library", 0)))
	{
		if((locale_locale = OpenLocale(NULL)))
		{
			if((locale_catalog = OpenCatalogA(locale_locale, catname, TAG_DONE)))
			{
				if(locale_catalog->cat_Version == version && locale_catalog->cat_Revision == revision)
					return(TRUE);

				CloseCatalog(locale_catalog);
				locale_catalog = NULL;
			}
			CloseLocale(locale_locale);
			locale_locale = NULL;
		}
		CloseLibrary((APTR) LocaleBase);
		LocaleBase = NULL;
	}
	return(FALSE);
}

VOID Locale_Close(VOID)
{
	if(LocaleBase)
	{
		if(locale_catalog)
		{
			CloseCatalog(locale_catalog);
			locale_catalog = NULL;
		}
		if(locale_locale)
		{
			CloseLocale(locale_locale);
			locale_locale = NULL;
		}
		CloseLibrary((APTR) LocaleBase);
		LocaleBase = NULL;
	}
}

STRPTR GetString(long id)
{
	LONG *l;
	UWORD *w;
	STRPTR builtin;

	l = (LONG *)CatCompBlock;

	while(*l != id)
	{
		w = (UWORD *)((ULONG)l + 4);
		l = (LONG *)((ULONG)l + (ULONG) * w + 6);
	}
	builtin = (STRPTR)((ULONG)l + 6);

	if(locale_catalog && LocaleBase)
		return((APTR) GetCatalogStr(locale_catalog, id, builtin));

	return(builtin);
}
