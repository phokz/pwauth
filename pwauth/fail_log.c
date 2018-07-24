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

#ifdef RESET_FAIL_COUNT
#define MODE_FAILLOG O_RDWR
#else
#define MODE_FAILLOG O_RDONLY
#endif

#ifdef FAILLOG_JFH
/* CHECK_FAILS - Check if the account is disable dued to the maximum number of
 * failed logins being exceeded.  Returns true if the account is OK to log
 * into or if the faillog information doesn't exist.  This never resets the
 * bad login count to zero.  It should, but I don't really believe anyone
 * uses this anymore, so I'm not going to bother implementing that.
 */

int check_fails(void)
{
    int result= 1;
    struct faillog flog;
    int flfd;

    if (!haveuid) return 1;

    if ((flfd= open(PATH_FAILLOG,MODE_FAILLOG)) >= 0)
    {
	lseek(flfd, hisuid * sizeof(struct faillog), 0);
	result= (read(flfd, &flog, sizeof(struct faillog))
	         != sizeof(struct faillog)) || 
	        flog.fail_max == 0 || flog.fail_cur < flog.fail_max;
	close(flfd);
    }
    return result;
}


/* LOG_FAILURE - Do whatever we need to do to log a failed login attempt.
 */

void log_failure(void)
{
    int flfd;
    struct faillog flog;

    if (!haveuid) return;

    /* Log the failure in /var/adm/faillog - JFH style */
    if ((flfd= open(PATH_FAILLOG,O_RDWR)) > 0)
    {
	/* Read the user's record (indexed by uid) */
	lseek(flfd, hisuid * sizeof(struct faillog), 0);
	if (read(flfd,&flog,sizeof(struct faillog)) != sizeof(struct faillog))
	{
	    flog.fail_cnt= 0;
	    flog.fail_max= 0;
	    flog.fail_time= 0L;
	    flog.failline[0]= '\0';
	}

	/* Increment the count (checking for overflow) */
	if (flog.fail_cnt + 1 > 0)
		flog.fail_cnt++;
	flog.fail_time= time(0L);
	strcpy(flog.fail_line,"http");

	/* Write it back out */
	lseek(flfd, hisuid * sizeof(struct faillog), 0);
	write(flfd, &flog, sizeof(struct faillog));
	close(flfd);
    }
}
#endif /* FAILLOG_JFH */


#ifdef FAILLOG_OPENBSD
/* CHECK_FAILS - Check if the account is disabled due to the maximum number of
 * failed logins being exceeded.  Returns true if the account is OK to log
 * into or if the faillog information doesn't exist.  If the count has not
 * been exceeded, then the count is reset to zero.
 */

int check_fails(void)
{
    int result= 1;
#if defined(MAX_FAIL_COUNT) || defined(RESET_FAIL_COUNT)
    struct badlogin flog;
    int flfd;

    if (!haveuid) return 1;

    if ((flfd= open(PATH_FAILLOG,MODE_FAILLOG)) >= 0)
    {
	/* Read user's record (indexed by uid) */
	lseek(flfd, hisuid * sizeof(struct badlogin), 0);
	if (read(flfd, &flog, sizeof(struct badlogin)) !=
		sizeof(struct badlogin) || flog.bl_time == 0)
	    memset(&flog, 0, sizeof(struct badlogin));
#ifdef MAX_FAIL_COUNT
	result= (flog.count < MAX_FAIL_COUNT);
#endif
#ifdef RESET_FAIL_COUNT
	if (result && flog.count > 0)
	{
	    flog.count= 0;
	    lseek(flfd, hisuid * sizeof(struct badlogin), 0);
	    write(flfd, &flog, sizeof(struct badlogin));
	}
#endif
	close(flfd);
    }
#endif /* MAX_FAIL_COUNT || RESET_FAIL_COUNT */
    return result;
}


/* LOG_FAILURE - Do whatever we need to do to log a failed login attempt.
 */

int log_failure(void)
{
    int flfd;
    struct badlogin flog;
    char *host;

    if (!haveuid) return;

    /* Log the failure in /var/log/failedlogin - OpenBSD style */
    if ((flfd= open(PATH_FAILLOG,O_RDWR)) > 0)
    {
	/* Read user's record (indexed by uid) */
	lseek(flfd, hisuid * sizeof(struct badlogin), 0);
	if (read(flfd, &flog, sizeof(struct badlogin)) !=
		sizeof(struct badlogin) || flog.bl_time == 0)
	    memset(&flog, 0, sizeof(struct badlogin));

	/* Increment the count (checking for overflow) */
	if (flog.count + 1 > 0)
		flog.count++;

	/* Set time and remote host names */
	time(&flog.bl_time);
	strncpy(flog.bl_line, "http", sizeof(flog.bl_line));
	if ((host= getenv("HOST")) == NULL) host= getenv("IP");
	if (host != NULL)
	    strncpy(flog.bl_host, host, sizeof(flog.bl_host));
	else
	    flog.bl_host[0]= '\0';
	flog.bl_name[0]= '\0';		/* Remote name always unknown */

	/* Write it back out */
	lseek(flfd, hisuid * sizeof(struct badlogin), 0);
	write(flfd, &flog, sizeof(struct badlogin));
	close(flfd);
    }
}
#endif
