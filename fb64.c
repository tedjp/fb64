#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fb64.h"

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

typedef void(*encodefunc_t)(const uint8_t*, size_t, char*);

static int encode(encodefunc_t encode_func) {
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
        encode_func(inbuf, len, outbuf);

        if (keep > 0 && len > 0)
            memcpy(inbuf, inbuf + len, keep);

        size_t to_write;
        if (encode_func == fb64_encode_nopad ||
                encode_func == fb64_encode_base64url_nopad) {
            to_write = fb64_encoded_size_nopad(len);
        } else {
            to_write = fb64_encoded_size(len);
        }

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

static void usage(const char *argv0, FILE *dest) {
    fprintf(dest, "Usage: %s [options]\n", argv0);
    fprintf(dest, "Options:\n"
            "-h --help   Print this message\n"
            "-d --decode Decode base64 or base64url input\n"
            "-n --no-pad Elide padding when encoding\n"
            "-u --url    Encode to base64url character set\n"
            "            --base64url is an alias for this option\n"
           );
}

int main(int argc, char *argv[]) {
    const struct option options[] = {
        { "decode",    no_argument, NULL, 'd' },
        { "help",      no_argument, NULL, 'h' },
        { "no-pad",    no_argument, NULL, 'n' },
        { "url",       no_argument, NULL, 'u' },
        { "base64url", no_argument, NULL, 'u' }, // alias for --url
    };

    int option;
    bool url = false, nopad = false;

    while ((option = getopt_long(argc, argv, "dhnu", options, NULL)) != -1) {
        switch (option) {
        case 'd':
            return decode();
        case 'h':
            usage(argv[0], stdout);
            return 0;
        case 'n':
            nopad = true;
            break;
        case 'u':
            url = true;
            break;
        case '?':
            usage(argv[0], stderr);
            return 1;
        }
    }

    encodefunc_t encode_func;

    if (url) {
        if (nopad)
            encode_func = fb64_encode_base64url_nopad;
        else
            encode_func = fb64_encode_base64url;
    } else {
        if (nopad)
            encode_func = fb64_encode_nopad;
        else
            encode_func = fb64_encode;
    }

    return encode(encode_func);
}
