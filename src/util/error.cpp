#include "error.h"

/*
 * Print a message and return to caller.
 * Caller specifes "errnoflag"
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap) {
    char buf[MAXLINE];
    vsnprintf(buf, MAXLINE, fmt, ap);
    if (errnoflag)
        snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s",
            strerror(error));
    strcat(buf, "\n");
    fflush(stdout);    /* in case stdout and stderr are the same */
    fputs(buf, stderr);
    fflush(NULL);    /* flushes all stdio output streams */
}

/*
 * Fatal error
 * Print a message and terminate.
 */
void err_quit(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
    exit(1);
}

/* 
 * Log system
 * Print a message and return.
 */
void log_msg(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
}
