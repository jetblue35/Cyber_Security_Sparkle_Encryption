#ifndef _ESCHCONFIG_H_
#define _ESCHCONFIG_H_



#define ESCH256



// Each message has a byte-length of 16 bytes.
#define MSGBLOCK_BLEN 16
// Each message has a word-length of 4 words.
#define MSGBLOCK_WLEN (MSGBLOCK_BLEN/4)
// Each squeeze has a byte-length of 16 bytes.
#define SQZBLOCK_BLEN 16
// Each squeeze has a word-length of 4 words.
#define SQZBLOCK_WLEN (SQZBLOCK_BLEN/4)

#if defined ESCH256
// ESCH256 uses SPARKLE384
#define NUM_BRANCHES 6
// The num of slim steps of ESCH256 is 7
#define STEPS_SLIM 7
// The num of big steps of ESCH256 is 11
#define STEPS_BIG 11

#elif defined ESCH384
// ESCH384 uses SPARKLE512
#define NUM_BRANCHES 8
// The num of slim steps of ESCH384 is 8
#define STEPS_SLIM 8
// The num of big steps of ESCH384 is 12
#define STEPS_BIG 12

#else
#error "Invalid definition"
#endif

#endif  // _ESCHCONFIG_H_