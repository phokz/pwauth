# Configuring Apache #

If  you want to use pwauth for basic authentication in Apache, then you will need to use mod\_authnz\_external or, if you are using an old version of Apache, mod\_auth\_external.

## 1. Install the module ##

You need version 2.1.1 or later of [mod\_auth\_external](http://code.google.com/p/mod-auth-external/) or [mod\_authnz\_external](http://code.google.com/p/mod-auth-external/).  See the installation instructions in that distribution for details.

## 2. Server Configuration ##

Find the Apache server configuration file.  In the old days it was called httpd.conf.  These days it may be a directory full of miscellaneous files and you may have to do some digging to find the right place to insert local configuration commands.  In OpenSuse, for example, you'd create a new file under /etc/apache2/conf.d containing these commands.

Insert the following lines:
```
AddExternalAuth pwauth /usr/local/libexec/pwauth
SetExternalAuthMethod pwauth pipe
```
Obviously if you used a different path for pwauth, you should use that path instead.

## 3. Directory Configuration ##

Put an .htaccess file in whatever directory you want to protect. (For .htaccess files to work, you may need to change some "`AllowOverride None`" directives in your httpd.conf file into "`AllowOverride AuthConfig`" directives).

A typical .htaccess file using mod\_authnz\_external would look like:
```
AuthType Basic
AuthName Your-Site-Name
AuthBasicProvider external
AuthExternal pwauth
require valid-user
```
A typical .htaccess file using mod\_auth\_external would look like:
```
AuthType Basic
AuthName Your-Site-Name
AuthExternal pwauth
require valid-user
```

Alternately, you could put these directives in a `<Directory>` block in your httpd.conf file.

If you want to allow users of only certain Unix groups to login, the perl "unixgroup" command included in this directory will do the job, though not very efficiently.  If you are using mod\_authnz\_external, a better approach is to use [mod\_authz\_unixgroup](http://unixpapa.com/mod_authz_unixgroup/).  This will not only allow you to restrict logins to users in particular groups, but restrict access to individual files based on group ownership of the files, if used with the standard Apache module mod\_authz\_owner.

## 4. Test ##

Test it by trying to access a file in the protected directory with your web browser.

If it fails to accept correct logins, then check Apache's error log file.  This should give some messages explaining why authentication failed.

If it was unable to execute pwauth, check that the pathnames and permissions are all correct.

If it says that pwauth failed, it will give the numeric status code that pwauth returned.
The [status code page](StatusCodes.md) tells the meaning of all these codes.