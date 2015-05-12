// info: display basic info

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(void)
{
    printf(1, "Size of char:  %d byte%s\n", sizeof(char), sizeof(char) > 1 ? "s" : "");
    printf(1, "Size of short: %d byte%s\n", sizeof(short), sizeof(short) > 1 ? "s" : "");
    printf(1, "Size of int:   %d byte%s\n", sizeof(int), sizeof(int) > 1 ? "s" : "");
    printf(1, "Size of long:  %d byte%s\n", sizeof(long), sizeof(long) > 1 ? "s" : "");
    exit();
}
