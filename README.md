# fb64d

Fast Base64 Decoder

## Example

```c
#include "fb64d.h"

fb64d_init(); // once

const char input[] = "SGVsbG8sIHdvcmxkIQ==";

// Use fb64d_buflen() to determine output size; hard-coded here for simplicity.
uint8_t output[14] = {0};

fb64d_decode(input, strlen(input), output);

printf("%s\n", output);
```

See [example.c](example.c) for a fuller example that handles inputs of various lengths.

## Benchmarks

A preliminary benchmark comparison with OpenSSL's Base64 decoder,
invoked via [Proxygen](https://github.com/facebook/proxygen), is included on
the [bench-proxygen branch](https://github.com/tedjp/fb64d/commits/bench-proxygen).

The first two use fb64d (String version wraps the input & output
in `std::string`s). BM\_ProxygenOpenSSLDecode is OpenSSL's BIO base64 decoder wrapped
in `std::string`s. The benchmark decodes 1 kiB of random data.

```
----------------------------------------------------------------
Benchmark                         Time           CPU Iterations
----------------------------------------------------------------
BM_Decode                      1277 ns       1275 ns     547301
BM_Decode_String               1288 ns       1287 ns     528328
BM_ProxygenOpenSSLDecode       2917 ns       2914 ns     237881
```

These results show that **fb64d is more than twice as fast**.

## Limitations

1. fb64d does not accept newlines in its input. It might still be faster than other
   decoders if the input is preprocessed to delete newlines.

2. The function `fb64d_init()` must be called once to initialize the lookup
   tables. It only has to be executed once at program startup. In future a
   prebuilt table could be used instead.

3. Both base64 and base64url symbols are accepted equally. ie. input may contain
   a mix of +, /, - and _ as the 62nd & 63rd symbols which will not trigger a
   decode error. If you need a strict decoder that will only accept one set of
   symbols you might like to modify the lookup tables to delete the symbols
   that you don't want, or make a second pass over the input to validate that
   it conforms to your requirements. See [RFC 4648 section 12 "Security
   Considerations"](https://tools.ietf.org/html/rfc4648#section-12).

## Bugs

1. The buffer length functions `fb64d_buflen` & `fb64d_nopad_buflen` return incorrect
   buffer lengths for inputs longer than ⅓ of `SIZE_MAX`. That's about 6 exabytes
   (6×10¹⁸) on a 64-bit machine.

## License

[MIT License](LICENSE)
