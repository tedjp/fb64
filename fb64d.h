#pragma once

#include <stddef.h>
#include <stdint.h>

// fb64d: Fast Base64 decoder
// Currently needs setup_tables() to be called once at start of execution
// to build the lookup tables.
// In the future these will be hard-coded.
void setup_tables();

// determine length for base64 input without padding (no possible trailing '=')
__attribute__((__const__))
size_t b64d_nopad_buflen(size_t inlen);

// determine length for Base64 input with padding (possible trailing '=')
__attribute__((__pure__))
size_t b64d_buflen(const uint8_t* input, size_t inlen);

// Returns nonzero on invalid input.
// output buffer *must* have enough space
int decode(const uint8_t *in, size_t len, uint8_t *out);
