#define _GNU_SOURCE
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ASCII_ZERO      48
#define EQUALS_CUTOFF   1e-6

#define ARR_LEN(x)      ( sizeof((x)) / sizeof((x[0])) )
#define IS_DIGIT(x)     ( (x) >= '0' && (x) <= '9' )
#define ASSERT_DIGIT(x) ( assert(IS_DIGIT((x))) )
#define FEQUALS(a, b)   ( ((a) - (b)) < EQUALS_CUTOFF )

typedef enum {
    BEFORE_DEC,
    AFTER_DEC,
    EXPONENT,
} pmode; /* parse mode */

float parse_float(char *str)
{
    bool neg, exp_neg;
    char prev, *ptr;
    float res, exp, prec;
    pmode mode;

    /* initialize variables */
    res = exp = 0.0;
    prec = 1.0;
    neg = exp_neg = false;
    mode = BEFORE_DEC;
    for (ptr = str; *ptr != '\0'; ptr++) {
        switch (mode) {
        case BEFORE_DEC:
            if (*ptr == '.') {
                mode = AFTER_DEC;
                continue;
            }
            if (ptr == str && *ptr == '-') {
                neg = true;
                continue;
            }
            ASSERT_DIGIT(*ptr);
            res = res * 10.0 + (*ptr - ASCII_ZERO);

            break;
        case AFTER_DEC:
            if (*ptr == 'e' || *ptr == 'E') {
                mode = EXPONENT;
                continue;
            }
            ASSERT_DIGIT(*ptr);
            res = res * 10.0 + (*ptr - ASCII_ZERO);
            prec *= 10.0;

            break;
        case EXPONENT:
            prev = *(ptr-1);
            if ((prev == 'e' || prev == 'E') && *ptr == '-') {
                exp_neg = true;
                continue;
            }
            ASSERT_DIGIT(*ptr);
            exp = exp * 10.0 + (*ptr - ASCII_ZERO);

            break;
        default:
            exit(1); /* should never be reached */
        }
    }

    if (neg) res *= -1;
    res /= prec;
    if (exp_neg) exp *= -1;
    exp = exp10((double)exp);
    res *= exp;

    return res;
}

int main(int argc, char **argv)
{
    int i;
    float f, solutions[] = { 11.3, 119.312, 114.3, .3, 114.,
                             -11.3, -119.312, -114.3, -.3, -114.,
                             11.3e2, 119.312e-2, 114.3e12, .3e-3, 114.e1,
                             -11.3e2, -119.312e-2, -114.3e12, -.3e-3, -114.e1 };
    char *tests[] = { "11.3", "119.312", "114.3", ".3", "114.",
                      "-11.3", "-119.312", "-114.3", "-.3", "-114.",
                      "11.3e2", "119.312e-2", "114.3e12", ".3e-3", "114.e1",
                      "-11.3e2", "-119.312e-2", "-114.3e12", "-.3e-3",
                      "-114.e1" };

    fprintf(stderr, "%s:\n%15s%25s\n", *argv, "string", "float");
    for (i = 0; i < ARR_LEN(tests); i++) {
        f = parse_float(tests[i]);
        fprintf(stderr, "%15s%25.5f\n", tests[i], f);
        assert(FEQUALS(f, solutions[i]));
    }

    return 0;
}
