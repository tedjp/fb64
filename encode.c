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
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "fb64.h"

static const char b64[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'};
static const char b64url[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '-', '_'};

static void enc_block(const char table[64], const uint8_t bytes[3], char dest[4]) {
    dest[0] = table[bytes[0] >> 2];
    dest[1] = table[((bytes[0] & 3) << 4) | (bytes[1] >> 4)];
    dest[2] = table[((bytes[1] & 15) << 2) | ((bytes[2] & 192) >> 6)];
    dest[3] = table[(bytes[2] & 63)];
}

static void encode(const uint8_t *buf, size_t len, char *out, const char table[64], bool pad) {
    while (len >= 3) {
        enc_block(table, buf, out);
        buf += 3;
        out += 4;
        len -= 3;
    }

    if (len) {
        uint8_t local[3] = {0,0,0};
        char local_out[4];
        memcpy(local, buf, len);

        enc_block(table, local, local_out);

        switch (len) {
        case 2:
            out[2] = local_out[2];
            /* fallthrough */
        case 1:
            out[1] = local_out[1];
            out[0] = local_out[0];
        }

        if (pad) {
            switch (len) {
            case 1:
                out[2] = '=';
                /* fallthrough */
            case 2:
                out[3] = '=';
            }
        }
    }
}

void fb64_encode(const uint8_t *buf, size_t len, char *out) {
    encode(buf, len, out, b64, true);
}

void fb64_encode_nopad(const uint8_t *buf, size_t len, char *out) {
    encode(buf, len, out, b64, false);
}

void fb64_encode_base64url(const uint8_t *buf, size_t len, char *out) {
    encode(buf, len, out, b64url, true);
}

void fb64_encode_base64url_nopad(const uint8_t *buf, size_t len, char *out) {
    encode(buf, len, out, b64url, false);
}

// NOTE: This function is const
size_t fb64_encoded_size(size_t input_len) {
    while (input_len % 3 != 0)
        ++input_len;

    return input_len * 4 / 3;
}

// NOTE: This function is const
size_t fb64_encoded_size_nopad(size_t input_len) {
    size_t encoded_size = fb64_encoded_size(input_len);

    switch (input_len % 3) {
    case 1:
        return encoded_size - 2;
    case 2:
        return encoded_size - 1;
    default:
        return encoded_size;
    }
}
