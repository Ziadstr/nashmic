/*
 * NashmiC — utf8.c
 * UTF-8 decoder/encoder
 */

#include "utf8.h"

int utf8_seq_len(unsigned char byte) {
    if (byte < 0x80) return 1;
    if ((byte & 0xE0) == 0xC0) return 2;
    if ((byte & 0xF0) == 0xE0) return 3;
    if ((byte & 0xF8) == 0xF0) return 4;
    return 1; /* invalid — treat as single byte */
}

uint32_t utf8_decode(const char *src, size_t len, size_t *pos) {
    if (*pos >= len) return 0;

    unsigned char c = (unsigned char)src[*pos];
    uint32_t cp;
    int seq;

    if (c < 0x80) {
        cp = c;
        seq = 1;
    } else if ((c & 0xE0) == 0xC0) {
        cp = c & 0x1F;
        seq = 2;
    } else if ((c & 0xF0) == 0xE0) {
        cp = c & 0x0F;
        seq = 3;
    } else if ((c & 0xF8) == 0xF0) {
        cp = c & 0x07;
        seq = 4;
    } else {
        (*pos)++;
        return 0xFFFD;
    }

    for (int i = 1; i < seq; i++) {
        if (*pos + i >= len) {
            (*pos)++;
            return 0xFFFD;
        }
        unsigned char next = (unsigned char)src[*pos + i];
        if ((next & 0xC0) != 0x80) {
            (*pos)++;
            return 0xFFFD;
        }
        cp = (cp << 6) | (next & 0x3F);
    }

    *pos += seq;
    return cp;
}

int utf8_encode(uint32_t cp, char *buf) {
    if (cp < 0x80) {
        buf[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800) {
        buf[0] = (char)(0xC0 | (cp >> 6));
        buf[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp < 0x10000) {
        buf[0] = (char)(0xE0 | (cp >> 12));
        buf[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        buf[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    buf[0] = (char)(0xF0 | (cp >> 18));
    buf[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
    buf[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
    buf[3] = (char)(0x80 | (cp & 0x3F));
    return 4;
}
