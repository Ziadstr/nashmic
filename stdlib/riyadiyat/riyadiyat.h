/*
 * NashmiC Standard Library — riyadiyat (Math)
 * Thin wrappers around <math.h> with Franco-Arab names.
 */

#ifndef NSH_RIYADIYAT_H
#define NSH_RIYADIYAT_H

#include <stdint.h>

#define NSH_PI 3.14159265358979323846
#define NSH_E  2.71828182845904523536

double nsh_jadhr(double x);
double nsh_qowa(double x, double n);
double nsh_mutlaq(double x);
int64_t nsh_ashwa2i(int64_t min, int64_t max);
double nsh_jeta(double x);
double nsh_jeta_tamam(double x);
double nsh_dal(double x);
double nsh_ardiye(double x);
double nsh_sa2fiye(double x);
double nsh_da2reb(double x);
double nsh_aqall(double a, double b);
double nsh_akthar(double a, double b);
double nsh_log_tabi3i(double x);
double nsh_log10_nsh(double x);

#endif /* NSH_RIYADIYAT_H */
