#ifndef GAREPL_H
#define GAREPL_H

/* The REPL mode is intended for users who might know assembly,
 * but aren't yet fluent in the particular assembly language needed
 * for their projects.  Keepiing this running in a spare terminal
 * allows instructions to be attempted with little penalty for failure.
 *
 * For Windows portability, Readline might be a liability.  If you
 * need to replace this, please patch garepl.cpp/h and the relevant
 * functions of goodasm.cpp/h.
 *
 * Potential alternatives:
 * https://github.com/AmokHuginnsson/replxx
 */

class GoodASM;

int garepl_encode(GoodASM *goodasm);

#endif // GAREPL_H
