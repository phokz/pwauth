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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utmp.h>
#include <time.h>

#include "config.h"
#include "fail_log.h"


#ifdef FAILLOG_JFH
/* CHECK_FAILS - Read the faillog file entry for the given uid.  Return NULL
 * if there have been no failures.  If there have been failures, reset the
 * count to zero if "reset" is true, and Return a string describing the state.
 * The format of the string depends on the versbose flag.
 *   verbose=0
 *      <count>:<unixtime>::<tty>\n
 *   verbose=1
 *      <count> failures since last login.  Last was <time> on <tty>.\n
 */

const char *check_fails(uid_t uid, int reset, int verbose)
{
    struct faillog flog;
    int flfd;
    static char buf[1024];
    char *ct;
    time_t now;

    /* Return null if we can't open the file */
    if ((flfd= open(PATH_FAILLOG, reset ? O_RDWR : O_RDONLY)) < 0)
	return NULL;

    /* Read the log file entry for this user - if none then there have
     * been no failures */
    lseek(flfd, uid * sizeof(struct faillog), 0);
    if (read(flfd, &flog, sizeof(struct faillog)) != sizeof(struct faillog))
    {
	close(flfd);
	return NULL;
    }

    /* Check if there have been any failures */
    if (flog.fail_cur <= 0)
    {
	close(flfd);
	return NULL;
    }

    /* Generate the result message */
    if (verbose)
    {
	time(&now);
	ct= ctime(&flog.fail_time);
	ct[24]= '\0';
	if (now - flog.fail_time < (365*24*3600))
	    ct[19]= '\0';
	if (now - flog.fail_time < (24*3600))
	    ct+= 11;
	snprintf(buf, sizeof(buf), "%d %s since last login.  Last was %s on %s.",
	    flog.fail_cnt, flog.fail_cnt == 1 ? "failure" : "failures",
	    ct, flog.fail_line);
    }
    else
	snprintf(buf, sizeof(buf), "%d:%ld::%s",
	    flog.fail_cnt, flog.fail_time, flog.fail_line);

    /* Reset the count, if that was desired */
    flog.fail_cnt= 0;
    lseek(flfd, uid * sizeof(struct faillog), 0);
    write(flfd, &flog, sizeof(struct faillog));
    close(flfd);

    return buf;
}
#endif /* FAILLOG_JFH */


#ifdef FAILLOG_OPENBSD
/* CHECK_FAILS - Read the faillog file entry for the given uid.  Return NULL
 * if there have been no failures.  If there have been failures, reset the
 * count to zero if "reset" is true, and Return a string describing the state.
 * The format of the string depends on the versbose flag.
 * verbose=0
 *   <count>:<unixtime>:<host>:<tty>\n
 * verbose=1
 *   <count> failures since last login.  Last was <time> from <host> on <tty>.\n
 */

const char *check_fails(uid_t uid, int reset, int verbose)
{
    struct badlogin flog;
    int flfd;
    static char buf[1024];
    char *ct;
    time_t now;

    /* Return null if we can't open the file */
    if ((flfd= open(PATH_FAILLOG, reset ? O_RDWR : O_RDONLY)) < 0)
	return NULL;

    /* Read the log file entry for this user - if none then there have
     * been no failures */
    lseek(flfd, uid * sizeof(struct badlogin), 0);
    if (read(flfd, &flog, sizeof(struct badlogin)) != sizeof(struct badlogin))
    {
	close(flfd);
	return NULL;
    }

    /* Check if there have been any failures */
    if (flog.count <= 0)
    {
	close(flfd);
	return NULL;
    }

    /* Generate the result message */
    if (verbose)
    {
	time(&now);
	ct= ctime(&flog.bl_time);
	ct[24]= '\0';
	if (now - flog.bl_time < (365*24*3600))
	    ct[19]= '\0';
	if (now - flog.bl_time < (24*3600))
	    ct+= 11;
	if (flog.bl_host[0] != '\0')
	    snprintf(buf, sizeof(buf), "%d %s since last login.  Last was %s from %s on %s.",
		flog.count, flog.count == 1 ? "failure" : "failures",
		ct, flog.bl_host, flog.bl_line);
	else
	    snprintf(buf, sizeof(buf), "%d %s since last login.  Last was %s on %s.",
		flog.count, flog.count == 1 ? "failure" : "failures",
		ct, flog.bl_line);
    }
    else
	snprintf(buf, sizeof(buf), "%d:%ld:%s:%s",
	    flog.count, flog.bl_time, flog.bl_host, flog.bl_line);

    /* Reset the count, if that was desired */
    flog.count= 0;
    lseek(flfd, uid * sizeof(struct badlogin), 0);
    write(flfd, &flog, sizeof(struct badlogin));
    close(flfd);

    return buf;
}
#endif
