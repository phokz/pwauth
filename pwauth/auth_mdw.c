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

#ifdef SHADOW_MDW
#ifdef NEED_UID
#include <pwd.h>
#endif
#include <shadow.h>	  /* is -I/usr/local/include on gcc command? */
char *kg_pwhash(const char *clear, const char *user, char *result, int resultlen);
char *pw_encrypt(const char *clear, const char *salt);
#endif /* SHADOW_MDW */

#ifdef SHADOW_MDW
/* ===================== MDW Authentication ===================== */


/* CHECK_AUTH - Check a login and return a status code.
 * (This version for systems with kg_pwhash() call.)
 */

int check_auth(const char *login, const char *passwd)
{
    char *cpass;
    char bf[40];
    struct spwd *spwd= getspnam(login);
#ifdef NEED_UID
    struct passwd *pwd;
#endif
#if defined(CHECK_LOGIN_EXPIRATION) || defined(CHECK_PASSWORD_EXPIRATION)
    int today= time(NULL)/(24*60*60);
#endif
    if (spwd == NULL)
    	return(errno == EACCES ? STATUS_INT_NOROOT : STATUS_UNKNOWN);
#ifdef NEED_UID
    if ((pwd= getpwnam(login)) == NULL) return(STATUS_UNKNOWN);
    hisuid= pwd->pw_uid;
    haveuid= 1;
#endif
#ifdef MIN_UNIX_UID
    if (hisuid < MIN_UNIX_UID) return(STATUS_BLOCKED);
#endif
    if (spwd->sp_pwdp[0] != '%')
	cpass= pw_encrypt(passwd, spwd->sp_pwdp);
    else if ((cpass= kg_pwhash(passwd, login, bf, 40)) == NULL)
	return(STATUS_INT_ERR);

    if (strcmp(cpass, spwd->sp_pwdp)) return STATUS_INVALID;
#ifdef CHECK_LOGIN_EXPIRATION
    if (spwd->sp_expire >= 0 && spwd->sp_expire < today)
	return STATUS_EXPIRED;
#endif
#ifdef CHECK_PASSWORD_EXPIRATION
    /* Root forced password expiration */
    if (spwd->sp_lstchg == 0)
	return STATUS_PW_EXPIRED;
	    
    /* Normal password expiration */
    if (spwd->sp_max >= 0 && spwd->sp_lstchg + spwd->sp_max < today)
	return STATUS_PW_EXPIRED;
#endif
    return STATUS_OK;
}
#endif /* SHADOW_MDW */
