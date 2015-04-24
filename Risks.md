## Security Risks ##

I believe that pwauth, if properly used, provides the most secure method of doing web authentication with system passwords.  However, many knowledgeable people would say that this is equivalent to talking about the safest way to shoot yourself in the head.  Using system accounts for web access can be an extremely bad idea.  You should understand why this is and think carefully about the risks before deciding if you want to do this.

System accounts are typically much more useful and powerful than the ordinary user accounts in web applications.  They can normally be used to log into the server and start an interactive command shell from which many system resources can be accessed and modified.  Since those users have access to hundreds of programs on your system, there is a good chance that they will be able to find a security hole in one of them that will let them escalate themselves to superuser on your server, at which point they own your server and all the data on it, including, with a little work, all your user's passwords.  System accounts are very high value accounts, and should be accorded extra protection.

These days most systems only allow system account logins via the SSH protocol, which is a very sophisticated protocol designed from the ground up for security.  The old TELNET and non-anonymous FTP protocols are rarely used anymore because they provide much less secure communications, with greater risk of a system account being compromised.  However, either one of those old abandoned protocols is far more secure than HTTP.  Here are a few of the weaknesses of HTTP:

  * **Promiscuous transmission of plain-text passwords.**  The old TELNET protocol used to send your plain-text password over the net just once, when you were logging in.  HTTP's basic authentication protocol sends it in plain-text in the header of every single request you make.

  * **Caching on intermediate servers.**  In the TELNET protocol, connections were direct from the user's computer to the server.  HTTP allows intermediate proxy servers to cache requests and the passwords with them, so your password may end up stored on all sorts of mysterious computers.

  * **Server Impersonation.**  The SSH protocol has elaborate checks built into it that try to ensure that you are giving your password to the real server and not an impostor.  HTTP has no such checks.  It is certainly possible to fool the user, and maybe possible to fool the browser into giving away passwords to fake servers.

  * **No speed limits on password guessing.**  If you enter a wrong password at a standard login prompt, there is typically a delay before you can try another.  The connection may be dropped after a few wrong guesses.  All this slows down programs that try using dictionary attacks to guess passwords.  In HTTP there are no such limitations.  You can send thousands of HTTP requests every second and keep doing it as long as you like.

  * **Difficulty of Monitoring.**  It's harder to notice password guessing or unauthorized accesses when they happen over the web, unless you are really alert in monitoring web access logs and error logs.

  * **Browser password caching.**  Most browsers practically beg you to let them remember your password for them.  No doubt some of your users will do this, and it is obviously possible for any virus or malicious user who gains access to their computer to extract those remembered passwords.  If one program (the browser) can get them, then other programs can be written to do the same thing.

  * **Browser vulnerabilities.**  Most TELNET clients were simple programs, fairly unbuggy and hard to exploit.  HTTP clients, on the other hand, are nightmarishly complex and notoriously riddled with security flaws.  Are you sure that nifty new plugin you installed last night doesn't quietly send copies of every password you enter to Nigeria?

  * **Lack of idle timeouts.**  If you wandered away from your computer, your TELNET connection would probably timeout after a while disconnecting you.  A basic authentication login _NEVER_ times out.  You can't even log out of it on most browsers except by completely exiting the browser.  Closing the window does not log you out.  Even closing all browser windows will not log you out on a Macintosh.  The risk of leaving active sessions laying around for other users who come to the computer later is high.

So, if this is such a bad idea, why did I ever write pwauth?  Well, the server I wrote it for, and the only one I ever used it on, was [Grex](http://cyberspace.org), an old public access Unix system which gives away free Unix shell accounts to anyone who asks for one.  On a server where system accounts are routinely given away to strangers, the whole security risk equation is obviously very different.

Of course, many of the concerns listed above can be mitigated by features built into pwauth, or by careful configuration and administration of the server.  You may have your own reasons why you are less concerned about the risks.  For example, I might be more comfortable using pwauth on an intranet than on the open net.  But you really should consider whether you should let your users have separate accounts for web access, with different logins and passwords, so that the consequences of one being compromised are not so great.

It should also be noted that in some cases these risks may be difficult to avoid.  If you have a set of users who access your server both via the web and via ssh, then you could have separate login/password databases for the different kinds of access, thus avoiding exposing system passwords on the web, but your users are very likely to undo your good work by choosing the same passwords in both databases, and grow more reluctant to change them periodically because of the annoyance of having to change them in two places.  (Though, come to think of it, you could have the password setting program from your web app, run pwauth to check that the password the user is setting for his web password does NOT match his system password.)

## Mitigating Risks ##

How can we counteract the risks described above?  Well, some are hard, and some are easy.

  * **Use HTTPS.**  Clearly requiring the secure https protocol for all authenticated transactions will help with many of these problem.  Passwords will still be sent in every query, but not in clear text.  Caching by proxies ceases to be a problem, and the risks of impostor servers are somewhat reduced.

  * **Serialized Sleep on Bad Logins.**  When an authentication fails with pwauth, it will grab an exclusive lock on a file and then sleep for a few seconds.  This causes all pwauth authentications on the server to be held up a bit, slowing things down for password guessers.  So instead of successfully guessing a password, an aggressive password guesser will probably just lock up your server.  Not ideal, but an improvement.  This can be disabled, but it is probably a bad idea to do so.

  * **Failure Logging.**  Pwauth can be configured to count the number of failed login attempts to each account since the last successful one, and to refuse further login attempts if the number goes too high. This is another way to limit password guessing.

  * **Don't Use Mod\_Authn\_Socache.** This module, new in Apache 2.4, is very tempting. It caches login credentials so that you don't have to keep re-running the authenticator over and over again for the same user. That's a big performance gain. But the cache it maintains is essentially an Apache-readable copy of your system password file, containing the encrypted passwords of your active users. Having that sitting around is only slightly less of a problem than making your password file readable to Apache would have been in the first place.

  * **Monitoring Logs.**  Many attacks against pwauth could be noticed early if you monitor the logs carefully, including the access logs, error logs and failure logs.  Ideally you would have tools in place that automatically detect attacks in progress so that defensive action can be taken quickly.

  * **Form-based Authentication.**  With basic authentication users cannot log off easily and there are no idle timeouts.  Using a [form-based authentication](FormAuth.md) approach can solve these problems, if done carefully.

All of these can help, but you shouldn't imagine that they completely solve the problem, and most of them increase the difficulty of setting up and maintaining the system.

On the other hand, since I wrote the first version of pwauth in 1996, I have never heard of a single incident in which someone successfully exploited it.  Probably that's because people have had the good sense not to use it on high-value servers that attract concerted attacks by skilled hackers.

So use your own judgement.  It's a useful tool, but not for every application.