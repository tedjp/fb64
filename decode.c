/*
 * This file is part of fb64.
 *
 * Copyright (c) 2018 Ted J. Percival
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string.h>

#include "fb64.h"

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

__attribute__((constructor))
static void setup_tables() {
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
}

// The number of bytes expected in the last block, based on the *unpadded*
// input length. encoded_len may already be modulo 4.
// returns 0 for inputs of invalid length (ie. length % 4 == 1).
__attribute__((const))
static size_t last_block_decoded_len(size_t encoded_len) {
    switch (encoded_len % 4) {
    case 0:
        return 3;
    case 3:
        return 2;
    case 2:
        return 1;
    case 1: // invalid
    default: // for compilers ;)
        return 0;
    }
}

// determine length for input without padding
// NOTE: This func is const
size_t fb64_decoded_size_nopad(size_t inlen) {
    // FIXME: Handle integer overflow
    return inlen * 3 / 4;
}

// determine length for input with padding
// NOTE: This func is pure
size_t fb64_decoded_size(const char* input, size_t inlen) {
    unsigned pad = 0;
    if (inlen >= 1 && input[inlen - 1] == '=') {
        if (inlen >= 2 && input[inlen - 2] == '=') {
            pad = 2;
        } else {
            pad = 1;
        }
    }

    return fb64_decoded_size_nopad(inlen - pad);
}

static int decode_block(const unsigned char in[4], uint8_t out[3]) {
    out[0] =  t0[in[0]]         | (t1[in[1]] & 3);
    out[1] = (t1[in[1]] & 0xf0) | (t2[in[2]] & 0x0f);
    out[2] = (t2[in[2]] & 192)  |  t3[in[3]];

    return (t0[in[0]] & T0BB) |
           (t1[in[1]] & T1BB) |
           (t2[in[2]] & T2BB) |
           (t3[in[3]] & T3BB);
}

// Returns nonzero on invalid input.
// output buffer *must* have enough space.
// Use fb64_decode_size() or fb64_decode_size_nopad() to determine
// the output buffer size based on the input length.
int fb64_decode(const char *in, size_t len, uint8_t *out) {
    int bad = 0;

    // if your input is always unpadded you can avoid the copy-decode-copy cycle
    // after the loop 25% of the time by changing the condition to `len > 3`,
    // but for possibly-padded input the last block has to do the
    // copy-decode-copy operation to avoid overrunning the output buffer if
    // there's padding.

    while (len > 4) {
        bad |= decode_block((const unsigned char*)in, out);
        // XXX: Avoid using len here and just use `in`?
        len -= 4;
        in += 4;
        out += 3;
    }

    // Final block (which might be a full block, or might include padding)
    // When padded we determine the actual output length (by counting
    // padding symbols) and replace padding with 'A's to avoid decode failure.
    // Unpadded input is unmodified.

    unsigned char block_in[4] = {'A', 'A', 'A', 'A'};
    uint8_t block_out[3];

    memcpy(block_in, in, len);

    // strip padding before final block decode
    // if your input is never padded then you can delete these operations,
    // other than the check for truncated input (len == 1)
    // and go straight to the decode_block call.
    if (len == 4 && in[3] == '=') {
        --len;
        block_in[3] = 'A';
    }

    if (len == 3 && in[2] == '=') {
        --len;
        block_in[2] = 'A';
    }

    if (__builtin_expect(len == 1 || (len == 2 && block_in[1] == '='), 0)) {
        // short input; won't trigger a badbit
        return 1;
    }

    bad |= decode_block(block_in, block_out);
    memcpy(out, block_out, last_block_decoded_len(len));

    return bad;
}
