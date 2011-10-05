# The following three lines should be modified to appropriate values for your
# system.  Most of the configurable stuff is in config.h
#
#   CC=       an ansi-C compiler.  If "cc" doesn't work, try "gcc".
#   LIB=      libraries to link in.  -lcrypt, -lshadow, -lpam sometimes needed.
#   LOCALFLAGS=   compiler flags.  Usually -g, -O, and stuff like that.

# Settings for author's system (Redhat 6.1)
CC=gcc
LIB= -lcrypt
LOCALFLAGS= -g 

# For PAM on Redhat Linux
# LIB=-lpam -ldl

# For PAM on Solaris or OS X
# LIB=-lpam

# -------------------- No User Servicable Parts Below -----------------------

TAR= README INSTALL CHANGES FORM_AUTH Makefile main.c checkfaillog.c \
	fail_check.c fail_log.c lastlog.c nologin.c snooze.c auth_aix.c \
	auth_bsd.c auth_hpux.c auth_mdw.c auth_openbsd.c auth_pam.c \
	auth_sun.c config.h fail_log.h pwauth.h unixgroup

.PHONY: clean distclean

CFLAGS= $(LOCALFLAGS)

pwauth: main.o auth_aix.o auth_bsd.o auth_hpux.o auth_mdw.o auth_openbsd.o \
	auth_pam.o auth_sun.o fail_log.o lastlog.o nologin.o snooze.o
	$(CC) -o pwauth $(CFLAGS) main.o auth_aix.o auth_bsd.o auth_hpux.o \
		auth_mdw.o auth_openbsd.o auth_pam.o auth_sun.o fail_log.o \
		lastlog.o nologin.o snooze.o $(LIB)

checkfaillog: checkfaillog.o fail_check.o
	$(CC) -o checkfaillog $(CFLAGS) checkfaillog.o fail_check.o $(LIB)

main.o: main.c config.h pwauth.h fail_log.h
auth_aix.o: auth_aix.c config.h pwauth.h
auth_bsd.o: auth_bsd.c config.h pwauth.h
auth_hpux.o: auth_hpux.c config.h pwauth.h
auth_mdw.o: auth_mdw.c config.h pwauth.h
auth_openbsd.o: auth_openbsd.c config.h
auth_pam.o: auth_pam.c config.h pwauth.h
auth_sun.o: auth_sun.c config.h pwauth.h
checkfaillog.o: checkfaillog.c config.h fail_log.h
fail_check.o: fail_check.c config.h fail_log.h
fail_log.o: fail_log.c config.h pwauth.h fail_log.h
lastlog.o: lastlog.c config.h pwauth.h
nologin.o: nologin.c config.h pwauth.h
snooze.o: snooze.c config.h pwauth.h


clean:
	rm -f *.o

distclean:
	rm -f *.o pwauth checkfaillog

pwauth.tar: $(TAR)
	tar cvf pwauth.tar $(TAR)
