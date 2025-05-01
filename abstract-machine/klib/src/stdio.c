#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) { panic("Not implemented"); }

int vsprintf(char *out, const char *fmt, va_list ap) { panic("Not implemented"); }

int sprintf(char *out, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char *cur = out;
    const char *fmt_ptr = fmt;
    while (*fmt_ptr) {
        if (*fmt_ptr == '%') {
            fmt_ptr++;
            switch (*fmt_ptr) {
            case 'd': {
                int v = va_arg(args, int);
                char temp[20];
                itoa(v, temp);
                for (char *p = temp; *p; p++) {
                    *cur++ = *p;
                }
                break;
            }
            case 's': {
                char *s = va_arg(args, char *);
                while (*s) {
                    *cur++ = *s++;
                }
                break;
            }
            default: {
                *cur++ = '%';
                *cur++ = *fmt_ptr;
                break;
            }
            }
        } else {
            *cur++ = *fmt_ptr;
        }
        fmt_ptr++;
    }
    *cur = '\0';
    va_end(args);
    return (int)(cur - out);
}

int snprintf(char *out, size_t n, const char *fmt, ...) { panic("Not implemented"); }

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) { panic("Not implemented"); }

#endif
