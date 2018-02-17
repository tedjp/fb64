#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "fb64.h"

struct {
    const char *encoded, *decoded;
} tests[] = {
    // foobar test vectors from
    // https://tools.ietf.org/html/rfc4648#section-10
    { "", "" },
    { "Zg==", "f" },
    { "Zm8=", "fo" },
    { "Zm9v", "foo" },
    { "Zm9vYg==", "foob" },
    { "Zm9vYmE=", "fooba" },
    { "Zm9vYmFy", "foobar" },
    //{ "#!/bin/bash", "" }, // decode error
    { "AAAA", "\x00\x00\x00" },
    { "////", "\xff\xff\xff" },
    { "++++", "\xfb\xef\xbe" },
    { "----", "\xfb\xef\xbe" },
    { "____", "\xff\xff\xff" },
    //{ "A=A=", "" }, // decode error
    //{ "A", "" }, // decode error
    // Unpadded inputs
    { "Zg", "f" },
    { "Zm8", "fo" },
    { "Zm9v", "foo" },
    { "Zm9vYg", "foob" },
    { "Zm9vYmE", "fooba" },
    { "Zm9vYmFy", "foobar" },
};

int main(void) {
    fb64_init();

    uint8_t buf[123];

    bool ok = true;

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        // dirty the output buffer so we have a good chance of detecting whether
        // the output length (buflen) is too long (except if the input was
        // all-ones).
        memset(buf, '\xff', sizeof(buf));
        size_t outlen = fb64_decoded_size(tests[i].encoded, strlen(tests[i].encoded));
        int err = fb64_decode(tests[i].encoded, strlen(tests[i].encoded), (uint8_t*)buf);
        if (err) {
            ok = false;
            fprintf(stderr, "Test input %s failed to decode correctly\n", tests[i].encoded);
            continue;
        }

        if (memcmp(buf, tests[i].decoded, outlen) != 0) {
            ok = false;
            fprintf(stderr, "Decode mismatch on input %s, got length %zu: [%.*s]\n", tests[i].encoded, outlen, (int)outlen, buf);
            continue;
        }
    }

    return ok ? 0 : 1;
}
