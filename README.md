# fb64

Fast Base64 Encoder & Decoder

Features
- Supports both [base64 & base64url encodings](https://tools.ietf.org/html/rfc4648).
- Supports both padded & unpadded input & output.
- Small memory footprint.
- *Fast.*

## Execute

When built or installed, fb64 can be used for command-line encoding & decoding:

    $ echo asdf | fb64
    YXNkZgo=

    $ echo YXNkZgo= | fb64 -d
    asdf

`fb64` may be preferable over GNU coreutils' `base64` if you want to decode
unpadded or base64url input, both of which require special handling
to decode with coreutils' `base64`.

    $ echo ßß | fb64
    w5/Dnwo=

    $ echo W5_Dnwo | fb64 -d
    ßß

coreutils' `base64` is less accommodating:

    $ echo w5_Dnwo | base64 -d
    �base64: invalid input

## Examples

### Decode

```c
const char input[] = "SGVsbG8sIHdvcmxkIQ==";

size_t decoded_size = fb64_decoded_size(input, strlen(input));
uint8_t output[decoded_size];

fb64_decode(input, strlen(input), output);

printf("%.*s\n", (int)decoded_size, output);
```

### Encode

```c
const uint8_t input[] = {'f', 'b', '6', '4', ' ', 'i', 's', ' ', 'f', 'a', 's', 't', '!'};

size_t output_size = fb64_encoded_size(sizeof(input));

char output[output_size + 1]; // include space for NUL-terminator

fb64_encode(input, sizeof(input), output);

// Manually terminate the C-string
output[output_size] = '\0';

printf("%s\n", output);
```

## Benchmarks

Both encode & decode benchmarks are for 1 kiB of random data.

|Decoder                      |    Time      | Iterations |
|:----------------------------|-------------:|-----------:|
|modp\_b64                    |    572 ns    |  1150750   |
|fb64                         |   1273 ns    |   538302   |
|fb64 string                  |   1302 ns    |   524609   |
|Proxygen/OpenSSL             |   2843 ns    |   243047   |
|Boost                        |  10024 ns    |    67508   |

|Encoder                      |    Time      | Iterations |
|:----------------------------|-------------:|-----------:|
|modp\_b64                    |    839 ns    |   802592   |
|fb64                         |    947 ns    |   716427   |
|Boost                        |   9026 ns    |    74113   |

The "fb64 string" variant wraps the input & output in a `std::string`
for a more direct comparison with the Proxygen/OpenSSL API.

## Advanced usage

fb64 can be used to encode or decode streams or large amounts of data by calling
the encode/decode functions on blocks of input aligned on encode/decode quantum
boundaries. Encode input should be split on 3-octet boundaries; decode input
should be split on 4-character boundaries.

## Limitations

1. `fb64_decode()` does not accept newlines in its input. It might still be faster
   than other decoders if the input is preprocessed to delete newlines.

2. The decode lookup tables are initialzed dynamically at program startup.
   In future they could be hard-coded.

3. Both base64 and base64url symbols are accepted equally. ie. input may contain
   a mix of +, /, - and _ as the last two symbols which will not trigger a
   decode error. If you need a strict decoder that will only accept one set of
   symbols you might like to modify the lookup tables to delete the symbols
   that you don't want, or make a second pass over the input to validate that
   it conforms to your requirements. See [RFC 4648 section 12 "Security
   Considerations"](https://tools.ietf.org/html/rfc4648#section-12).

## Bugs

1. The buffer length functions `fb64_decoded_size()` &
   `fb64_decoded_size_nopad()` return incorrect buffer lengths for inputs longer
   than ⅓ of `SIZE_MAX`. That's about 6 exabytes (6×10¹⁸) on a 64-bit machine.
   The preprocessor macros `FB64_DECODE_MAX` & `FB64_ENCODE_MAX` can be used to
   ensure that these limits are not exceeded.

## Comparison with modp\_b64

[modp\_b64](https://github.com/chromium/chromium/tree/master/third_party/modp_b64)
is faster. It also uses more memory.

|Library  |Decode memory|Decode speed|Encode memory|Encode speed|Total static footprint|
|---------|------------:|-----------:|------------:|-----------:|---------------------:|
|fb64     |        1 kiB|          1×|      128 kiB|          1×|             1.125 kiB|
|modp\_b64|        4 kiB|       2.15×|      768 kiB|        1.1×|             4.75  kiB|

### Tradeoffs

fb64 benefits:
- Supports base64 & base64url code sets simultaneously
  for both encode & decode (modp\_b64 only supports regular base64).
- Padding is optional at runtime
  (modp\_b64 requires padding to be selected at compile time or to be implemented
  by the caller).
- Output buffer size is exact
  (modp\_b64 may over-allocate by a few bytes).
- Smaller memory footprint. All tables fit within one 4 kiB page and take up
  less L1 cache; useful if you only encode/decode occasionally.

modp\_b64 benefits:
- Faster, especially at decoding.

## License

[MIT License](LICENSE)
