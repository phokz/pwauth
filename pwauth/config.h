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
 *              permission.
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


/* PWAUTH configuration file 
 *
 * Note - the default settings in this file are the way I use it.  I
 * guarantee you, they won't work for you.  You must change them.
 *
 */


/* There are lots of different kinds of password databases.  Define one of
 * the following:
 *
 *  - SHADOW_NONE: old fashioned Unix systems which have the encrypted
 *    passwords in the passwd file.  Actually, since you don't need to be
 *    root to access these, you might do better using Lou Langholtz's
 *    mod_auth_system.c, which is available from the contributed modules
 *    directory at apache.org.
 *
 *  - SHADOW_BSD: This is the shadow password system in BSDI, NetBSD, OpenBSD,
 *    and FreeBSD.  It uses exactly the same calls as SHADOW_NONE, but
 *    getpwnam() only returns the encrypted password if you are root.  This
 *    would only work with OS X if the accounts being authenticated are
 *    configured with legacy crypt style passwords.  In general, the PAM
 *    option is more likely to be usuable in OS X.
 *
 *  - SHADOW_SUN:  This is the shadow password system in Solaris, Linux and
 *    IRIX 5.3 systems.  It uses getspnam() to fetch passwords and crypt() to
 *    encrypt them.
 *
 *  - SHADOW_JFH:  This the old version of Julianne F. Haugh's public-domain
 *    shadow package.  It uses getspnam() to fetch passwords and pw_encrpyt()
 *    to encrypt them.  The JFH shadow code is available at
 *        ftp://gumby.dsd.trw.com/pub/security/shadow-3.2.2.tar.Z
 *    Newer versions seem to be compatible with SHADOW_SUN.
 *
 *  - SHADOW_MDW:  This is Grex's variation on the JFH shadow file system,
 *    which uses Marcus D. Watt's interface to the password encryption.  If you
 *    ain't Grex, you ain't got it.
 *
 *  - SHADOW_AIX:  This is the AIX shadow password system.  It uses getuserpw()
 *    to fetch passwords and (apparantly) crypt() to encrypt them.  This has
 *    not been tested.  Shadow BSD is also likely to work with AIX.
 *
 *  - SHADOW_HPUX:  This is the HP-UX shadow password system.  It uses
 *    getprpwnam() to fetch passwords and either crypt() or bigcrypt() to
 *    encrypt them (I'm not sure which is right).  This has not been tested
 *    and probably doesn't work.
 *
 *  - PAM: Talk to the authentication system through PAM - the plug-in
 *    authentication module interface.  This exists on Solaris 7, FreeBSD,
 *    most versions of Linux and OS X.  You'll need to create
 *    /etc/pam.d/pwauth or edit /etc/pam.config to include entries for
 *    pwauth.  If you are using PAM to authenticate out of something you
 *    don't need to be root to access, then you might use instead Ingo
 *    Lutkebohle's mod_auth_pam.c module.  You probably want to comment
 *    out the UNIX_LASTLOG, MIN_UNIX_UID, and NOLOGIN_FILE options below.
 *
 *  - PAM_SOLARIS:  Solaris versions of PAM have some incompatibilities with
 *    Linux/FreeBSD versions.  I think HP-UX needs this too, but don't really
 *    know.
 *
 *  - PAM_SOLARIS_26:  Solaris 2.6 PAM has some header file declarations and
 *    function behaviors that don't agree with either their own documentation
 *    or with any other implementation.  Bugs, in short.  This option uses
 *    PAM with work-arounds for the Solaris 2.6 bugs.
 *
 *  - PAM_OS_X:  OS X keeps it's header files in a different place, so use
 *    this option instead of the PAM option.
 *
 *  - LOGIN_CONF_OPENBSD:  Many BSD derived systems use a login.conf file to
 *    configure authentication instead of (or in addition to) PAM.  We
 *    currently support authentication through this mechanism only for
 *    OpenBSD.  Of course, if you login.conf configuration is standard, you
 *    can just use SHADOW_BSD, but if you want pwauth to respect settings
 *    in login.conf this option can be used instead.  The API used here, is
 *    however, pretty much unique to OpenBSD and will not work on NetBSD or
 *    FreeBSD.
 */

/* #define SHADOW_NONE		/**/
/* #define SHADOW_BSD		/* FreeBSD, NetBSD, OpenBSD, BSDI, OS X */
#define SHADOW_SUN		/* Linux, Solaris, IRIX */
/* #define SHADOW_JFH		/**/
/* #define SHADOW_MDW		/**/
/* #define SHADOW_AIX		/* AIX */
/* #define SHADOW_HPUX		/* HPUX ? */

/* #define PAM			/* Linux PAM or OpenPAM*/
/* #define PAM_OS_X		/* PAM on OS X */
/* #define PAM_SOLARIS		/* PAM on Solaris other than 2.6 */
/* #define PAM_SOLARIS_26	/* PAM on Solaris 2.6 */
/* #define LOGIN_CONF_OPENBSD	/* login.conf on OpenBSD */


/* There is also limited support for two failure logging systems (the database
 * that informs you that "there have been 3426 unsuccessful attempts to log
 * into your account since your last login" and which may disable accounts
 * with too many failed logins).
 *
 * If a FAILLOG option is enabled, pwauth will increment the failure count
 * each time there is a failed attempt to login.  Depending on the the
 * configuration, it may also deny logins to users who have had too many
 * bad login attempts.
 *
 * Very few Unix systems seem to have faillog files installed, so most
 * installations will not want this option.
 *
 * No faillog option should be used with PAM.  This kind of logging is handled
 * at a lower level with PAM.
 *
 *  - FAILLOG_JFH:  This is associated with the JFH shadow system.  Some Linux
 *    systems may have this, but most don't seem to.  Failures are logged in
 *    the /var/adm/faillog file, and if any user accumulates too many failed
 *    logins, future logins are denied.  The faillog.h header file is part of
 *    the JFH shadow file package.  If you define PATH_FAILLOG, then this
 *    will be used as the path of the faillog file instead of the one defined
 *    in faillog.h.
 *
 *  - FAILLOG_OPENBSD:  OpenBSD has a faillog, although it does not disable
 *    logins if any maximum exceeded.  Failure counts are kept in
 *    /var/log/failedlogin.  There is no system header file that defines the
 *    format of this file, however.  Instead the definition for the file
 *    format is embedded in the "login" source code.  Bad things will happen
 *    if the definition there does not the match the definition in pwauth.
 *
 *    Though OpenBSD's login program does not refuse further logins if the
 *    number of failed logins has gotten too large, pwauth will do so if you
 *    define MAX_FAIL_COUNT.  If you want to use a file different than the
 *    default, you can define this in PATH_FAILLOG.
 *
 *  - FAILLOG_PWAUTH:  This is meant to be used by systems that lack any
 *    native faillog.  This will keep a faillog for pwauth only.  For the
 *    moment, this is identical to FAILLOG_OPENBSD, except that the default
 *    path of the log file is different.  In future releases this may
 *    diverge from FAILLOG_OPENBSD.  Defining MAX_FAIL_COUNT and PATH_FAILLOG
 *    has the same effect as above.
 *
 * RESET_FAIL_COUNT controls whether or not the failure count is reset to
 * zero when a successful login occurs.  The advantages of this are obvious.
 * The problem with it is that it obliterates all record of how many failures
 * there were.  Login utilities normally print out the failure count before
 * resetting it, so that users are notified if their account is under attack,
 * but pwauth cannot print messages that the user will see.  The optimum
 * strategy would probably be to have your CGI run a separate program, such
 * as the 'checkfaillog' program included in this distribution, that
 * reports and resets the failure count.
 */

/* #define FAILLOG_JFH				/**/
/* #define FAILLOG_OPENBSD			/**/
/* #define FAILLOG_PWAUTH			/**/

/* #define PATH_FAILLOG "/var/log/faillog"	/**/
/* #define MAX_FAIL_COUNT 40			/**/
/* #define RESET_FAIL_COUNT			/**/


/* If UNIX_LASTLOG is defined, the program will update the lastlog entry so
 * that there is a record of the user having logged in.  This is important on
 * systems where you expire unused accounts and some users may only log in
 * via the web.  If you have the lastlog.h header file, define HAVE_LASTLOG_H.
 *
 * If you are using PAM to authentication out of something other than the
 * unix user database, then you should disable this.  The lastlog file is
 * index by the user's uid number, and users from other databases don't have
 * uid numbers.
 */

#define UNIX_LASTLOG		/**/
#define HAVE_LASTLOG_H		/**/


/* If NOLOGIN_FILE is defined to the full path name of a file, then the
 * existance of that file is checked on every login attempt.  If it exists
 * all logins to accounts with uid's of MIN_NOLOGIN_UID or more will be
 * rejected.
 *
 * If you are using PAM, then you should disable this.  In that case, this
 * check is better done through PAM, and the maximum uid check won't work
 * right with PAM.
 */

#define NOLOGIN_FILE "/etc/nologin"	/**/
#define MIN_NOLOGIN_UID 1		/**/


/* Defining CHECK_LOGIN_EXPIRATION and CHECK_PASSWORD_EXPIRATION causes
 * pwauth to check if the given login has expired, or it's password has
 * expired.  Most modern versions of Unix support these options.
 *
 * These checks have not been implemented for the SHADOW_AIX and SHADOW_HPUX
 * options.  It's probably possible to do so for SHADOW_HPUX, but lacking a
 * system to test on, I haven't bothered.
 */

#define CHECK_LOGIN_EXPIRATION		/**/
#define CHECK_PASSWORD_EXPIRATION	/**/


/* It is generally sensible to restrict what users can run pwauth.  Though
 * there are other programs that users can use to test if password guesses
 * are correct, like "su" and "passwd", pwauth is much easier to interface
 * a password guessing program to, so why not be paranoid and restrict it
 * as much as possible?
 *
 * If you are using pwauth with mod_auth_external, you will want to restrict
 * it to be runnable from whatever uid your httpd runs as.  (For apache, this
 * is determined by the "User" directive in your httpd.conf file.  It may be
 * called something like "nobody" or "httpd" or "apache".  Usually the easiest
 * way to figure it out is just to do a "ps" and see what most apache processes
 * are running as.)
 *
 * There are two ways to do this.  First, you can compile in the uid numbers
 * that are allowed to run this program, by listing them on the SERVER_UID
 * variable below.  At runtime, pwauth will check that the uid of the user
 * that invoked it is on this list.  So if you have just one uid that should
 * be able to run pwauth, you can say something like:
 *    #define SERVER_UIDS 72
 * If you have several, separate them by commas, like this:
 *    #define SERVER_UIDS 12,343,93
 * The root account can always run this program, so you don't have to
 * to list that.  If you do list it, it must be given last.
 *
 * The second option is to create a special group, called something like
 * "pwauth" for user id's that are allowed to run pwauth.  To do this, you
 * should compile pwauth with the SERVER_UIDS variable UNDEFINED.  This will
 * disable the runtime uid check.  Then, when you install the pwauth program,
 * set it's group ownership to the "pwauth" group, and permit it so that only
 * the owner and the group can run it.  Do not permit it to be executable to
 * others.  This has the advantage of not requiring a recompile if you want
 * to change the uid list.
 */

#define SERVER_UIDS 72		/* user "nobody" */


/* If MIN_UNIX_UID is defined to an integer, logins with uid numbers less than
 * that value will be rejected, even if the password is correct.
 *
 * If you are using PAM to authenticate against anything other than the local
 * unix password file, then leave this undefined (if you define it, only login
 * names which are in the local unix passwd file and have uid's greater the
 * given value will be accepted).
 */

#define MIN_UNIX_UID 500	/**/


/* If IGNORE_CASE is defined, the login given is checked in two different
 * ways. First without any changes and then with all letters converted to
 * lower case. This is useful for users accustomed to the Windows environment.
 */

/* #define IGNORE_CASE             /**/


/* If DOMAIN_AWARE is enabled, then we we check login names to see if they
 * contain a backslash, and discard anything up to and including the backslash.
 * This is for use in environments where there are windows users accustomed
 * to login names formed like "domain\username".
 */

/* #define DOMAIN_AWARE            /**/


/* On failed authentications, pwauth will sleep for SLEEP_TIME seconds, using
 * a lock on the file whose full path is given by SLEEP_LOCK to prevent any
 * other instances of pwauth from running during that time.  This is meant
 * to slow down password guessing programs, but could be a performance
 * problem on extremely heavily used systems.  To disable this, don't define
 * SLEEP_LOCK.  SLEEP_TIME defaults to 2 seconds if not defined.
 */

#define SLEEP_LOCK "/var/run/pwauth.lock"


/* If ENV_METHOD is defined, pwauth expects mod_auth_external to be configured
 *     SetAuthExternalMethod environment
 * instead of
 *     SetAuthExternalMethod pipe
 * This is insecure on some versions of Unixes, but might be a bit faster.
 */

/* #define ENV_METHOD		/**/


/* If /usr/include/paths.h exists define this.  Obviously I need to autoconfig
 * this.
 */

#define PATHS_H 		/**/
