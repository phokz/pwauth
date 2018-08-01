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

#ifdef LOGIN_CONF_OPENBSD
#include <pwd.h>
#include <login_cap.h>
#include <bsd_auth.h>

/* =================== OpenBSD LOGIN.CONF Authentication =================== */


/* CHECK_AUTH - Check a login and return a status code.
 * (This version for systems with auth_usercheck() call.)
 */

int check_auth(const char *login, const char *passwd)
{
    auth_session_t *as;
    login_cap_t *lc;
    struct passwd *pwd;
    char *style;
    int state, exists;

    /*  The following would work, but it doesn't return the logincap to us,
     *  and we will need it below to check for nologin files.
     *
     *     as= auth_usercheck(login, NULL, NULL, passwd);
     */

    /* Get the login cap for this user from the database */
    pwd= getpwnam(login);
    exists= (pwd != NULL);
#ifdef NEED_UID
    if (exists) {hisuid= pwd->pw_uid; haveuid= 1;}
#endif
    if ((lc = login_getclass(pwd ? pwd->pw_class : NULL)) == NULL)
	return STATUS_INT_ERR;

    /* Get the login style to use */
    if ((style= login_getstyle(lc, NULL, NULL)) == NULL)
    {
	login_close(lc);
    	return STATUS_INT_ERR+1;
    }

    /* Start a BSD authentication session and set it up */
    if ((as= auth_open()) == NULL)
    {
	login_close(lc);
	return STATUS_INT_ERR+2;
    }
    auth_setitem(as, AUTHV_SERVICE, "response");
    auth_setdata(as, "", 1);
    auth_setdata(as, passwd, strlen(passwd)+1);

    /* Run the authenticator */
    as= auth_verify(as, style, login, lc->lc_class, (char *)NULL);

    /* Check if login is expired */
#ifdef CHECK_LOGIN_EXPIRATION
    if (auth_check_expire(as) < 0)
    {
    	login_close(lc);
	auth_close(as);
	return STATUS_EXPIRED;
    }
#endif

    /* Check if password has expired */
#ifdef CHECK_PASSWORD_EXPIRATION
    if (auth_check_change(as) < 0)
    {
    	login_close(lc);
	auth_close(as);
	return STATUS_PW_EXPIRED;
    }
#endif

    /* Get the results */
    pwd= auth_getpwd(as);
    exists= exists || (pwd != NULL);
    state= auth_getstate(as);
    auth_close(as);
#ifdef NEED_UID
    /* prefer this uid number to any one we got before - probably the same */
    if (pwd != NULL) {hisuid= pwd->pw_uid; haveuid= 1;}
#endif

    if (!(state & AUTH_OKAY))
    {
	login_close(lc);
	return (exists ? STATUS_INVALID : STATUS_UNKNOWN);
    }

    /* Check for nologin file */
    if (!login_getcapbool(lc, "ignorenologin", 0))
    {
	/* If ignorelogin is not define, nologin should be defined */
	char *nologin= login_getcapstr(lc, "nologin", "", NULL);
	login_close(lc);
	if (nologin == NULL)
	    return STATUS_INT_ERR+4;

	/* check existance of nologin file defined for class in login.conf */
	if (*nologin != '\0')
	{
	    int rc= access(nologin,F_OK);
	    free(nologin);
	    if (rc == 0) return STATUS_NOLOGIN;
	}

	/* Check existance of stock nologin file */
#ifdef _PATH_NOLOGIN
	if (access(_PATH_NOLOGIN,F_OK) == 0)
	    return STATUS_NOLOGIN;
#endif
    }
    else
	login_close(lc);

    /* Check if uid is above minimum */
#ifdef MIN_UNIX_UID
    if (haveuid && hisuid < MIN_UNIX_UID) return(STATUS_BLOCKED);
#endif

    return STATUS_OK;
}
#endif /* LOGIN_CONF_OPENBSD */
