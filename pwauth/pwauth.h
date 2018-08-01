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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/resource.h>
/* Configuration is meant to be done in config.h */
#include "config.h"

/* Status codes returned from pwauth - note normally we return STATUS_UNKNOWN
 * if the login name does not exist and STATUS_INVALID if the login exists
 * but the password is incorrect.  However, in some configurations it is not
 * possible to distinguish between these two cases, so STATUS_UNKNOWN is
 * returned for both.
 */

#define STATUS_OK 	  0   /* Valid Login */
#define STATUS_UNKNOWN	  1   /* Login doesn't exist or password incorrect */
#define STATUS_INVALID	  2   /* Password was incorrect */
#define STATUS_BLOCKED    3   /* UID is below minimum allowed to use this */
#define STATUS_EXPIRED    4   /* Login ID has passed it's expiration date */
#define STATUS_PW_EXPIRED 5   /* Password has expired and must be changed */
#define STATUS_NOLOGIN    6   /* Logins have been turned off */
#define STATUS_MANYFAILS  7   /* Bad login limit exceeded */

#define STATUS_INT_USER  50   /* pwauth was run by wrong uid */
#define STATUS_INT_ARGS  51   /* login/password not passed in correctly */
#define STATUS_INT_ERR   52   /* Miscellaneous internal errors */
#define STATUS_INT_NOROOT 53  /* pwauth cannot read password database */


#ifdef IGNORE_CASE
# include <ctype.h>
#endif

#ifdef UNIX_LASTLOG
# define NEED_UID
# include <utmp.h>
# ifdef HAVE_LASTLOG_H
#  include <lastlog.h>
# endif
# ifndef UT_LINESIZE
#  define UT_LINESIZE 8
# endif
# ifndef UT_HOSTSIZE
#  define UT_HOSTSIZE 16
# endif
# if !defined(LASTLOG) && defined(PATHS_H)
#  include <paths.h>
#  ifdef _PATH_LASTLOG
#   define LASTLOG _PATH_LASTLOG
#  endif
# endif
# ifndef LASTLOG
#  define LASTLOG "/usr/adm/lastlog"
# endif
#endif /*UNIX_LASTLONG*/

#ifdef NOLOGIN_FILE
# ifndef MIN_NOLOGIN_UID
#  define MIN_NOLOGIN_UID 0
# endif
# if MIN_NOLOGIN_UID > 0
#  define NEED_UID
# endif
#endif

#include "fail_log.h"

#ifdef MIN_UNIX_UID
# if MIN_UNIX_UID <= 0
#  undef MIN_UNIX_UID
# else
#  ifndef NEED_UID
#   define NEED_UID
#  endif
# endif
#endif

#ifdef NEED_UID
extern int hisuid;
extern int haveuid;
#endif

#ifndef SLEEP_TIME
# define SLEEP_TIME 2
#endif

#ifndef BFSZ
# define BFSZ 1024
#endif

void snooze(int seconds);
void lastlog(void);
int check_auth(const char *login, const char *passwd);
int check_nologin(void);
