#define _BYTES_ 32

#include <string.h>
#include <stdio.h>

#include "esch.h"
#include "header.h"



#define ROT(x, n) (((x) >> (n)) | ((x) << (32-(n))))
#define ELL(x) (ROT(((x) ^ ((x) << 16)), 16))

typedef unsigned char uchar_t;
typedef unsigned long long int ullint_t;
int crypto_hash(uchar_t *out, const uchar_t *in, ullint_t inlen);
void tobyte(unsigned char x[], char *hexstring) {

  int i;

    for (i = 0; i < (strlen(hexstring) / 2); i++) {
     
        sscanf(hexstring+2*i, "%02hhX", &x[i]);
    }
}
int main (int argc, char *argv[]) {

    unsigned char msg[_BYTES_];
    unsigned char		digest[_BYTES_];
    int mlen=8;
    int i;

    
    char my_msg[_BYTES_]="1801042100";
    if( argc > 1 ) {
        strcpy(my_msg,argv[1]);

    }
    puts("Saturnin");
    printf("Message (hex):\t %s",my_msg);
    printf("\nHash:\t");

    tobyte(msg,my_msg);
    
    int len = strlen(my_msg)/2;
    int ret = crypto_hash(digest, msg, len);

    for (i = 0; i < _BYTES_; i++)
    {
        if (i > 0) printf(":");
        printf("%02X", digest[i]);
    }
    printf("\n");    
    return ret;
}   


// The function injectm_ref adds a 16-byte block of the message to the two
// leftmost branches of the state (i.e. to the state-words x0, y0, x1, and y1),
// whereby the block is first ransformed via a linear Feistel function.

void injectm_ref(state_t *state, const uchar_t *msgbytes, int nb)
{
    uint32_t *msgwords = (uint32_t *) msgbytes;
    uint32_t tmpx = 0, tmpy = 0;
    int i, j;

    // Since the message block is 16 bytes long, we need to consider only two
    // x-words when computing tmpx and two y-words when computing tmpy.

    for(i = 0; i < MSGBLOCK_WLEN; i += 2) {
        tmpx ^= msgwords[i];
        tmpy ^= msgwords[i+1];
    }
    tmpx = ELL(tmpx);
    tmpy = ELL(tmpy);

    // The two leftmost x-words of the state are updated by adding the two
    // x-words of the message and tmpy to them, and the same is done with the two
    // leftmost y-words. The remaining nb/2-2 x-words are updated by just adding
    // tmpy to them, and the same is done with the remaining nb/2-2 y-words.

    for (i = j = 0; i < MSGBLOCK_WLEN/2; i++) {
        state->x[i] ^= (msgwords[j++] ^ tmpy);
        state->y[i] ^= (msgwords[j++] ^ tmpx);
    }
    for (i = MSGBLOCK_WLEN/2; i < nb/2; i++) {
        state->x[i] ^= tmpy;
        state->y[i] ^= tmpx;
    }
}


// The function trunc_state extracts the four 32-bit words x0, y0, x1, and y1
// from the state and copies these 16 bytes to the array <out>.

void trunc_state(uchar_t *out, const state_t *state)
{
    uint32_t *out_words = (uint32_t *) out;
    int i, j;

    for (i = j = 0; j < SQZBLOCK_WLEN; i++) {
        out_words[j++] = state->x[i];
        out_words[j++] = state->y[i];
    }
}


// To ensure compatibility with the SUPERCOP, the below implementation of
// crypto_hash can handle overlapping input and output buffers.

int crypto_hash(uchar_t *out, const uchar_t *in, ullint_t inlen)
{
    state_t state = { { 0 }, { 0 } };   // State with x, y array initialized to 0
    uchar_t lastblk[MSGBLOCK_BLEN] = { 0 };   // Buffer for last block of message
    // The type size_t is large enough to contain the size in bytes of any object
    size_t in_blen = (size_t) inlen, hashed_bytes = 0, lastblk_blen;

    // A message exceeding 16 bytes is absorbed in 16-byte blocks. Note that the
    // loop below is not iterated at all when inlen <= 16 bytes.

    while((in_blen - hashed_bytes) > MSGBLOCK_BLEN) {
        // Add 16 bytes of the message to the state
        injectm_ref(&state, &(in[hashed_bytes]), NUM_BRANCHES);
        // Execute SPARKLE with a slim number of steps
        sparkle_ref(&state, NUM_BRANCHES, STEPS_SLIM);
        hashed_bytes += MSGBLOCK_BLEN;
    }

    // The last block can be between 0 and 16 bytes long (it can only be 0 when
    // inlen is 0). It is padded only when its length is shorter than 16 bytes.

    lastblk_blen = in_blen - hashed_bytes;
    memcpy(lastblk, &(in[hashed_bytes]), lastblk_blen);
    if (lastblk_blen < MSGBLOCK_BLEN) {
        lastblk[lastblk_blen] = 0x80;
    }
    // Absorb the (padded) last message block
    injectm_ref(&state, lastblk, NUM_BRANCHES);
    // Const_M is added to y2, which is state.y[2]
    if (lastblk_blen < MSGBLOCK_BLEN) {
        state.y[(NUM_BRANCHES/2)-1] ^= 0x01000000;
    } else {
        state.y[(NUM_BRANCHES/2)-1] ^= 0x02000000;
    }
    // Execute SPARKLE with a big number of steps
    sparkle_ref(&state, NUM_BRANCHES, STEPS_BIG);

    // Squeeze to produce the message digest

    trunc_state(out, &state);
    sparkle_ref(&state, NUM_BRANCHES, STEPS_SLIM);
    trunc_state(&(out[SQZBLOCK_BLEN]), &state);

    return 0;
}
