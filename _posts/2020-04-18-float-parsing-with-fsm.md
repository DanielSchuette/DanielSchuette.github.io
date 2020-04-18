---
layout: post
title:  "Float Parsing Using a Finite-State Machine"
date:   2020-04-18 08:55:00 +0200
categories: parsing
---

In this post, we'll examine an algorithm for parsing a floating point number from its string representation. Whereas it's fairly easy to parse an integer from a string[^1], most programming languages support multiple ways of expressing floats. Thus, parsing them is a little more difficult and a quite interesting programming exercise. Take a look at the following array of strings:

```c
char *tests[] = { "11.3", "119.312", "114.3", ".3", "114.",
                  "-11.3", "-119.312", "-114.3", "-.3", "-114.",
                  "11.3e2", "119.312e-2", "114.3e12", ".3e-3", "114.e1",
                  "-11.3e2", "-119.312e-2", "-114.3e12", "-.3e-3",
                  "-114.e1" };
```

They all represent valid floating point numbers. It's obvious that we have to organize our parser well to ensure that we correctly recognize of all the different formats above. Finite-state machines --- or FSMs for short --- are a tool well-suited for this task, so we'll use them to implement a simple, but robust (and extensible[^2]) float parser.

# Finite-state machines

Finite-state machines are a [mathematical model of computation](https://en.wikipedia.org/wiki/Finite-state_machine), in which an abstract machine moves through a certain number of distinct states. States can be associated with certain actions and that's how FSMs produce output. Transitions between states are triggered by inputs to the machine. It's not very important to go deep into theory to apply the concept of a finite-state machine to our problem, so we'll just demonstrate of what practical use FSMs can be.

# State diagram

As we said above, an FSM can only be in a certain number of states. Thus, the first thing we do is define our states:

```c
typedef enum {
    START,      /* start state */
    PRE_DEC,    /* currently parsing before decimal point */
    POST_DEC,   /* currently parsing after decimal point*/
    EXPONENT,   /* currently parsing exponent */
    INVALID,    /* failure state */
    EMIT_RES,   /* success state */
} state;        /* possible states of our FSM */
```

We'll feed one character of the input string after another into our machine, moving from `PRE_DEC` to `POST_DEC` to `EXPONENT`. The state diagram[^3] below depicts, under which conditions transitions occur. When we encounter a `'.'`, we are moving from the `PRE_DEC` to the `POST_DEC` state (`DEC` means decimal, if you haven't guessed already). When we're in the `POST_DEC` state and find an `'e'` or `'E'`, we transition to `EXPONENT`.

![State diagram](../../../../assets/fsm_diagram_float.png)

The diagram also shows which error conditions might occur. First, parsing of an invalid character means failure. If we reach the end of the input and are still in `PRE_DEC`, that's also an error since floats **must** have a decimal point (at least in our implementation). As soon as we're in `POST_DEC`, an end of input means success, though.

To have our machine actually produce output, we associate state transitions with *entry actions*. In our case, we check for a `'-'` on some transitions. It must be emphasized that we only consider the transition from one state to another as a valid trigger for an entry action. Such actions are labeled with *E* in the graph. The actions we perform when we stay in a certain state are marked with *R* (for *repeated* action). They occur whenever we encounter a character in the inclusive range `'0'` to `'9'`. In the next section, we will take a look at the code to understand this somewhat abstract description even better and to see what these mysterious actions actually are.

As a last remark --- to avoid parsing errors with FSMs, it is important to associate a transition with every possible input value. If the machine doesn't know what to do with a particular input, something's gone wrong. Such a transition doesn't necessary need to go to another state, though. It can go back to the same state as well.

# Implementation
```c
float parse_float(char *str)
{
    bool neg, exp_neg, run;
    char *ptr;
    float res, exp, prec;
    state state;
```
First, we define a number of variables. `neg` and `neg_exp` are set if our machine encounters a `'-'` in `PRE_DEC` or `EXPONENT`. `run` is initially `true` and set to `false` if the machine has stopped --- either due to successful parsing or if an error occurred. `res`, `exp` and `prec` store the values of the result, exponent, and precision, respectively. Details follow in a bit.

```c
    res = exp = 0.0;
    prec = 1.0;
    neg = exp_neg = false;
    state = START;
    ptr = str;
    run = true;
```
Then, all variables are initialized. Incrementing `ptr` means fetching another input value.

```c
    while (run) {
        switch (state) {
        case EMIT_RES:
            run = false;
            break;
        case INVALID:
            fprintf(stderr, "parse_float(): %s is not a valid input\n", str);
            run = false;
            break;
```
The entire FSM is a single `switch` statement. Depending on what state we're in, we are going to perform the correct action, change states if required and fetch the next input, if we need to. `EMIT_RES` and `INVALID` are the easiest cases to handle. They simply halt the machine by setting `run` to `false`.

```c
        case START:
            if (*ptr == '-') {
                neg = true;
                ptr++;
            }
            state++; /* from START, we always move to next state */
            break;
```
Since we cannot know in most states what the previous state was, we need a `START` state. In it, we check if an entry action for `PRE_DEC` needs to be done and if so, set `neg`. Only then the next input needs to be fetched.

```c
        case PRE_DEC:
            if (*ptr == '.') {                  /* transition to next state */
                state++;
                ptr++;
            } else if (IS_DIGIT(*ptr)) {    /* transition within this state */
                res = res * 10.0 + (*ptr - ASCII_ZERO); /* parse this digit */
                ptr++;
            } else {    /* no valid input char, transition to failure state */
                state = INVALID;
            }
            break;
```
`PRE_DEC` implements its specification in the most obvious way possible. See the comments for some explanations. It is absolutely possible to rewrite the entire machine with less redundancy and fewer variables. That is not the purpose of this post, though.

```c
        case POST_DEC:
            if (*ptr == 'e' || *ptr == 'E') {   /* transition to next state */
                state++;
                ptr++;
                if (*ptr == '-') {               /* next state entry action */
                    exp_neg = true;
                    ptr++;
                }
            } else if (IS_DIGIT(*ptr)) {    /* transition within this state */
                res = res * 10.0 + (*ptr - ASCII_ZERO);      /* parse digit */
                prec *= 10.0;                /* increment precision as well */
                ptr++;
            } else if (*ptr == '\0') {     /* end of input emits the result */
                state = EMIT_RES;
            } else {                                       /* invalid input */
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
```
`POST_DEC` and `EXPONENT` follow the same pattern. They try to match `*ptr` with all possible legal inputs, perform the appropriate action and if necessary, make a state transition and fetch the next input.

```c
    if (neg) res *= -1;
    res /= prec;
    if (exp_neg) exp *= -1;
    exp = exp10((double)exp);
    res *= exp;

    return res;
}
```
Finally, we combine all variables into the final result. `res` is negated if necessary and corrected for the precision value in `prec`. Lastly, `res` is multiplied with the exponent and returned.

# Conclusion

This post demonstrates how useful finite-state machines can be to formalize and structure multi-state algorithms like the one we examined. They are a great tool for code analysis, too, since they enforce consistent reasoning about what different states a computer program can be in and what output is produced in every of those states. Obviously, that's not an absolute guarantee of correctness. But it can certainly help to make your code more robust and reliable.

As always, if you find any mistakes or have suggestions for improvements, comment below or on twitter!

# The Full Example:
<script src="https://gist.github.com/DanielSchuette/0b74dfca44741b6451966b7a049f16f0.js"></script>

<hr class="hr-light">

[^1]: They are just strings of digits, really. Individual parts of a float are parsed like we would parse an integer, so keep reading if you're not sure how you would go about doing the latter.
[^2]: Other formats for floating point numbers exist, e.g. `1.0F`.
[^3]: In a state diagram, nodes represent states while edges represent transitions. Actions are sometimes associated with nodes and sometimes with edges; we'll go with the former --- even though it would have been valid to write all actions right next to the inputs that produce them.
