/* libvstring/strcat.c */

#include "libvstring.h"

STRPTR StrCat(STRPTR s, STRPTR d)
{
	while(*d) d++;
	
	while(*d++ = *s++);

	*d = '\0';

	return d;
}
