# pwauth #
**Author and maintainer**: [Jan Wolter](http://unixpapa.com/) ([email](http://unixpapa.com/white.cgi))

**Pwauth** is an authenticator designed to be used with [mod\_auth\_external or mod\_authnz\_external](http://code.google.com/p/mod-auth-external/) and the Apache HTTP daemon to support reasonably secure web authentication out of the system password database on most versions of Unix.

What pwauth actually does is very simple: given a login and a password, it returns a status code indicating whether it is a valid login/password or not. It is normally installed as an suid-root program, so other programs (like Apache or a CGI program) can run it to check if a login/password is valid even though they don't themselves have read access to the system password database.

Though originally designed for web authentication, it can be used for other authentication applications, such as with the [exim](http://wiki.exim.org/AuthenticatedSmtpUsingPwauth) mail server.

People considering use of pwauth should be aware that there are innate security risks when you allow system passwords to be used on the web.  Please be sure to read the wiki page on [security risks](Risks.md).