/* This is an OS dependent, generated file */
/* Generated by Mark Murray */

#ifndef __ROKEN_H__
#define __ROKEN_H__

/* -*- C -*- */
/*
 * Copyright (c) 1995, 1996, 1997, 1998, 1999 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Kungliga Tekniska
 *      H�gskolan and its contributors.
 * 
 * 4. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* $Id: roken.h.in,v 1.113.2.1 1999/07/22 03:20:59 assar Exp $ */
/* $FreeBSD$ */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <paths.h>
#include <pwd.h>

#define ROKEN_LIB_FUNCTION

#include <roken-common.h>

int strcpy_truncate (char *dst, const char *src, size_t dst_sz);
int strcat_truncate (char *dst, const char *src, size_t dst_sz);

struct passwd *k_getpwnam (char *user);
struct passwd *k_getpwuid (uid_t uid);

const char *get_default_username (void);
time_t tm2time (struct tm tm, int local);
int unix_verify_user(char *user, char *password);
void inaddr2str(struct in_addr addr, char *s, size_t len);
void mini_inetd (int port);
int roken_concat (char *s, size_t len, ...);
size_t roken_mconcat (char **s, size_t max_len, ...);
int roken_vconcat (char *s, size_t len, va_list args);
size_t roken_vmconcat (char **s, size_t max_len, va_list args);
ssize_t net_write (int fd, const void *buf, size_t nbytes);
ssize_t net_read (int fd, void *buf, size_t nbytes);
int issuid(void);
int get_window_size(int fd, struct winsize *);
extern char *optarg;
extern int optind;
extern int opterr;
extern const char *__progname;
extern char **environ;

/*
 * kludges and such
 */

int roken_gethostby_setup(const char*, const char*);
struct hostent* roken_gethostbyname(const char*);
struct hostent* roken_gethostbyaddr(const void*, size_t, int);

#define roken_getservbyname(x,y) getservbyname(x,y)

#define roken_openlog(a,b,c) openlog(a,b,c)

void set_progname(char *argv0);

#endif /* __ROKEN_H__ */
