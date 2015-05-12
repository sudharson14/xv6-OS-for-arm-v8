#include "types.h"
#include "arm.h"


void* memset(void *dst, int v, int n)
{
    uint8	*p;
    uint8	c;
    uint32	val;
    uint32	*p4;

    p   = dst;
    c   = v & 0xff;
    val = (c << 24) | (c << 16) | (c << 8) | c;

    // set bytes before whole uint32
    for (; (n > 0) && ((uint64)p % 4); n--, p++){
        *p = c;
    }

    // set memory 4 bytes a time
    p4 = (uint*)p;

    for (; n >= 4; n -= 4, p4++) {
        *p4 = val;
    }

    // set leftover one byte a time
    p = (uint8*)p4;

    for (; n > 0; n--, p++) {
        *p = c;
    }

    return dst;
}


int memcmp(const void *v1, const void *v2, uint n)
{
    const uchar *s1, *s2;

    s1 = v1;
    s2 = v2;

    while(n-- > 0){
        if(*s1 != *s2) {
            return *s1 - *s2;
        }

        s1++, s2++;
    }

    return 0;
}

void* memmove(void *dst, const void *src, uint n)
{
    const char *s;
    char *d;

    s = src;
    d = dst;

    if(s < d && s + n > d){
        s += n;
        d += n;

        while(n-- > 0) {
            *--d = *--s;
        }

    } else {
        while(n-- > 0) {
            *d++ = *s++;
        }
    }

    return dst;
}

// memcpy exists to placate GCC.  Use memmove.
void* memcpy(void *dst, const void *src, uint n)
{
    return memmove(dst, src, n);
}

int strncmp(const char *p, const char *q, uint n)
{
    while(n > 0 && *p && *p == *q) {
        n--, p++, q++;
    }

    if(n == 0) {
        return 0;
    }

    return (uchar)*p - (uchar)*q;
}

char* strncpy(char *s, const char *t, int n)
{
    char *os;

    os = s;

    while(n-- > 0 && (*s++ = *t++) != 0)
        ;

    while(n-- > 0) {
        *s++ = 0;
    }

    return os;
}

// Like strncpy but guaranteed to NUL-terminate.
char* safestrcpy(char *s, const char *t, int n)
{
    char *os;

    os = s;

    if(n <= 0) {
        return os;
    }

    while(--n > 0 && (*s++ = *t++) != 0)
        ;

    *s = 0;
    return os;
}

int strlen(const char *s)
{
    int n;

    for(n = 0; s[n]; n++)
        ;

    return n;
}

