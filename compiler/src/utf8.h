/*
 * NashmiC — utf8.h
 * UTF-8 decoding utilities for Arabic + Latin dual-mode lexing
 */

#ifndef NSH_UTF8_H
#define NSH_UTF8_H

#include <stdint.h>
#include <stddef.h>

/* Decode one UTF-8 codepoint from src, advance *pos, return codepoint.
 * Returns 0xFFFD (replacement char) on invalid sequences. */
uint32_t utf8_decode(const char *src, size_t len, size_t *pos);

/* Encode a codepoint into buf (must have 4 bytes). Returns bytes written. */
int utf8_encode(uint32_t cp, char *buf);

/* Return byte length of UTF-8 sequence starting at src[0]. */
int utf8_seq_len(unsigned char byte);

#endif /* NSH_UTF8_H */
