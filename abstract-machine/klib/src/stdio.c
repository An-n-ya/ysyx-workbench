#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) { 
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    int len = vsprintf(buf, fmt, args);
    va_end(args);
    if (len != 0) {
        putstr(buf);
    }
    return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) { 
    char *cur = out;
    const char *fmt_ptr = fmt;
    while (*fmt_ptr) {
        if (*fmt_ptr == '%') {
            fmt_ptr++;
            switch (*fmt_ptr) {
            case 'd': {
                int v = va_arg(ap, int);
                char temp[20];
                itoa(v, temp);
                for (char *p = temp; *p; p++) {
                    *cur++ = *p;
                }
                break;
            }
            case 's': {
                char *s = va_arg(ap, char *);
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
    return (int)(cur - out);
}

int sprintf(char *out, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    return vsprintf(out, fmt, args);
}

int snprintf(char *out, size_t n, const char *fmt, ...) { panic("Not implemented"); }

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) { panic("Not implemented"); }

#endif
