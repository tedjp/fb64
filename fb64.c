#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fb64.h"

static void usage(const char *argv0, FILE *dest) {
    fprintf(dest, "Usage: %s [-d]\n", argv0);
}

static int decode() {
    char input[4096];
    uint8_t decoded[4096 * 3 / 4]; // 3072

    ssize_t len;

    while ((len = read(STDIN_FILENO, input, sizeof(input))) > 0) {
        if (input[len - 1] == '\n')
            --len;
        if (input[len - 1] == '\r')
            --len;

        size_t decoded_len = fb64_decoded_size(input, (size_t) len);
        assert(decoded_len <= sizeof(decoded));

        int err = fb64_decode(input, (size_t) len, decoded);
        if (err) {
            fprintf(stderr, "Decode error, ensure input has no whitespace\n");
            return 1;
        }

        len = write(STDOUT_FILENO, decoded, decoded_len);
        if (len == -1) {
            perror("Write error");
            return 1;
        }

        if (len != decoded_len) {
            fprintf(stderr, "Short write\n");
            return 1;
        }
    }

    if (len < 0) {
        perror("Read error");
        return 1;
    }

    return 0;
}

static int encode() {
    uint8_t inbuf[4096];
    char outbuf[fb64_encoded_size(sizeof(inbuf))];
    ssize_t len;
    size_t keep = 0;
    bool wrote = false;

    // Align a multiple of 3 octets at a time so that the output
    // is a multiple of 4 bytes and padding comes naturally at
    // the true end.
    for (;;) {
        len = read(STDIN_FILENO, inbuf + keep, sizeof(inbuf) - keep);

        if (len < 0) {
            perror("Read error");
            return 1;
        }

        if (len + keep == 0) {
            if (wrote) {
                static const char newline = '\n';
                return write(STDOUT_FILENO, &newline, 1) == 1 ? 0 : 1;
            }

            return 0;
        }

        if (len == 0) {
            // Encode last morsel
            len = keep;
            keep = 0;
        } else {
            // Truncate to quantum to avoid bogus padding
            len += keep;
            keep = 0;

            while (len % 3 != 0) {
                --len;
                ++keep;
            }
        }

        // len might be zero
        fb64_encode(inbuf, len, outbuf);

        if (keep > 0 && len > 0)
            memcpy(inbuf, inbuf + len, keep);

        size_t to_write = fb64_encoded_size(len);

        // output
        do {
            ssize_t outlen = write(STDOUT_FILENO, outbuf, to_write);
            if (outlen < 0) {

                perror("Write error");
                return 1;
            }

            if (outlen > 0)
                wrote = true;

            to_write -= (size_t) outlen;

        } while (to_write > 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc >= 2 && strcmp(argv[1], "-d") == 0)
        return decode();

    if (argc == 1)
        return encode();

    usage(argv[0], stderr);
    return 1;
}
