#include <klib-macros.h>
#include <klib.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    size_t n = 0;
    char *cur = (char *)s;
    for (;;) {
        if (*cur != '\0') {
            cur += 1;
            n += 1;
        } else {
            break;
        }
    }
    return n;
}

char *strcpy(char *dst, const char *src) {
    size_t n2 = strlen(src);
    memcpy(dst, src, n2 + 1);
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) { panic("Not implemented"); }

char *strcat(char *dst, const char *src) {
    size_t n1 = strlen(dst);
    size_t n2 = strlen(src);

    memcpy(dst + n1, src, n2);
    dst[n1 + n2] = '\0';
    return dst;
}

int strcmp(const char *s1, const char *s2) {
    size_t i = 0;
    for (;;) {
        char c1, c2;
        c1 = s1[i];
        c2 = s2[i];
        if (c1 != c2)
            return -1;
        if (c1 == '\0' && c2 == '\0') {
            return 0;
        }
        i++;
    }
    return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) { panic("Not implemented"); }

void *memset(void *s, int c, size_t n) {
    char *src = (char *)s;
    for (int i = 0; i < n; i++) {
        src[i] = c;
    }
    return s;
}

void *memmove(void *dst, const void *src, size_t n) { panic("Not implemented"); }

void *memcpy(void *out, const void *in, size_t n) {
    // *in and *out don't overlap
    char *src = (char *)in;
    char *dest = (char *)out;
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
    return 0;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    char *cs1 = (char *)s1;
    char *cs2 = (char *)s2;
    for (size_t i = 0; i < n; i++) {
        char c1, c2;
        c1 = cs1[i];
        c2 = cs2[i];
        if (c1 != c2)
            return -1;
    }
    return 0;
}

#endif
