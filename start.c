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

/*
** start.c
**
** based on source by Grzegorz Kraszewski
**
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <workbench/startup.h>

struct Library *SysBase = 0;
struct Library *DOSBase = 0;

extern ULONG Main(VOID);

ULONG __abox__ = 1;

ULONG Start(VOID)
{
	struct Process *myproc = 0;
	struct Message *wbmessage = 0;
	BOOL have_shell = FALSE;
	ULONG return_code = RETURN_OK;

	SysBase = *(struct Library**)4L;
	myproc = (struct Process*)FindTask(NULL);
	
	if (myproc->pr_CLI)
		have_shell = TRUE;

	if(!have_shell)
	{
		WaitPort(&myproc->pr_MsgPort);
		wbmessage = GetMsg(&myproc->pr_MsgPort);
	}

	if((DOSBase = OpenLibrary((STRPTR)"dos.library", 0)))
	{
		return_code = Main();
		CloseLibrary(DOSBase);
	}
	else
		return_code = RETURN_FAIL;

	if(wbmessage)
	{
		Forbid();
		ReplyMsg(wbmessage);
	}
	
	return return_code;
}