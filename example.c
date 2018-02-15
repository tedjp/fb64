#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fb64d.h"

int main(int argc, char *argv[]) {
    // Initialize decoder tables
    fb64d_init();

    // Use command-line arg as input, or premade string.
    const char *input = argc > 1 ? argv[1] : "Zm9vYmFy";

    size_t input_length = strlen(input);

    // Determine output length to allocate buffer
    size_t output_length = fb64d_buflen(input, input_length);

    // Dynamically allocate output buffer
    uint8_t *output = malloc(output_length);
    if (!output)
        return 1;

    // Decode.
    int err = fb64d_decode(input, input_length, output);
    if (err)
        return 2;

    // Write decoded data to stdout.
    // Output is binary, so for many inputs it will appear
    // to be garbage. Consider piping output to `hexdump`.
    if (write(STDOUT_FILENO, output, output_length) < 0) {
        perror("write failed");
        return 3;
    }

    // Cleanup.
    free(output);

    return 0;
}
