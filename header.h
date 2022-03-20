
#ifndef _SPARKLE_REF_H
#define _SPARKLE_REF_H

#ifdef _MSC_VER
typedef unsigned __int8 uint8_t;
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif

#define MAX_BRANCHES 8

typedef struct {
  uint32_t x[MAX_BRANCHES];
  uint32_t y[MAX_BRANCHES];
} state_t;

void sparkle_ref(state_t *state, int nb, int ns);
void sparkle_inv_ref(state_t *state, int nb, int ns);

void print_state_ref(const state_t *state, int nb);
void test_sparkle_ref(int nb, int ns);

#endif  // _SPARKLE_REF_H