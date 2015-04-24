# Installation Instructions #

Before installing pwauth, please be sure that you have read and understood the [Risks](Risks.md) page.  This is not a tool that is appropriate for every server.

## 1. Edit config.h ##

All configuration of pwauth is done via the config.h file.  You should set or check the following variables:

### 1.1. Authentication Method ###

What API should authentication be done through?  The most common options are:

  * SHADOW\_SUN:  This is the shadow password system in Solaris, Linux and IRIX 5.3 systems.  It uses getspnam() to fetch passwords and crypt() to encrypt them.

  * SHADOW\_BSD: This is the shadow password system in BSDI, NetBSD, OpenBSD, and FreeBSD.  This uses getpwnam() to fetch passwords and crypt() to encrypt them.  This would only work with OS X if the accounts being authenticated are configured with legacy crypt style passwords.  In general, the PAM option is more likely to be usable in OS X.

  * PAM: Talk to the authentication system through PAM - the plug-in authentication module interface.  This exists on Linux, Solaris 7, FreeBSD, and OS X.  You'll need to create /etc/pam.d/pwauth or edit /etc/pam.config to include entries for pwauth.  If you are using PAM to authenticate out of something you don't need to be root to access, then you might use instead Ingo Lutkebohle's mod\_auth\_pam.c module.  If you are using Solaris, you may need to use PAM\_SOLARIS or PAM\_SOLARIS\_26 instead to work around various bugs.

  * LOGIN\_CONF\_OPENBSD:  Many BSD derived systems use a login.conf file to configure authentication instead of (or in addition to) PAM.  We currently support authentication through this mechanism only for OpenBSD.  Of course, if you login.conf configuration is standard, you can just use SHADOW\_BSD, but if you want pwauth to respect settings in login.conf this option can be used instead.  The API used here, is however, pretty much unique to OpenBSD and will not work on NetBSD or FreeBSD.

There are several other more obscure methods used which are documented in the config.h file.

In most cases the method that you want if you just want to authenticate from the system password file is SHADOW\_SUN or SHADOW\_BSD depending on your Unix version.  PAM is slower and more complex to set up, but allows greater flexibility in configuration and allows you to authenticate from other sources.

### 1.2. Failure Logging ###

A failure log keeps a count of how many bad logins have been attempted for each user,
and disables logins to any account where some limit is exceeded.  This is useful to
prevent people from trying to guessing passwords by trying every word in the dictionary, but it can be a problem because it means anyone can disable any user account just by trying some bad passwords on it.

A few Unix systems, like OpenBSD, have failure logs for normal logins to the system, and pwauth can be configured to tie into them.  But it is also possible to set up pwauth to use it's own failure log even on systems that don't have one for normal logins.

If you choose to use this, it would probably be good to have your CGI's run the "checkfaillog" program after successful logins to report the count of bad logins and to reset the count to zero.

I think most installations don't use this and it should not be enabled with PAM, which performs this function at a lower level.


### 1.3. Lastlog Logging ###

Nearly all Unix systems maintain a lastlog file that keeps record of the last time that each user logged into the system.  It is possible to have pwauth update this when successful authentications occur.  This can be important on systems where you expire unused accounts and some users may only log in via the web.

This should not be enabled with PAM, where this is done at a lower level.

### 1.4. /etc/nologin ###

Most Unix systems will refuse logins if the file /etc/nologin exists.  It is normal to configure pwauth to also refuse logins if that file exists.  Alternately you could set it up to have a separate file to disable pwauth logins simply by giving a different path name in the configuration.  Again, this should not be used with PAM.

### 1.5. Login/Password Expiration ###

Most modern Unix systems allow administrators to set dates at which accounts expire and can no longer be used, or at which passwords expire and must be changed.  Sometimes administrators use these features to temporarily disable accounts that should not be usable.  So it is almost always advisable to have pwauth respect these settings.  If your administrators don't use these features, there is no harm, but if they do, they should be respected.

### 1.6. Who May Run Pwauth? ###

It is generally sensible to restrict what users can run pwauth.  Though there are other programs that users can use to test if password guessesare correct, like "su" and "passwd", pwauth is much easier to interface a password guessing program to, so why not be paranoid and restrict it as much as possible?

If you are using pwauth with mod\_auth\_external, you will want to restrict it to be runnable from whatever uid your httpd runs as.  (For apache, this is determined by the "User" directive in your httpd.conf file.  It may be called something like "nobody" or "httpd" or "apache".  Usually the easiest way to figure it out is just to do a "ps" and see what most apache processes are running as.)

There are two ways to do this.  First, you can compile in the uid numbers that are allowed to run this program, by listing them on the SERVER\_UID variable below.  At runtime, pwauth will check that the uid of the user that invoked it is on this list.  So if you have just one uid that should be able to run pwauth, you can say something like:
```
   #define SERVER_UIDS 72
```
If you have several, separate them by commas, like this:
```
   #define SERVER_UIDS 12,343,93
```
The root account can always run this program, so you don't have to to list that.  If you do list it, it must be given last.

The second option is to create a special group, called something like "pwauth" for user id's that are allowed to run pwauth.  To do this, you should compile pwauth with the SERVER\_UIDS variable UNDEFINED.  This will disable the runtime uid check.  Then, when you install the pwauth program, set it's group ownership to the "pwauth" group, and permit it so that only the owner and the group can run it.  Do not permit it to be executable to others.  This has the advantage of not requiring a recompile if you want to change the uid list.  If you are thinking of distributing pwauth as a precompiled package, then this might be your best option.

### 1.7. Minimum User Id ###

Typically we don't want to let people use pwauth to log into the "root" account or any other administrative accounts. Those administrative accounts usually have lower numbered user IDs than regular user accounts.  So you can defined MIN\_UNIX\_UID to reject any logins to accounts with user id numbers below a certain value.

### 1.8. Case Sensitivity and Domain Awareness ###

The IGNORE\_CASE flag causes every login ID to be checked two ways - first as given, then with all uppercase characters converted to lowercase.  This is useful if your site has many habitual Windows users who may be accustomed to login IDs not being case sensitive.  It really only "ignores case" if the actual IDs on your server are all lower case though.

In some environments you may also have windows users accustomed to login names formed like "domain\username".  In such cases, you can set the DOMAIN\_AWARE flag, and pwauth will check each login name to see if it contains a backslash, and, if so, discard the backslash and everything before it.

### 1.9. Sleep Lock ###

When you are logging into a unix system at the "Login:" prompt, you will notice that if you enter an incorrect password, then there is a delay of a few seconds before you are prompted for a new one.  This is to prevent password guessing programs from submitting huge numbers of password guesses quickly.  This is harder to do in a web environment, however, since many http requests can be made simultaneously.

Pwauth handles this problem by sleeping for SLEEP\_TIME seconds after each failed authentication, using a lock on the file whose full path is given by SLEEP\_LOCK to prevent any other instances of pwauth from running during that time.  The default settings for these options should be suitable for most systems.

In theory this could be a performance problem on extremely heavily used system, though I have never heard of anyone having problems with this.

### 1.10. Argument Passing ###

Normally the login and password are passed to pwauth through a pipe.  This method is secure on all versions of Unix.  It is possible to configure pwauth to instead take its arguments from environment variables, but this is not recommended because on some versions of Unix it is possible to other users to eavesdrop on a processes environment variable settings, and we do not want user's plain text passwords to be readily visible.

## 2. Edit Makefile ##

Edit the Makefile, and check the definitions of CC, LIB, and LOCALFLAGS.

LIB is the one most likely to give trouble.  The necessary libraries are likely to depend on the version of Unix you are using and on the configuration.  Library flags you might need include:
| -lcrypt | Linux with SHADOW\_SUN option |
|:--------|:------------------------------|
| -lpam   | Any Unix with PAM option |
| -ldl    | Linux with PAM (needed for dynamic loading of PAM modules |

## 3. Compile ##

The "make" command will compile the program.

There should be no error messages.

If there are error messages about undefined functions, then you may need to add some more libraries to the LIB definition in the Makefile.

If you are compiling a PAM version and get error messages about not being able to find header files like "pam\_appl.h" then you may need to load some PAM development module that isn't part of the standard installation.

## 4. PAM Configuration ##

Skip this section if you are not using PAM.

Every program that uses PAM needs a PAM configuration.  There are two common ways in which PAM configurations are arranged.

If you have a /etc/pam.d directory, then you will need to create a file named "pwauth" inside it.  The exact commands needed vary among different Unix and Linux distributions.  Looking at the other files in that directory can give you a lot of clues about what is needed.  There are a few really old examples listed here. For more, see the [InstallNotes](InstallNotes.md) page.

Under Redhat 6.x, I used this to authenticate out of the shadow password file:
<pre>
auth       required     /lib/security/pam_pwdb.so shadow nullok<br>
auth       required     /lib/security/pam_nologin.so<br>
account    required     /lib/security/pam_pwdb.so</pre>
Under OS X 10.4.11, something like the following works (possibly the pam\_securityserver line should be removed):
<pre>
auth       required     pam_nologin.so<br>
auth       sufficient   pam_securityserver.so<br>
auth       sufficient   pam_unix.so<br>
auth       required     pam_deny.so<br>
account    required     pam_permit.so</pre>

If you have a /etc/pam.conf file instead of a /etc/pam.d directory, then you need to
add appropriate lines to that file.  For Solaris 2.6, you need to add lines like this to authenticate out of the shadow password file:
<pre>
pwauth  auth     required  /lib/security/pam_unix.so<br>
pwauth  account  required  /lib/security/pam_unix.so</pre>
To authentication from an SMB server on Solaris 2.6, a line like this would be used:
<pre>
pwauth  auth    required  /lib/security/pam_smb_auth.so.1</pre>
You may want a "nolocal" flag on that line if you are authenticating from a remote server, or you may not.  Note that if you configure pam\_smb so that root access isn't required, you should be able to use mod\_auth\_pam instead of mod\_auth\_external and pwauth and get faster authentications.

## 5. Test Pwauth ##

The simplest test is to start a root shell and just run pwauth.  It will attempt to read the login and password from standard input, so type a login name, hit return, then type a password, and hit return (the password will echo on your screen).  The check the status code that was returned (in csh: "echo $status"  in sh: "echo $?").

If the login/password were correct you should get a zero status code.  If not, you will get some other value.  See the list of [status codes](StatusCodes.md) to find the meaning of the various values returned.  Any values 50 or greater indicate a configuration error.

## 6. Install Pwauth ##

Install it in some sensible place (say, /usr/local/libexec/pwauth).  Unless you are doing SHADOW\_NONE, it should be installed suid-root so that it has the necessary access to read the password database.  That is:
<pre>
chown root pwauth<br>
chmod u+s pwauth</pre>

If you are using the group method of regulating access to pwauth, then you should chgrp the installed file to whatever group you are using, and permit the file so the only the group can run it ("chmod 4710 pwauth").

You should probably do a final test, by su-ing to whatever user will be running pwauth (e.g. "httpd") and running it from that account.

## 7. Configure the Calling Program ##

You will probably need to do some configuration of whatever program is supposed to be invoking pwauth.

If that program is mod\_auth\_external or mod\_authnz\_external, then  [configuration instructions](InstallApache.md) are available here.