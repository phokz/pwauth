			       pwauth 2.3.10

                             Author: Jan Wolter

		      http://www.unixpapa.com/pwauth/

Pwauth is a conceptually a simple program.  You run it, giving it a login
and a password, and it returns a status code telling whether or not that
login/password is valid.  It is designed to be combined with mod_auth_external
(or mod_authnz_external) and Apache to give reasonably secure HTTP
authentication from a Unix password file, though it can be used in other ways
too.

Mod_auth_external and mod_authnz_external are available from
http://www.unixpapa.com/mod_auth_external/

Pwauth ends up being slightly more complex because of the lack of consistancy
in the way different versions of Unix do authentication.  It includes code
for doing low-level authentication in most different versions of Unix.  It
also can be configured to use one higher-level interface to authentication,
PAM.  All configuration is compiled in, because in typical applications
this program runs very frequently (on every web hit on a protected page),
so the cumulative overhead of reading a config file on every run would be
substantial.

I believe that mod_auth_external, with the included pwauth program, is the
most secure method for doing web authentication out of unix shadow password
systems. Mod_auth_pam or mod_auth_system can also do this, but since they
are internal authenticators, they will only work if you make the shadow
password file readable to the http server. This means that if there are
any exploitable vulnerabilities in the http server, then it may be possible
for people to grab a copy of your shadow password file. Worse, any CGI
program on your system which is not run under suExec or cgiwrap also has
read access to your shadow password database, and any bugs in these might
also expose your entire password database. When mod_auth_external and pwauth
are used, neither the http server nor any CGI programs are given access to
the shadow database. Only the "pwauth" program does. Since it is a small
and simple program, it is much easier to assure that it does not have
security weaknesses.

Having said that, authenticating from a Unix password file is an idea that
many sensible people find seriously questionable.  See Apache's FAQ
(http://httpd.apache.org/docs/misc/FAQ-G.html#passwdauth) for a overview
of some of the issues.  Pwauth has features that can address most of the
arguments made here, if correctly configured, but you need to be aware of
the issues and extremely careful.  I've used it for many years without
problems on systems that are under almost continuous assault by hackers,
but none of those systems are at all typical in their security requirements.
You should think hard about using this software and proceed with caution.

Installation instructions are in the INSTALL file.  The FORM-AUTH file
discusses using this in form-based authentication applications.  Configuration
information is in the comments in the "pwauth.h" file.

Versions of pwauth before version 2.2.8 were distributed as part of the
mod_auth_external distribution.

Author and Maintainer:  Jan Wolter  http://www.unixpapa.com/
