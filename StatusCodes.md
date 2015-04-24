# Status Codes #

[Pwauth](Introduction.md) will always exit with one of the status codes listed in the table below.

Codes 50 and above usually represent configuration errors in pwauth.
| **code** | **internal name** | **description** |
|:---------|:------------------|:----------------|
|  0  | STATUS\_OK | Login OK. |
|  1  | STATUS\_UNKNOWN | Nonexistant login or (for some configurations) incorrect password. |
|  2  | STATUS\_INVALID | Incorrect password (for some configurations).  |
|  3  | STATUS\_BLOCKED | Uid number is below MIN\_UNIX\_UID value configured in config.h. |
|  4  | STATUS\_EXPIRED | Login ID has expired. |
|  5  | STATUS\_PW\_EXPIRED | Login's password has expired. |
|  6  | STATUS\_NOLOGIN | Logins to system have been turned off (usually by /etc/nologin file). |
|  7  | STATUS\_MANYFAILES | Limit on number of bad logins exceeded. |
| 50  | STATUS\_INT\_USER | pwauth was invoked by a uid not on the SERVER\_UIDS list.  If you get this error code, you probably have SERVER\_UIDS set incorrectly in pwauth's config.h file. |
|  51 | STATUS\_INT\_ARGS | pwauth was not given a login & password to check.  The means the passing of data from mod\_auth\_external to pwauth is messed up.  Most likely one is trying to pass data via environment variables, while the other is trying to pass data via a pipe. |
|  52 | STATUS\_INT\_ERR | one of several rare and unlikely internal errors occurred.  You'll have to read the source code to figure these out. |
|  53 | STATUS\_INT\_NOROOT | pwauth was not able to read the password database.  Usually this means it is not running as root.  (PAM and login.conf configurations will return 1 in this case.) |

If you see a negative error code (or a number like 255 or 254) reported in the Apache error log, then that is an error code from `mod_authnz_external` and means that it was unable to run pwauth at all.  See the [mod\_auth\_external documentation](http://code.google.com/p/mod-auth-external/wiki/Configuration) for more information.