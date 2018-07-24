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

#ifdef NEED_UID
int hisuid;
int haveuid= 0;
#endif

#ifdef SERVER_UIDS
/* Array of uid numbers that may run this program */
int server_uids[]= {SERVER_UIDS, 0};
#endif


int main(int argc, char **argv)
{
#ifdef ENV_METHOD
    char *login, *passwd;
#else
    char login[BFSZ+1], passwd[BFSZ+1];
    char *c;
#endif
#ifdef SERVER_UIDS
    int uid;
    int i;
#endif
    int status;
    struct rlimit rlim;

    /* Don't dump core (could contain part of shadow file) */
    rlim.rlim_cur = rlim.rlim_max = 0;
    (void)setrlimit(RLIMIT_CORE, &rlim);

#ifdef SERVER_UIDS
    /* Check that we were invoked by one of the listed uids or by root */
    uid= getuid();
    for (i= 0; server_uids[i] != 0 && server_uids[i] != uid; i++)
	;
    if (uid != server_uids[i])
	exit(STATUS_INT_USER);
#endif

    /* Get the arguments (login and password) */
#ifdef ENV_METHOD
    if ((login= getenv("USER")) == NULL ||
	(passwd= getenv("PASS")) == NULL)
	    exit(STATUS_INT_ARGS);
#else
    if (fgets(login, BFSZ, stdin) == NULL ||
	fgets(passwd, BFSZ, stdin) == NULL)
	    exit(STATUS_INT_ARGS);

    if ((c= strchr(login,'\n')) != NULL) *c= '\0';
    if ((c= strchr(passwd,'\n')) != NULL) *c= '\0';
#endif

#ifdef DOMAIN_AWARE
    if ((c= strchr(login,'\\')) != NULL)
	strcpy (login,++c);
#endif

    /* Check validity of login/passwd */
    status= check_auth(login,passwd);
#ifdef IGNORE_CASE
    if (status == STATUS_UNKNOWN)
    {
	int uc= 0;
	for (c= login; *c != '\0'; c++)
	    if (isascii(*c) && isupper(*c))
	    {
		uc= 1;
		*c= tolower(*c);
	    }
	if (uc)
	    status= check_auth(login,passwd);
    }
#endif

    bzero(passwd,strlen(passwd));	/* Erase plain-text from our memory */

#if defined(FAILLOG_JFH) || defined(FAILLOG_OPENBSD) || defined(FAILLOG_PWAUTH)
    if (status == STATUS_OK && !check_fails())
	status= STATUS_MANYFAILS;
#endif

#ifdef NOLOGIN_FILE
    if (status == STATUS_OK && !check_nologin())
	status= STATUS_NOLOGIN;
#endif

    if (status == STATUS_OK)
    {
	/* Good login */
#ifdef UNIX_LASTLOG
	lastlog();
#endif
#ifdef SLEEP_LOCK
	snooze(0);
#endif
	exit(STATUS_OK);
    }
    else
    {
	/* Bad login */
#ifdef KEEP_FAILLOG
	log_failure();
#endif
#ifdef SLEEP_LOCK
	snooze(SLEEP_TIME);
#endif
	exit(status);
    }
}
