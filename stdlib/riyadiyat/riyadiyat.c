/*
 * NashmiC Standard Library — riyadiyat (Math)
 */

#include "riyadiyat.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

double nsh_jadhr(double x) { return sqrt(x); }
double nsh_qowa(double x, double n) { return pow(x, n); }
double nsh_mutlaq(double x) { return fabs(x); }

int64_t nsh_ashwa2i(int64_t min, int64_t max) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }
    if (min > max) {
        int64_t tmp = min;
        min = max;
        max = tmp;
    }
    return min + (int64_t)(rand() % (max - min + 1));
}

double nsh_jeta(double x) { return sin(x); }
double nsh_jeta_tamam(double x) { return cos(x); }
double nsh_dal(double x) { return tan(x); }
double nsh_ardiye(double x) { return floor(x); }
double nsh_sa2fiye(double x) { return ceil(x); }
double nsh_da2reb(double x) { return round(x); }
double nsh_aqall(double a, double b) { return (a < b) ? a : b; }
double nsh_akthar(double a, double b) { return (a > b) ? a : b; }
double nsh_log_tabi3i(double x) { return log(x); }
double nsh_log10_nsh(double x) { return log10(x); }
