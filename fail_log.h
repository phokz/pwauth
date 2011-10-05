/* For now, FAILLOG_PWAUTH is just FAILLOG_OPENBSD */
#ifdef FAILLOG_PWAUTH
# define FAILLOG_OPENBSD
# ifndef PATH_FAILLOG
#  define PATH_FAILLOG "/var/log/pwauth.faillog"
# endif
#endif

#ifdef FAILLOG_JFH
# define KEEP_FAILLOG
# include "faillog.h"
# ifndef NEED_UID
#  define NEED_UID
# endif
# ifndef PATH_FAILLOG
#  define PATH_FAILLOG FAILFILE
# endif
#endif

#ifdef FAILLOG_OPENBSD
# define KEEP_FAILLOG

/* The following is clipped from OpenBSD 3.5 src/usr.bin/login/failedlogin.c
 * If you are actually using this on OpenBSD to update the same faillog file
 * that is used by the login program, then it is important that this
 * definition match the definition there.
 */

struct badlogin {
    char    bl_line[UT_LINESIZE];   /* tty used */
    char    bl_name[UT_NAMESIZE];   /* remote username */
    char    bl_host[UT_HOSTSIZE];   /* remote host */
    time_t  bl_time;                /* time of the login attempt */
    size_t  count;                  /* number of bad logins */
};

# ifndef NEED_UID
#  define NEED_UID
# endif
# ifndef PATH_FAILLOG
#  ifdef _PATH_FAILEDLOGIN
#   define PATH_FAILLOG _PATH_FAILEDLOGIN
#  else
#   define PATH_FAILLOG "/var/log/failedlogin"
#  endif
# endif
#endif

