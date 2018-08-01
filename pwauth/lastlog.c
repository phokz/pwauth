/* =======================================================================
 * Copyright 1996, Jan D. Wolter and Steven R. Weiss, All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The names of the authors must not be used to endorse or
 *    promote products derived from this software without prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * =======================================================================
 */

#include <unistd.h>

#include "pwauth.h"

/* LASTLOG - update the system's lastlog */

#ifdef UNIX_LASTLOG
void lastlog(void)
{
    struct lastlog ll;
    int fd;
    char *hostname= getenv("HOST");
    char *hostaddr= getenv("IP");

    ll.ll_time= time(0L);
    strncpy(ll.ll_line,"http",UT_LINESIZE);

    if (hostname != NULL && strlen(hostname) <= UT_HOSTSIZE)
	strncpy(ll.ll_host,hostname,UT_HOSTSIZE);
    else if (hostaddr != NULL)
	strncpy(ll.ll_host,hostaddr,UT_HOSTSIZE);
    else
	ll.ll_host[0]= '\0';

    if ((fd= open(LASTLOG,O_WRONLY)) < 0) return;

    lseek(fd, (long)(hisuid * sizeof(struct lastlog)), SEEK_SET);
    write(fd, &ll, sizeof(struct lastlog));
    close(fd);
}
#endif /*UNIX_LASTLOG*/
