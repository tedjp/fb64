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

#ifndef FB64_H
#define FB64_H 1

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Largest inputs that the _size() functions will return accurate values for.
// These are >1 Exabyte on 64-bit systems so you probably have other, smaller
// limits (eg. available system memory; incoming network buffer size) that
// will be hit first. Beyond these values, the _size() functions will silently
// return a wrapped value that will result in a buffer overflow, so don't
// allocate the output buffer based on an untrusted input buffer length.
//
// (If you want to encode/decode larger inputs, split the input on 3-octet
// boundaries for encode or 4-char boundaries for decode and make multiple calls
// to the encode/decode function.)
#define FB64_ENCODE_MAX (SIZE_MAX / 4)
#define FB64_DECODE_MAX (SIZE_MAX / 3)

#define FB64_EXPORT __attribute__((visibility("default")))

// determine length for Base64 input with padding (possible trailing '=')
FB64_EXPORT
__attribute__((__pure__))
size_t fb64_decoded_size(const char* input, size_t inlen);

// determine length for Base64 input without padding (no possible trailing '=')
FB64_EXPORT
__attribute__((__const__))
size_t fb64_decoded_size_nopad(size_t inlen);

// Decode base64 string
// Returns nonzero on invalid input.
// output buffer *must* have enough space. Use fb64_docoded_size() to determine
// how much output buffer to reserve beforehand.
// Input must not contain newlines. Newlines are considered an error.
// Output must not overlap with input (ie. cannot decode in-place).
FB64_EXPORT
int fb64_decode(const char *in, size_t len, uint8_t *out);

// Encoding:
// These functions *do not* output a trailing NUL-byte. Neither the encoding
// functions nor the fb64_encoded_size*() functions include space for
// a terminator.
// This makes them suitable for writing directly to network buffers,
// but means that it's the caller's responsibility to NUL-terminate if they
// intend to use the output as a C-string.

// Size of output buffer needed to encode input with possible padding
FB64_EXPORT
__attribute__((__const__))
size_t fb64_encoded_size(size_t input_len);

// Size of output buffer needed to encode input without padding.
FB64_EXPORT
__attribute__((__const__))
size_t fb64_encoded_size_nopad(size_t input_len);

// Encode bytes to Base64
// Using the standard base64 alphabet (+ & /) with padding.
FB64_EXPORT
void fb64_encode(const uint8_t *buf, size_t len, char *out);

// Encode bytes to Base64
// Using the standard base64 alphabet (+ & /) without padding.
FB64_EXPORT
void fb64_encode_nopad(const uint8_t *buf, size_t len, char *out);

// Encode bytes to Base64
// Using the base64url alphabet (- & _) with padding.
FB64_EXPORT
void fb64_encode_base64url(const uint8_t *buf, size_t len, char *out);

// Encode bytes to Base64
// Using the base64url alphabet (- & _) without padding.
FB64_EXPORT
void fb64_encode_base64url_nopad(const uint8_t *buf, size_t len, char *out);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
