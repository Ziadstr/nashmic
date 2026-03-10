/*
 * NashmiC — diagnostics.h
 * Error/warning formatting with Jordanian proverbs and colors
 */

#ifndef NSH_DIAGNOSTICS_H
#define NSH_DIAGNOSTICS_H

#include "span.h"

/* Report an error at a source span */
void diag_error_at(const char *filename, const char *source,
                   NshSpan span, const char *message);

/* Report a warning at a source span */
void diag_warning_at(const char *filename, const char *source,
                     NshSpan span, const char *message);

/* Report a generic error (no source location) */
void diag_error(const char *message);

/* Get the total error count */
int diag_error_count(void);

/* Reset error count */
void diag_reset(void);

#endif /* NSH_DIAGNOSTICS_H */
