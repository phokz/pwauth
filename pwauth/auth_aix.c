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

#ifdef SHADOW_AIX
#ifdef NEED_UID
#include <sys/types.h>
#include <pwd.h>
#endif
#include <userpw.h>

/* ============= Low-Level AIX Authentication ===================== */


/* CHECK_AUTH - Check a login against the system password file and return a
 * status code. (This version for systems with a getuserpw() call.)
 */

int check_auth(const char *login, const char *passwd)
{
    char *cpass;
    struct userpw *upwd= getuserpw(login);
#ifdef NEED_UID
    struct passwd *pwd;
#endif
    if (upwd == NULL)
    	return(errno == EACCES ? STATUS_INT_NOROOT : STATUS_UNKNOWN);
#ifdef NEED_UID
    if ((pwd= getpwnam(login)) == NULL) return(STATUS_UNKNOWN);
    hisuid= pwd->pw_uid;
    haveuid= 1;
#endif
#ifdef MIN_UNIX_UID
    if (hisuid < MIN_UNIX_UID) return(STATUS_BLOCKED);
#endif
    cpass= crypt(passwd, upwd->upw_passwd);
    return(strcmp(cpass, upwd->upw_passwd) ? STATUS_INVALID : STATUS_OK);
}
#endif /* SHADOW_AIX */


#ifdef AUTHENTICATE_AIX
#ifdef NEED_UID
#include <sys/types.h>
#include <pwd.h>
#endif
#include <userpw.h>
#include <usersec.h>

/* ================ High-Level AIX Authentication ==================== */


/* CHECK_AUTH - Check a login through the high-level AIX authentication
 * interface and return a status code.
 */

int check_auth(const char *login, const char *passwd)
{
    char *cpass;
    char *message;
    int i= 0;
    int stat = STATUS_INVALID;
#ifdef NEED_UID
    struct passwd *pwd;
    if ((pwd= getpwnam(login)) == NULL) return(STATUS_UNKNOWN);
    hisuid= pwd->pw_uid;
    haveuid= 1;
#endif
#ifdef MIN_UNIX_UID
    if (hisuid < MIN_UNIX_UID) return(STATUS_BLOCKED);
#endif
    if (authenticate(login, passwd, &i, &message) == 0)
      { stat = STATUS_OK; }
    free(&message);  
    return(stat);
}
#endif /* AUTHENICATE_AIX */
