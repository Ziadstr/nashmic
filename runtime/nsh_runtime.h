/*
 * NashmiC Runtime — nsh_runtime.h
 * Core runtime + cultural easter eggs
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

/* ── Easter Egg Functions ────────────────────────────────────── */

/* drobi — random Jordanian proverb/joke */
void nsh_drobi(void);

/* mansaf — mansaf recipe in ASCII art */
void nsh_mansaf(void);

/* sahteen — "bon appetit" (used after test runs pass) */
void nsh_sahteen(void);

/* nashmi — NashmiC manifesto/about */
void nsh_nashmi(void);

#endif /* NSH_RUNTIME_H */
