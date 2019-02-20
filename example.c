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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fb64.h"

int main(int argc, char *argv[]) {
    // Use command-line arg as input, or premade string.
    const char *input = argc > 1 ? argv[1] : "Zm9vYmFy";

    size_t input_length = strlen(input);

    // Determine output length to allocate buffer
    size_t output_length = fb64_decoded_size(input, input_length);

    // Dynamically allocate output buffer
    uint8_t *output = malloc(output_length);
    if (!output)
        return 1;

    // Decode.
    int err = fb64_decode(input, input_length, output);
    if (err) {
        fprintf(stderr, "Decode error\n");
        return 2;
    }

    // Write decoded data to stdout.
    // Output is binary, so for many inputs it will appear
    // to be garbage. Consider piping output to `hd` (hexdump).
    if (write(STDOUT_FILENO, output, output_length) < 0) {
        perror("write failed");
        return 3;
    }

    // Cleanup.
    free(output);

    return 0;
}
