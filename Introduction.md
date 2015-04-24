# Introduction #

**pwauth** is a unix authentication checking tool, designed to be run by other programs that need to know if a login/password combination is valid.  It was originally designed to do web authentications, in combination with [mod\_auth\_external or mod\_authnz\_external](http://code.google.com/p/mod-auth-external/), but has been used in many other web applications and in mail servers.

What **pwauth** actually does is very simple.  You pass it a login and password, and it
returns a flag telling if they are valid.  Normally it checks against your Unix server's system password database, but it can also be configured to use PAM to authentication from a wide range of other sources.

# Why? #

On unix systems, checking the validity of a password normally requires root access, and all that tools that normally do that ("su", "login", "passwd", etc) run as root.  Normally if you wanted to check web passwords against that database you'd have to somehow give the http daemon access to the password database.  This would be a horrible idea from a security point of view, however, because any bug in that large complex program, or any of the programs it runs (like user CGI scripts) could open a hole through which your entire password database could be compromised.

**Pwauth** provides a way around that.  Instead of giving the http daemon access to the password database, you let it run **pwauth**, perhaps using mod\_auth\_external.  **Pwauth** is a much smaller program with a much narrower purpose, and is thus much easier to make secure.

# Typical Applications #

Typical applications for **pwauth** include:
  * _Basic Authentication for Web Applications_.
> "Basic Authentication" is the authentication system built into most browsers, where the browser provides a pop-up login box asking the user for their login/password and the authentication is negotiated between the user's browser and the http daemon. This is relatively rarely used on production websites these days. In this configuration pwauth is run by Apache through the mod\_auth\_external. It can be used to protect both CGI programs and static documents. CGIs do not have to do any authentication themselves - the name of the logged in users, if any, is passed to them by Apache.

  * _Form-Based Authentication for Web Applications_.
> In most modern websites authentication is not handled by the http daemon, but by the CGI programs running under it. In such cases, the login/password input boxes generally appear on the web page, and the data is passed to the CGI program, which does the check against the user database itself. Usually after the inital login the CGI program issues a session ID which is then sent back to the server with each new page request, so that we can tell that the user is already logged in. In this configuration, **pwauth** would be run directly by the CGI. Mod\_auth\_external is not needed.

  * _SMTP Authentication for Mail Servers_.
> The **pwauth** program has been used with the exim mail server to authenticate users.  See instructions [here](http://wiki.exim.org/AuthenticatedSmtpUsingPwauth).

  * _Many Others_
> Any time a non-root program wants to check the validity of an authentication, **pwauth** is likely to be a useful tool, so it has been used in a wide variety of applications, including [Hudson](http://wiki.hudson-ci.org/display/HUDSON/pwauth) and [MediaWiki](http://www.mediawiki.org/wiki/Extension:PwAuthPlugin).

# Authentication Sources #

**Pwauth** can check logins any of a variety of unix password database API's:

  * _Direct system password database access_.
> This uses the low-level system interface to the password database to check the password. Typically this means calling something like getpwnam() to look the given login name up the database, and then running crypt() on the given password to see if it matches the value in the database. The exact system calls vary significantly for different unix systems. Pwauth includes support for all popular systems and some unpopular ones. This is generally the best approach from a pure performance point of view.

  * _PAM authentication_.
> PAM (Portable Authentication Module) is a still higher level interface to the authentication system supported by some versions of Unix (notably Solaris, Linux and FreeBSD). With PAM there is a configuration file that specifies the authentication requirements for different programs, designating which dynamically loaded libraries to use to do the authentication. Authentication from the system password database is only one of many possibilities. Another common application is to use the pam\_smb module to do NT-style SMB authentication.

  * _Login.conf authentication_. (currently OpenBSD only)
> Some versions of Unix, mainly the BSD versions, have a "login.conf" file that can be used to configure authentication for different classes of users. Among other things, the configuration file specifies a program to be run to authenticate a user. For OpenBSD only, version 2.3.0 of pwauth adds the ability to read the login.conf file and authenticate users by the method specified there. Though substantially slower than directly accessing the password database, this configuration is useful if you are using unusual settings in "login.conf" and want pwauth to comply with them.

  * _AIX authentication_.
> The AIX operating system has it's own mechanism, similar to login.conf, that can be used to configure authentication for each user. Version 2.3.9 of pwauth adds the ability to authenticate through this interface.

# Features #

Other configurable features of pwauth include:

  * _Lastlog support_. Many Unix systems maintain a lastlog file that keeps record of the last login on each account. Pwauth can be configured to update this on each successful authentication.

  * _Restrictions on calling process uid_. Pwauth can be configured to only work for uids on a short list. Normally this would include only the uid that the HTTP daemon runs on. It will decline to run for any other users. This would mostly prevent users logged into the system from running the program directly - not that it would give them any information that the 'su' or 'passwd' programs wouldn't give them, but it is easier to interface a password guessing program to pwauth than to those, so restricting who can run it is useful.

  * _Restrictions on authentiated uids_. Pwauth is normally configured to reject all attempts to log into accounts with uids below some value. This prevents it's use on system accounts.

  * _Understand expired logins and passwords_. Most versions of Unix today include mechanisms where by login IDs can be set to expire after a certain date. There are also mechanisms by which passwords can be made to expire, so that the account becomes inaccessible if the password is not changed by that date. Pwauth version 2.3.0 is configured by default refuse all attempts to log into accounts that have expired logins or passwords. Of course, since it cannot send messages to the user, it cannot warn of impending expirations. This would have to be done by your CGIs.

  * _Understand /etc/nologin_. On most Unix systems, if you create a file named /etc/nologin then users will not be able to log in. The contents of the file are displayed to them instead. Pwauth version 2.3.0 is configured by default to refuse logins if that file exists, though it cannot, of course, display the contents. You can also configure it to use a different file, so that pwauth authentications can be disabled separately from regular logins.

  * _Sequentialized sleep after bad logins_. Normally you don't want authentication programs to be too fast, because that would allow people to zip through a dictionary of password guesses very quickly. Traditional Unix login programs sleep a bit before giving you a chance to try another password after one has failed. This is more complex to do with HTTP authentication because the user doesn't need to wait for the response of the first one to make another request - the same user can easily run many requests at the same time. In the default configuration, pwauth gets an exclusive lock on a file after every authentication request. On successful authentications, it drops the lock immediately, but on unsuccessful authentications it sleeps a few seconds before dropping it. Under normal usage, this should have no noticable effect, but after a failed authentication attempt, all pwauth processes on the system will be held up for a couple seconds. So even if a user tries barraging your system with password guesses, he will get responses back no faster than one every couple seconds. In the meantime, valid authentications could get very slow, and the system will accumulate a large number of processes blocked on the lock file. Admittedly not ideal. Maybe if I ever see this happen, I'll implement some better behavior for that case.

  * _Failure logs_. Basically, a failure log keeps count of how many consecutive bad logins are made, and on the next good login, reports the failure count, and resets it to zero. It may also disable login attempts if some limit is exceeded. Relatively few unix login programs do this, but, starting with version 2.3.0, pwauth can support this even if the regular login system does not. At least, it does the part about incrementing the count on bad logins and disabling accounts that exceed some limit. However there is no path by which the pwauth could report the bad login count to the user. If you want to report failed logins, the pwauth distribution includes a checkfaillog utility that can be run from a CGI after a successful login to report the failed logins and reset the count to zero.

  * _Suppression of core dumps_. On some versions of Unix core dump files can be publically readable. Since a core dump from pwauth would likely include a user's plain text password and possibly fragments of the system password file, this is undesirable. Pwauth disables core dumps.

Because in typical configurations pwauth is run very frequently (on each HTTP request to a protected page), all configuration is compiled in. This means it does not have to read and parse a configuration file on every run, improving performance. It does mean that you need to recompile pwauth every time you change it's configuration. (But, of course, it does not have to be recompiled if the PAM or login.conf configuration is changed.)

# Security Considerations #

I believe that pwauth, with mod\_auth\_external, is the most secure method for doing web authentication out of unix shadow password systems. Mod\_auth\_pam or mod\_auth\_system can also do this, but since they are internal authenticators, they will only work if you make the shadow password file readable to the http server. This means that if there are any exploitable vulnerabilities in the http server, then it may be possible for people to grab a copy of your shadow password file. Worse, any CGI program on your system which is not run under suExec or cgiwrap also has read access to your shadow password database, and any bugs in these might also expose your entire password database. When mod\_auth\_external and pwauth are used, neither the http server nor any CGI programs are given access to the shadow database. Only the pwauth program does. Since it is a small and simple program, it is much easier to assure that it does not have security weaknesses.

Having said that, web authentication from a Unix password file is an idea that many sensible people find [seriously questionable](Risks.md). We developed it for use on a system whose security concerns are [very unusual](http://www.cyberspace.org/staffnote/goals.html), and it has worked well for us over many years, in the face of fairly intense hacker activity.  Pwauth has features that can address mitigate many of the risks if correctly configured, but you need to be aware of the issues and extremely careful.

A fundamental security problem with web authentication is that the passwords are sent in clear text over the network. In the case of basic authentication, they are sent with every page request. Furthermore, with the http protocol, unlike a protocol like telnet, copies of passwords are likely to be cached in various places along the way. Though exploits based on this are rare, it is a fundamentally sloppy way to treat a password. I strongly recommend that pwauth be used with the https protocol, which encrypts all requests including the passwords, whenever possible.