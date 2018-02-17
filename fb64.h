#ifndef FB64_H
#define FB64_H 1

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize lookup tables for encode & decode
// Call once at process startup, otherwise your encodes & decodes will be
// all-zeroes.
void fb64_init();

// determine length for Base64 input with padding (possible trailing '=')
__attribute__((__pure__))
size_t fb64_decoded_size(const char* input, size_t inlen);

// determine length for Base64 input without padding (no possible trailing '=')
__attribute__((__const__))
size_t fb64_decoded_size_nopad(size_t inlen);

// Decode base64 string
// Returns nonzero on invalid input.
// output buffer *must* have enough space. Use fb64_docoded_size() to determine
// how much output buffer to reserve beforehand.
// Input must not contain newlines. Newlines are considered an error.
// Output must not overlap with input (ie. cannot decode in-place).
int fb64_decode(const char *in, size_t len, uint8_t *out);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
