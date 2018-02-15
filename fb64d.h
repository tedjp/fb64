#ifndef FB64D_H
#define FB64D_H 1

#include <stddef.h>
#include <stdint.h>

// fb64d: Fast Base64 decoder
// Currently needs fb64d_init() to be called once at start of execution
// to build the lookup tables.
// In the future they will be hard-coded.

// Initialize lookup tables
// Call once at process startup, otherwise your decodes will be all-zeroes.
void fb64d_init();

// determine length for Base64 input with padding (possible trailing '=')
__attribute__((__pure__))
size_t fb64d_buflen(const uint8_t* input, size_t inlen);

// determine length for Base64 input without padding (no possible trailing '=')
__attribute__((__const__))
size_t fb64d_nopad_buflen(size_t inlen);

// Decode base64 string
// Returns nonzero on invalid input.
// output buffer *must* have enough space. Use fb64d_buflen() to determine
// how much output buffer to reserve beforehand.
// Input must not contain newlines. Newlines are considered an error.
int fb64d_decode(const uint8_t *in, size_t len, uint8_t *out);

#endif
