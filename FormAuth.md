# Pwauth with Form Authentication #

Although pwauth was designed for use with the mod\_auth\_external Apache module to do "Basic Authentication", it can also be used with "Form Authentication".

In "Form Authentication" you display a login form in HTML, like
```
<form action="login.cgi" method="post">
Login:    <input type="text" name="login"><br>
Password: <input type="password" name="passwd"><br>
<input type="submit" value="Login Now">
</form>
```

When a person submits this form, the "login.cgi" program gets run.  It checks the login and password, and if they are correct, initiates a session for the user.  See [my authentication pages](http://unixpapa.com/auth/) for more information about this, including explanations about why it is important for good security to use `method="post"`, and to turn off caching both on the login form page and on the first page transmitted after a successful login.

It is possible to use pwauth (or any other authenticator written for mod\_auth\_external) with this kind of authentication system.  All you have to do is have your CGI program run pwauth when it wants to check the password.

Here's a sample function in Perl that does exactly this.  The 'trypass()' function will return the same [status codes](StatusCodes.md) that pwauth does (_ie_, zero on success, other values on failure).
```
$pwauth_path= "/usr/local/libexec/pwauth";

sub trypass {
   my $userid= $_[0];
   my $passwd= $_[1];

   open PWAUTH, "|$pwauth_path" or die("Could not run $pwauth_path");
   print PWAUTH "$userid\n$passwd\n";
   close PWAUTH;
   return !$?;
}
```

Obviously the `$pwauth_path` should be defined to wherever you install pwauth, and the `die()` call should be replaced with whatever is an appropriate way to handle a fatal error in your CGI program.

Note that pwauth must be configured so that SERVER\_UIDS includes whatever uid your CGI program runs as.  Normally this is the same user ID that httpd runs as, but if your CGIs are running under suExec, then you may need to include other uid numbers.

You may want to examine the return code from pwauth more carefully than is done in this example, so that you can tell the user if his login was rejected due to logins being turned off, his account being expired, or his password being expired.  Though in some configurations pwauth will return different return codes for bad password and bad login name, it is generally considered good practice NOT to tell the user which of these two occurred.

From a security point of view, form-based authentication has pluses and minuses.  Mostly in such approaches you will be assigning session IDs to logged in users (see my [authentication guide](http://www.unixpapa.com/auth/) for details).  This has the advantage that you can provide logout buttons and idle timeouts, neither of which is possible with basic authentication.  The ability for users to log out is a considerable improvement to security. However, those session IDs are frequently passed around in cookies, which can have some security problems if you ever have untrusted users on your server.  Cookies are passed to CGI programs in environment variables, and many older Unix systems allow other users to see a process's environment variables.