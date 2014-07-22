/* libvstring/strequ.c */

#include "libvstring.h"


BOOL StrEqu(STRPTR s1, STRPTR s2)
{
	while (*s1 && (*s1 == *s2)) { s1++; s2++; }
	if (*s1 == *s2) return TRUE;
	else return FALSE;
}

