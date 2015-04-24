# Introduction #

This page includes notes on installing pwauth on various versions of Unix. These notes are mostly user contributed. If you have information to add, feel free to edit this page.

# OS X #

## OS X - Version 10.6.6 ##

From Micah R Ledbetter:

  * Use the regular PAM option not the PAM\_OS\_X or PAM\_OLD\_OS\_X option. Those apply only to versions 10.5 and before.

  * The Mac OS X apache user id uid 70.

  * I copied the contents of the pre-existing /etc/pam.d/sshd file into /etc/pam.d/pwauth, and it worked just fine.
```
      auth       optional       pam_krb5.so
      auth       optional       pam_mount.so
      auth       sufficient     pam_serialnumber.so serverinstall legacy
      auth       required       pam_opendirectory.so
      account    required       pam_nologin.so
      account    required       pam_sacl.so sacl_service=ssh
      account    required       pam_opendirectory.so
      password   required       pam_opendirectory.so
      session    required       pam_launchd.so
      session    optional       pam_mount.so
```