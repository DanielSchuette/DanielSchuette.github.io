---
layout: post
title:  "Float parsing using a finite-state machine"
date:   2020-04-18 08:55:00 +0200
categories: parsing
---

In this post, we'll take a look at parsing a floating point number from its string representation. Whereas it's fairly easy to parse an integer from a string[^1], most programming languages support multiple ways of expressing floats. Thus, parsing them is a little more difficult. Take a look at the following array of strings:

```c
char *tests[] = { "11.3", "119.312", "114.3", ".3", "114.",
                  "-11.3", "-119.312", "-114.3", "-.3", "-114.",
                  "11.3e2", "119.312e-2", "114.3e12", ".3e-3", "114.e1",
                  "-11.3e2", "-119.312e-2", "-114.3e12", "-.3e-3",
                  "-114.e1" };
```

They all represent valid floating point numbers. It's obvious that we have to organize our parser well to ensure that we correctly recognize of all the different formats above. Finite-state machines or FSMs for short, are a well-suited tool for this task, so well use them to implement a simple, but robust and extensible[^2] float parser.

# Finite-state machines

Finite-state machines are a [mathematical model of computation](https://en.wikipedia.org/wiki/Finite-state_machine), in which an abstract machine moves through a certain number of distinct states. These states are associated with certain actions. Transitions between states as well as actions are triggered by inputs to the machine. It's not very important to go deep into theory to apply the concept of a state machine, so we'll just demonstrate of what practical use FSMs can be.

# State diagram

As we said above, an FSM can only be in a certain number of states. Thus, the first thing we do is to define our states:

```c
typedef enum {
    PRE_DEC,    /* currently parsing before decimal point */
    POST_DEC,   /* currently parsing after decimal point*/
    EXPONENT,   /* currently parsing exponent */
} state;        /* possible states of our FSM */
```

We'll feed one character of the input string after another into our machine, moving from `PRE_DEC` to `POST_DEC` to `EXPONENT`. The state diagram below depicts, under which conditions transitions occur. When we encounter a `'.'`, we are moving from the `PRE_DEC` to the `POST_DEC` state (`DEC` means decimal, if you haven't guessed already). When we're in the `POST_DEC` state and find an `'e'` or `'E'`, we transition to `EXPONENT`.

![State diagram](../../../../assets/fsm_diagram_float.png)

The diagram also shows which error conditions might occur. First, parsing of an invalid character means failure. If we reach the end of the input and are still in `PRE_DEC`, that's also an error since floats **must** have a decimal point (at least in our implementation). As soon as we're in `POST_DEC`, an end of input means success, though.

To actually produce output with our machine, we associate state transitions with *entry actions*. In our case, we check for a `'-'` on every transition. It must be stressed that we only consider the transition from one state to another as a valid trigger for an entry action. Such actions are labeled with *E* in the graph. The actions we perform when we stay in a certain state are marked with *R* (for *repeated* action). They occur whenever we encounter a character in the inclusive range `'0'` to `'9'`. In the next section, we will take a look at the code to understand this somewhat abstract description even better and to see what these mysterious actions actually are.

# Implementation

# Conclusion

This post demonstrates how useful finite-state machines can be to formalize and structure multi-state algorithms like the one we examined. They are a great tool for code analysis, too, since they enforce consistent reasoning about what different states a computer program can be in and what output is produced in every of those states. Obviously, that's not an absolute guarantee of correctness. But it can certainly help to make your code more robust and reliable.

As always, if you find any mistakes or have suggestions for improvements, comment below or on twitter!

<hr class="hr-light">

[^1]: They are just strings of digits, really. Individual parts of a float are parsed like we would parse an integer, so keep reading if you're not sure how you would go about doing the latter.
[^2]: Other formats for floating point numbers exist, e.g. `1.0F`.
