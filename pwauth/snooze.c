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


#include "pwauth.h"

#ifdef SLEEP_LOCK

/* SNOOZE - Do a serialized sleep of the given number of seconds.  This means,
 * wait till no other pwauth processes are in their sleeps, and then sleep
 * for the number of seconds given.  Note that a snooze(0) may lead to some
 * sleep time, if other pwauth processes are in sleeps.
 */

void snooze(int seconds)
{
    int slfd;
    struct flock lock;
    lock.l_type= F_WRLCK;
    lock.l_whence= SEEK_SET;
    lock.l_start= 0;
    lock.l_len= 0;

    /* Lock the sleep-lock file to serialize our sleeps */

    if ((slfd= open(SLEEP_LOCK,O_CREAT|O_RDWR,0644)) >= 0)
	fcntl(slfd,F_SETLKW,&lock);

    sleep(seconds);

    /* Release sleep-lock file */
    /*lock.l_type= F_UNLCK; fcntl(slfd,F_SETLK,&lock);*/
    close(slfd);
}

#endif
