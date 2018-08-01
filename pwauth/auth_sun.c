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

#ifdef SHADOW_SUN
#ifdef NEED_UID
#include <pwd.h>
#endif
#include <shadow.h>
struct spwd *getspnam(const char *name);
char *crypt(const char *key, const char *salt);
#endif /* SHADOW_SUN */

#ifdef SHADOW_JFH
#ifdef NEED_UID
#include <pwd.h>
#endif
#include <shadow.h>	  /* this may be hidden in /usr/local/include */
struct spwd *getspnam(const char *name);
char *pw_encrypt(const char *clear, const char *salt);
#endif /* SHADOW_JFH */

#if defined(SHADOW_JFH) || defined(SHADOW_SUN)
/* ===================== JFH and SUN Authentication ===================== */


/* CHECK_AUTH - Check a login and return a status code.
 * (This version for systems with getspnam() call.)
 */

int check_auth(const char *login, const char *passwd)
{
    char *cpass;
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
#ifdef SHADOW_JFH
    cpass= pw_encrypt(passwd, spwd->sp_pwdp);
#else
    cpass= crypt(passwd, spwd->sp_pwdp);
#endif
    if (!cpass || strcmp(cpass, spwd->sp_pwdp)) return STATUS_INVALID;
#ifdef CHECK_LOGIN_EXPIRATION
    if (spwd->sp_expire >= 0 && spwd->sp_expire < today)
	return STATUS_EXPIRED;
#endif
#ifdef CHECK_PASSWORD_EXPIRATION
    /* Root forced password expiration */
    if (spwd->sp_lstchg == 0)
	return STATUS_PW_EXPIRED;
	    
    /* Normal password expiration */
    /* We used to have sp_lstchg + sp_max + sp_inact < today
     * here, apparantly during the inact period you are only supposed to be able
     * to get on to change your password, not anything else, show we shouldn't
     * allow access during that period.
     */
    if (spwd->sp_max >= 0 &&
        spwd->sp_lstchg + spwd->sp_max < today)
	return STATUS_PW_EXPIRED;
#endif
    return STATUS_OK;
}
#endif /* SHADOW_JFH || SHADOW_SUN */
