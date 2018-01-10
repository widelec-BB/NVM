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

#ifndef __GLOBAL_DEFINES_H__
#define __GLOBAL_DEFINES_H__

#ifdef __DEBUG__
#include <clib/debug_protos.h>
#define tprintf( ... ) KPrintF("NVM: " __VA_ARGS__ )
#else
#define tprintf( ... ) 
#endif

#define APP_DATE     __AMIGADATE__
#define APP_AUTHOR   "Filip \"widelec\" Maryja�ski"
#define APP_NAME     "NVM"
#define APP_CYEARS   "2012-2014"
#define APP_BASE     "NVM"
#define APP_DESC     "YouTube subtitles downloader"
#define APP_VER      "$VER: " APP_NAME " 0.9" APP_DATE "� " APP_CYEARS " BlaBla group" 

#define APP_ABOUT    "\33b%p\33n\n\t" APP_AUTHOR "\n\n" "\33b%I\33n\n\t" "Konrad \"recedent\" Czuba"

#endif