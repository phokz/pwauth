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
#include <utmp.h>
#include <pwd.h>

#include "config.h"
#include "fail_log.h"


int main(int argc, char **argv)
{
    int i, j;
    int reset= 0, verbose= 1;
    char *user= NULL;
    const char *msg= NULL;
    uid_t uid= getuid();

    /* Parse command line */
    for (i= 1; i < argc; i++)
    {
	if (argv[i][0] == '-')
	{
	    for (j= 1; argv[i][j] != '\0'; j++)
	    {
		switch (argv[i][j])
		{
		    case 'z': reset= 1; break;
		    case 's': verbose= 0; break;
		    default: goto usage;
		}
	    }
	}
	else if (user == NULL)
	    user= argv[i];
	else
	    goto usage;
    }

    /* Root can run this on other user's accounts */
    if (user != NULL)
    {
	struct passwd *pw;

	if ((pw= getpwnam(user)) == NULL)
	{
	    fprintf(stderr,"%s: User %s does not exist.\n", argv[0], user);
	    exit(2);
	}

	if (uid != 0 && pw->pw_uid != uid)
	{
	    fprintf(stderr,"%s: Only root can access other user's accounts.\n",
		    argv[0]);
	    exit(3);
	}

	uid= pw->pw_uid;
    }

    /* Do the thing */
#if defined(FAILLOG_JFH) || defined(FAILLOG_OPENBSD)
    msg= check_fails(uid, reset, verbose);
#endif

    /* Output the result */
    if (msg != NULL)
    	puts(msg);
    else if (!verbose)
    	puts("0:::");

    exit(0);

usage: fprintf(stderr,"usage: %s [-z] [-s] [user]\n", argv[0]);
    exit(1);
}
