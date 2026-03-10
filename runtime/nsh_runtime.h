/*
 * NashmiC Runtime — nsh_runtime.h
 * Core runtime: init, shutdown, itba3 (print)
 */

#ifndef NSH_RUNTIME_H
#define NSH_RUNTIME_H

#include <stdint.h>

/* Initialize the NashmiC runtime. Called before main(). */
void nsh_runtime_init(void);

/* Shutdown the NashmiC runtime. Called after main(). */
void nsh_runtime_shutdown(void);

/* itba3 — print to stdout (اطبع)
 * Supports format strings like printf. */
void nsh_itba3(const char *fmt, ...);

/* i2ra — read line from stdin (اقرأ) */
char *nsh_i2ra(void);

/* itla3 — exit the program (اطلع) */
void nsh_itla3(int code);

#endif /* NSH_RUNTIME_H */
