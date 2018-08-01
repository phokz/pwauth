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

#if defined(PAM_SOLARIS_26) || defined(PAM_SOLARIS) || defined(PAM_OS_X)
#define PAM
#endif /* PAM_SOLARIS_26 or PAM_SOLARIS */

#ifdef PAM
#ifdef NEED_UID
#include <pwd.h>
#endif
#ifdef PAM_OLD_OS_X
#include <pam/pam_appl.h>
#else
#include <security/pam_appl.h>
#endif
#endif

#ifdef PAM
/* ========================= PAM Authentication ======================== */
/*         Based on version by Karyl Stein (xenon313@arbornet.org)       */
/*            and on parts of mod_auth_pam.c by Ingo Lutkebohnle         */

/* Application data structure passed to PAM_conv: */

struct ad_user {
	const char *login;
	const char *passwd;
	};

/* The pam_unix.so library in Solaris 2.6 fails to pass along appdata_ptr
 * when it calls the conversation function.  So we use a global variable.  */

#ifdef PAM_SOLARIS_26
struct ad_user user_info;
#define PAM_SOLARIS
#endif /* PAM_SOLARIS_26 */

/* PAM_CONF - PAM Conversation Function.  Called by PAM to get login/password.
 * Note that "appdata_ptr" is really a "struct ad_user *" structure.
 */

#ifdef PAM_SOLARIS
/* In Solaris PAM, msg is a pointer to a pointer to a size num_msg array of
 * pam_message structures.
 */
# define msgi(i) ((*msg)[i])
#else
/* In Linux PAM and OpenPAM, msg is a pointer to a size num_msg array of
 * pointers to pam_message structures. 
 */
# define msgi(i) (*(msg[i]))
#endif

#ifdef PAM_SOLARIS_26
int PAM_conv (int num_msg, struct pam_message **msg,
     struct pam_response **resp, void *appdata_ptr)
{
    struct ad_user *user= &user_info;
#else
int PAM_conv (int num_msg, const struct pam_message **msg,
     struct pam_response **resp, void *appdata_ptr)
{
    struct ad_user *user= (struct ad_user *)appdata_ptr;
#endif /* PAM_SOLARIS_26 */
    struct pam_response *response;
    int i;

    /* Sanity checking */
    if (msg == NULL || resp == NULL || user == NULL)
    	return PAM_CONV_ERR;

#ifdef PAM_SOLARIS
    if (*msg == NULL)
	return PAM_CONV_ERR;
#endif

    response= (struct pam_response *)
    	malloc(num_msg * sizeof(struct pam_response));

    for (i= 0; i < num_msg; i++)
    {
	response[i].resp_retcode= 0;
	response[i].resp= NULL;

	switch (msgi(i).msg_style)
	{
	case PAM_PROMPT_ECHO_ON:
	    /* Store the login as the response */
	    /* This likely never gets called, since login was on pam_start() */
	    response[i].resp= appdata_ptr ? (char *)strdup(user->login) : NULL;
	    break;

	case PAM_PROMPT_ECHO_OFF:
	    /* Store the password as the response */
	    response[i].resp= appdata_ptr ? (char *)strdup(user->passwd) : NULL;
	    break;

	case PAM_TEXT_INFO:
	case PAM_ERROR_MSG:
	    /* Shouldn't happen since we have PAM_SILENT set. If it happens
	     * anyway, ignore it. */
	    break;

	default:
	    /* Something strange... */
	    if (response != NULL) free(response);
	    return PAM_CONV_ERR;
	}
    }
    /* On success, return the response structure */
    *resp= response;
    return PAM_SUCCESS;
}


/* CHECK_AUTH - Check a login and return a status code.
 * (This version for systems using PAM.)
 */

int check_auth(const char *login, const char *passwd)
{
#ifndef PAM_SOLARIS_26
    struct ad_user user_info= {login, passwd};
#endif /* PAM_SOLARIS_26 */
    struct pam_conv conv= { PAM_conv, (void *)&user_info };
    pam_handle_t *pamh= NULL;
    int retval;

#ifdef NEED_UID
    struct passwd *pwd;

    if ((pwd= getpwnam(login)) == NULL) return STATUS_UNKNOWN;
    hisuid= pwd->pw_uid;
    haveuid= 1;
#endif
#ifdef MIN_UNIX_UID
    if (hisuid < MIN_UNIX_UID) return STATUS_BLOCKED;
#endif

#ifdef PAM_SOLARIS_26
    user_info.login= login;
    user_info.passwd= passwd;
#endif /* PAM_SOLARIS_26 */

    retval= pam_start("pwauth", login, &conv, &pamh);

    if (retval == PAM_SUCCESS)
	retval= pam_authenticate(pamh, PAM_SILENT);

    if (retval == PAM_SUCCESS)
	retval= pam_acct_mgmt(pamh, 0);		/* permitted access? */

    if (pam_end(pamh,retval) != PAM_SUCCESS)	/* close PAM */
    {
	pamh= NULL;
	return STATUS_INT_ERR;
    }

    /* On failure we always return STATUS_UNKNOWN although we can't tell
     * if the failure was because of a bad login or a bad password */
    return (retval == PAM_SUCCESS ? STATUS_OK : STATUS_UNKNOWN);
}
#endif  /* PAM */
