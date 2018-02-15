# fb64d
Fast Base64 Decoder in C

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
