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

#ifdef SHADOW_BSD
/* BSD shadow password system requires no special coding - good job */
#define SHADOW_NONE
#endif /* SHADOW_BSD */

#ifdef SHADOW_NONE
#include <unistd.h>
#include <pwd.h>

/* ===================== NONE Authentication ===================== */


/* CHECK_AUTH - Check a login and return a status code.
 * (This version for systems with only a getpwnam() call.)
 */

int check_auth(const char *login, const char *passwd)
{
    char *cpass;
    struct passwd *pwd= getpwnam(login);
#if defined(CHECK_LOGIN_EXPIRATION) || defined(CHECK_PASSWORD_EXPIRATION)
    time_t now= time(NULL);
#endif

    if (pwd == NULL) return(STATUS_UNKNOWN);
#ifdef NEED_UID
    if (pwd->pw_passwd[0] == '*' && pwd->pw_passwd[1] == '\0')
    	return STATUS_INT_NOROOT;
    hisuid= pwd->pw_uid;
    haveuid= 1;
#endif
#ifdef MIN_UNIX_UID
    if (hisuid < MIN_UNIX_UID) return(STATUS_BLOCKED);
#endif
    cpass= crypt(passwd, pwd->pw_passwd);
    if (strcmp(cpass, pwd->pw_passwd)) return STATUS_INVALID;
#ifdef CHECK_LOGIN_EXPIRATION
    if (pwd->pw_expire > 0 && pwd->pw_expire <= now)
	return STATUS_EXPIRED;
#endif
#ifdef CHECK_PASSWORD_EXPIRATION
    if (pwd->pw_change > 0 && pwd->pw_change <= now)
	return STATUS_PW_EXPIRED;
#endif
    return STATUS_OK;
}
#endif /* SHADOW_NONE */
