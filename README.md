# fb64

Fast Base64 Encoder & Decoder

Features
- Supports both [base64 & base64url encodings](https://tools.ietf.org/html/rfc4648).
- Supports both padded & unpadded input & output.
- Small memory footprint.
- *Fast.*

## Examples

### Decode

```c
const char input[] = "SGVsbG8sIHdvcmxkIQ==";

fb64_init(); // initialize decode tables (once)

size_t decoded_size = fb64_decoded_size(input, strlen(input));
uint8_t output[decoded_size];

fb64_decode(input, strlen(input), output);

printf("%.*s\n", (int)decoded_size, output);
```

### Encode

```c
const uint8_t input[] = {'f', 'b', '6', '4', ' ', 'i', 's', ' ', 'f', 'a', 's', 't', '!'};

// Use fb64_encoded_size() to determine output length. It does not
// include space for a NUL-terminator.

size_t output_size = fb64_encoded_size(sizeof(input));

char output[output_size + 1]; // include space for NUL-terminator

fb64_encode(input, sizeof(input), output);

// Manually terminate the C-string
output[output_size] = '\0';

printf("%s\n", output);
```

## Benchmarks

fb64 is **twice as fast as OpenSSL & 7 times as fast as Boost** at decoding
1 kiB of random data.

fb64 is **8 times as fast as Boost** at encoding 1 kiB of random data.

```
----------------------------------------------------------------
Benchmark                         Time           CPU Iterations
----------------------------------------------------------------
BM_Decode                      1275 ns       1275 ns     544111
BM_Decode_String               1304 ns       1304 ns     532441
BM_ProxygenOpenSSLDecode       2817 ns       2817 ns     247103
BM_BoostDecode                 8915 ns       8915 ns      76537
fb64_Encode                    1029 ns       1029 ns     689610
BoostEncode                    9054 ns       9054 ns      71812
```

The "String" variant wraps the input & output in a `std::string`.

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
