/*
 * This file is part of fb64.
 *
 * Copyright (c) 2019 Ted J. Percival
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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "fb64.h"

static const struct {
    // Decoded value has a spurious NUL-terminator for convenience
    const char *encoded, *decoded;
    bool error:1;
} decode_tests[] = {
    // foobar test vectors from
    // https://tools.ietf.org/html/rfc4648#section-10
    { "", "" },
    { "Zg==", "f" },
    { "Zm8=", "fo" },
    { "Zm9v", "foo" },
    { "Zm9vYg==", "foob" },
    { "Zm9vYmE=", "fooba" },
    { "Zm9vYmFy", "foobar" },
    { "#!/bin/bash", "", true },
    { "AAAA", "\x00\x00\x00" },
    { "////", "\xff\xff\xff" },
    { "++++", "\xfb\xef\xbe" },
    { "----", "\xfb\xef\xbe" },
    { "____", "\xff\xff\xff" },
    { "SGVsbG8sIHdvcmxkIQ==", "Hello, world!" },
    { "A=A=", "", true }, // decode error
    { "A", "", true  }, // decode error
    // Unpadded inputs
    { "Zg", "f" },
    { "Zm8", "fo" },
    { "Zm9v", "foo" },
    { "Zm9vYg", "foob" },
    { "Zm9vYmE", "fooba" },
    { "Zm9vYmFy", "foobar" },
};

static const struct {
    const char *input;
    size_t input_len;
    const char *output;
    bool pad:1;
    bool base64url:1;
} encode_tests[] = {
    { "", 0, "", true, false },
    { "\x00", 1, "AA==", true, false },
    { "\x00", 1, "AA", false, true },
    { "ab", 2, "YWI", false, true },
    { "foobar", 6, "Zm9vYmFy", true, false },
    { "\xff\xff\xff", 3, "////", true, false },
    { "\xff\xff\xfe", 3, "///+", true, false },
    { "\xff\xff\xff", 3, "____", true, true },
    { "\xff\xff\xfe", 3, "___-", true, true },
};

int main(void) {
    uint8_t buf[123];

    bool ok = true;

    for (size_t i = 0; i < sizeof(decode_tests) / sizeof(decode_tests[0]); ++i) {
        // dirty the output buffer so we have a good chance of detecting whether
        // the output length (buflen) is too long (except if the input was
        // all-ones).
        memset(buf, '\xff', sizeof(buf));
        size_t outlen = fb64_decoded_size(decode_tests[i].encoded, strlen(decode_tests[i].encoded));
        int err = fb64_decode(decode_tests[i].encoded, strlen(decode_tests[i].encoded), (uint8_t*)buf);
        if (err && !decode_tests[i].error) {
            ok = false;
            fprintf(stderr, "Test input %s failed to decode correctly\n", decode_tests[i].encoded);
            continue;
        } else if (!err && decode_tests[i].error) {
            ok = false;
            fprintf(stderr, "Expeted test %s to fail but it succeeded\n", decode_tests[i].encoded);
            continue;
        }

        if (decode_tests[i].error)
            continue; // skip tests on invalid encoded input: nothing to check agaist

        if (memcmp(buf, decode_tests[i].decoded, outlen) != 0) {
            ok = false;
            fprintf(stderr, "Decode mismatch on input %s, got length %zu: [%.*s]\n", decode_tests[i].encoded, outlen, (int)outlen, buf);
            continue;
        }
    }

    for (size_t i = 0; i < sizeof(encode_tests) / sizeof(encode_tests[0]); ++i) {
        char encoded[123];
        memset(encoded, '\xff', sizeof(encoded));
        // convenience typecast
        const uint8_t* input = (const uint8_t*)encode_tests[i].input;

        size_t expect_size;

        if (encode_tests[i].pad) {
            expect_size = fb64_encoded_size(encode_tests[i].input_len);

            if (encode_tests[i].base64url) {
                fb64_encode_base64url(input, encode_tests[i].input_len, encoded);
            } else {
                fb64_encode(input, encode_tests[i].input_len, encoded);
            }
        } else {
            expect_size = fb64_encoded_size_nopad(encode_tests[i].input_len);

            if (encode_tests[i].base64url) {
                fb64_encode_base64url_nopad(input, encode_tests[i].input_len, encoded);
            } else {
                fb64_encode_nopad(input, encode_tests[i].input_len, encoded);
            }
        }

        if (expect_size != strlen(encode_tests[i].output)) {
            ok = false;
            fprintf(stderr, "Output length from test %zu was %zu, expected %zu\n",
                    i + 1, expect_size, strlen(encode_tests[i].output));
            continue;
        }

        if (memcmp(encode_tests[i].output, encoded, expect_size) != 0) {
            ok = false;
            fprintf(stderr, "Encoded format of input %zu was %.*s; expected %s\n", i + 1, (int)expect_size, encoded, encode_tests[i].output);
            continue;
        }
    }

    return ok ? 0 : 1;
}
