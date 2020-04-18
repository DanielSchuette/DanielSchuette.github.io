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
#define FEQUALS(a, b)   ( ((a) - (b)) < EQUALS_CUTOFF )

typedef enum {
    START,
    PRE_DEC,
    POST_DEC,
    EXPONENT,
    INVALID,
    EMIT_RES,
} state; /* parse mode */

float parse_float(char *str)
{
    bool neg, exp_neg, run;
    char *ptr;
    float res, exp, prec;
    state state;

    res = exp = 0.0;
    prec = 1.0;
    neg = exp_neg = false;
    state = START;
    ptr = str;
    run = true;
    while (run) {
        switch (state) {
        case EMIT_RES:
            run = false;
            break;
        case INVALID:
            fprintf(stderr, "parse_float(): %s is not a valid input\n", str);
            run = false;
            break;
        case START:
            if (*ptr == '-') {
                neg = true;
                ptr++;
            }
            state++; /* from START, we always move to next state */
            break;
        case PRE_DEC:
            if (*ptr == '.') {
                state++;
                ptr++;
            } else if (IS_DIGIT(*ptr)) {
                res = res * 10.0 + (*ptr - ASCII_ZERO);
                ptr++;
            } else {
                state = INVALID;
            }
            break;
        case POST_DEC:
            if (*ptr == 'e' || *ptr == 'E') {
                state++;
                ptr++;
                if (*ptr == '-') {
                    exp_neg = true;
                    ptr++;
                }
            } else if (IS_DIGIT(*ptr)) {
                res = res * 10.0 + (*ptr - ASCII_ZERO);
                prec *= 10.0;
                ptr++;
            } else if (*ptr == '\0') {
                state = EMIT_RES;
            } else {
                state = INVALID;
            }
            break;
        case EXPONENT:
            if (IS_DIGIT(*ptr)) {
                exp = exp * 10.0 + (*ptr - ASCII_ZERO);
                ptr++;
            } else if (*ptr == '\0') {
                state = EMIT_RES;
            } else {
                state = INVALID;
            }
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
