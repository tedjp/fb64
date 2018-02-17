#ifndef FB64_COMMON_H
#define FB64_COMMON_H

__attribute__((__const__))
inline unsigned char next(unsigned char c) {
    switch (c) {
    case 'Z':
        return 'a';
    case 'z':
        return '0';
    case '9':
        return '+';
    case '+':
        return '/';
    default:
        return ++c;
    }
}

void fb64_init();

__attribute__((__visibility__("internal")))
void decode_init();

__attribute__((__visibility__("internal")))
void encode_init();

#endif
