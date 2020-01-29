#ifndef ERROR_H
#define ERROR_H

#include <errno.h>
#include <stdarg.h>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

#define MAXLINE 4096

static void err_doit(int, int, const char *, va_list);
void err_quit(const char *fmt, ...);
void log_msg(const char *fmt, ...);

#endif