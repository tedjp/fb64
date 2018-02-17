# fb64

Fast Base64 Encoder & Decoder

## Example

```c
#include "fb64.h"

fb64_init(); // once

const char input[] = "SGVsbG8sIHdvcmxkIQ==";

// Use fb64_decoded_size() to determine output size; hard-coded here for simplicity.
uint8_t output[14] = {0};

fb64_decode(input, strlen(input), output);

printf("%s\n", output);
```

See [example.c](example.c) for a fuller example that handles inputs of various lengths.

## Benchmarks

A preliminary benchmark comparison with OpenSSL's Base64 decoder,
invoked via [Proxygen](https://github.com/facebook/proxygen), is included on
the [bench-proxygen branch](https://github.com/tedjp/fb64/commits/bench-proxygen).

The first two use fb64 (String version wraps the input & output
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

These results show that **fb64 decode is more than twice as fast**.

## Advanced usage

fb64 can be used to encode or decode streams or large amounts of data by calling
the encode/decode functions on blocks of input aligned on encode/decode quantum
boundaries. Encode input should be split on 3-octet boundaries; decode input
should be split on 4-character boundaries.

## Limitations

1. `fb64_decode()` does not accept newlines in its input. It might still be faster
   than other decoders if the input is preprocessed to delete newlines.

2. The function `fb64_init()` must be called once to initialize the lookup
   tables. It only has to be executed once at program startup. In future the
   lookup tables could be hard-coded.

3. Both base64 and base64url symbols are accepted equally. ie. input may contain
   a mix of +, /, - and _ as the 62nd & 63rd symbols which will not trigger a
   decode error. If you need a strict decoder that will only accept one set of
   symbols you might like to modify the lookup tables to delete the symbols
   that you don't want, or make a second pass over the input to validate that
   it conforms to your requirements. See [RFC 4648 section 12 "Security
   Considerations"](https://tools.ietf.org/html/rfc4648#section-12).

## Bugs

1. The buffer length functions `fb64_decoded_size()` &
   `fb64_decoded_size_nopad()` return incorrect buffer lengths for inputs longer
   than ⅓ of `SIZE_MAX`. That's about 6 exabytes (6×10¹⁸) on a 64-bit machine.

## License

[MIT License](LICENSE)
