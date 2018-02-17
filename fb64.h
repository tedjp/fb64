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

// Encoding:
// These functions *do not* output a trailing NUL-byte. Neither the encoding
// functions nor the fb64_encoded_size*() functions include space for
// a terminator.
// This makes them suitable for writing directly to network buffers,
// but means that it's the caller's responsibility to NUL-terminate if they
// intend to use the output as a C-string.

// Size of output buffer needed to encode input with possible padding
__attribute__((__const__))
size_t fb64_encoded_size(size_t input_len);

// Size of output buffer needed to encode input without padding.
__attribute__((__const__))
size_t fb64_encoded_size_nopad(size_t input_len);

// Encode bytes to Base64
// Using the standard base64 alphabet (/ & +) with padding.
void fb64_encode(const uint8_t *buf, size_t len, char *out);

// Encode bytes to Base64
// Using the standard base64 alphabet (/ & +) without padding.
void fb64_encode_nopad(const uint8_t *buf, size_t len, char *out);

// Encode bytes to Base64
// Using the base64url alphabet (- & _) with padding.
void fb64_encode_base64url(const uint8_t *buf, size_t len, char *out);

// Encode bytes to Base64
// Using the base64url alphabet (- & _) without padding.
void fb64_encode_base64url_nopad(const uint8_t *buf, size_t len, char *out);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
