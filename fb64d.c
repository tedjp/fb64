#include <string.h>

#include "fb64d.h"

// Future: These tables can be hard-coded
// rather than built at startup.

// T0: 6 bits, unused bit, badbit
//     6 bits are MSB of the first octet
// T1: 4 bits, unused bit, badbit, 2 bits
//     2 bits in LSB position are combined with T0 for first octet
//     4 bits in MSB position are the 4 MSB of second octet
// T2: 2 bits, unused bit, badbit, 4 bits
//     2 bits are the MSB of the third octet
//     4 bits are the LSB of the second octet
// T3: unused bit, badbit, 6 bits
//     6 bits are the LSB of the third octet
// IOW T1 & T2 contain bits in the "wrong" order
// because they can be straight masked & ORed rather than
// having to be shifted.
static uint8_t t0[256], t1[256], t2[256], t3[256];

// Bad bits
#define T0BB (1 << 0)
#define T1BB (1 << 2)
#define T2BB (1 << 4)
#define T3BB (1 << 6)

static void fill_badbits() {
    memset(t0, T0BB, 256);
    memset(t1, T1BB, 256);
    memset(t2, T2BB, 256);
    memset(t3, T3BB, 256);
}

__attribute__((const))
static unsigned char next(unsigned char c) {
    switch (c) {
    case 'Z':
        return 'a';
    case 'z':
        return '0';
    case '9':
        return '+';
    case '+':
        return '/';
    default:
        return ++c;
    }
}

__attribute__((const))
static uint8_t splitshift_t1(uint8_t n) {
    return n >> 4 | n << 4;
}

__attribute__((const))
static uint8_t splitshift_t2(uint8_t n) {
    return n >> 2 | n << 6;
}

void fb64d_init() {
    fill_badbits();

    uint8_t n = 0;
    unsigned char c = 'A';
    for (n = 0; n < 64; ++n) {
        t0[c] = n << 2;
        t1[c] = splitshift_t1(n);
        t2[c] = splitshift_t2(n);
        t3[c] = n;
        c = next(c);
    }

    // token68 alternate characters
    // ~ & . are documented as (rare) alternatives and part of the
    // ABNF for HTTP/2's HTTP2-Settings header, but they're actually
    // not allowed according to RFC 4648 (base64url spec) and there's
    // no clear documentation of which code represents which value.
    // So we only include base64 (above) & base64url codes.
    t0['-'] = 62 << 2;
    t0['_'] = 63 << 2;
    t1['-'] = splitshift_t1(62);
    t1['_'] = splitshift_t1(63);
    t2['-'] = splitshift_t2(62);
    t2['_'] = splitshift_t2(63);
    t3['-'] = 62;
    t3['_'] = 63;

    // Clear badbit for padding ('=')
    t0['='] = 0;
    t1['='] = 0;
    t2['='] = 0;
    t3['='] = 0;
}

// determine length for input without padding
// NOTE: This func is const
size_t fb64d_nopad_buflen(size_t inlen) {
    // FIXME: Handle integer overflow
    return inlen * 3 / 4;
}

// determine length for Base64 input with padding
// NOTE: This func is pure
size_t fb64d_buflen(const uint8_t* input, size_t inlen) {
    if (inlen < 4)
        return inlen;

    unsigned pad = 0;
    if (input[inlen - 1] == '=') {
        if (input[inlen - 2] == '=') {
            pad = 2;
        } else {
            pad = 1;
        }
    }

    return fb64d_nopad_buflen(inlen - pad);
}

static int decode_block(const uint8_t in[4], uint8_t out[3]) {
    out[0] =  t0[in[0]]         | (t1[in[1]] & 3);
    out[1] = (t1[in[1]] & 0xf0) | (t2[in[2]] & 0x0f);
    out[2] = (t2[in[2]] & 192)  | (t3[in[3]] & ~192);

    return (t0[in[0]] & T0BB) |
           (t1[in[1]] & T1BB) |
           (t2[in[2]] & T2BB) |
           (t3[in[3]] & T3BB);
}

// Returns nonzero on invalid input.
// output buffer *must* have enough space.
// Use b64d_buflen() or b64d_nopad_buflen() to determine
// the output buffer size based on the input length.
int fb64d_decode(const uint8_t *in, size_t len, uint8_t *out) {
    int bad = 0;

    while (len > 3) {
        bad |= decode_block(in, out);
        // XXX: Avoid using len here and just use `in`?
        len -= 4;
        in += 4;
        out += 3;
    }

    if (len > 0) {
        uint8_t block[4] = {'='};
        memcpy(block, in, len);

        bad |= decode_block(block, out);
    }

    return bad;
}
