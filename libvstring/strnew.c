/* libvstring/strnew.c */

#include "libvstring.h"

#include <proto/exec.h>


STRPTR StrNew(STRPTR str)
{
	STRPTR n = NULL;
	
	if(str != NULL)
	{
		ULONG len = StrLen(str);

		if(len == 0) return NULL;
		
		if (n = internal_alloc(len + sizeof(char)))
		{
			StrCopy(str, n);
		}
	}

	return n;
}

