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

/* ── Array Helpers ───────────────────────────────────────────── */

/* Generic array struct — codegen emits typed versions, but runtime
 * helpers operate on void* data with element size. */

/* nsh_array_push — append an element to the array (grows if needed).
 * data: pointer to the data pointer (T**)
 * len:  pointer to the length
 * cap:  pointer to the capacity
 * elem: pointer to the element to copy
 * elem_size: sizeof(T) */
void nsh_array_push(void **data, int64_t *len, int64_t *cap,
                    const void *elem, int64_t elem_size);

/* nsh_array_len — return the length (alias for .len field access) */
int64_t nsh_array_len(int64_t len);

/* nsh_array_free — free the data pointer */
void nsh_array_free(void *data);

/* ── String Helpers ──────────────────────────────────────────── */

/* nsh_str_concat — concatenate two strings, returns malloc'd result */
char *nsh_str_concat(const char *a, const char *b);

/* nsh_str_len — return string length */
int64_t nsh_str_len(const char *s);

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
